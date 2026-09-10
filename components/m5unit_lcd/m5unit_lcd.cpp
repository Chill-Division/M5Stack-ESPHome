#include "m5unit_lcd.h"

#include <algorithm>
#include <cstring>

#include "esphome/core/application.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome::m5unit_lcd {

static const char *const TAG = "m5unit_lcd";

// Unit LCD I2C command set. Reference: https://docs.m5stack.com/en/unit/lcd
// and M5GFX's Panel_M5UnitLCD driver.
static const uint8_t CMD_READ_ID = 0x04;        // -> 0x77 0x89 <major> <minor>
static const uint8_t CMD_READ_BUFCOUNT = 0x09;  // -> free command-buffer slots (0-255)
static const uint8_t CMD_INVOFF = 0x20;
static const uint8_t CMD_INVON = 0x21;
static const uint8_t CMD_BRIGHTNESS = 0x22;  // [1]=0-255
static const uint8_t CMD_CASET = 0x2A;       // [1]=xs [2]=xe
static const uint8_t CMD_RASET = 0x2B;       // [1]=ys [2]=ye
static const uint8_t CMD_ROTATE = 0x36;      // [1]=0-7
static const uint8_t CMD_SET_POWER = 0x38;   // [1]=0 low / 1 normal / 2 high
static const uint8_t CMD_SET_SLEEP = 0x39;   // [1]=0 wake / 1 sleep
static const uint8_t CMD_WRITE_RAW_16 = 0x42;  // RGB565 pixels, big-endian, until STOP
static const uint8_t CMD_FILLRECT_16 = 0x6A;   // [1]=xs [2]=ys [3]=xe [4]=ye [5-6]=RGB565

// Command-buffer accounting. The unit reports free slots as a 0-255 count;
// M5GFX budgets roughly one slot per four pixels of image data and tops up
// when it gets below 64. We are a little more conservative.
static const int BUFCOUNT_LOW_WATER = 48;
static const int BUFCOUNT_REFILL = 128;
static const uint32_t BUFCOUNT_TIMEOUT_MS = 500;

void M5UnitLCD::setup() {
  if (!this->read_id_()) {
    ESP_LOGE(TAG, "No Unit LCD found at 0x%02X (bad or missing ID response)", this->address_);
    this->mark_failed();
    return;
  }

  const size_t len = WIDTH * HEIGHT * BYTES_PER_PIXEL;
  this->init_internal_(len);  // allocates buffer_ and clears it to black
  if (this->buffer_ == nullptr) {
    this->mark_failed();
    return;
  }
  RAMAllocator<uint8_t> allocator;
  this->prev_ = allocator.allocate(len);
  if (this->prev_ == nullptr) {
    ESP_LOGE(TAG, "Could not allocate shadow buffer");
    this->mark_failed();
    return;
  }
  memset(this->prev_, 0, len);

  if (!this->send_config_()) {
    ESP_LOGE(TAG, "Failed to configure panel");
    this->mark_failed();
    return;
  }
  // Blank the panel so it matches the (all-black) shadow buffer.
  const uint8_t blank[] = {CMD_FILLRECT_16, 0, 0, WIDTH - 1, HEIGHT - 1, 0x00, 0x00};
  this->send_(blank, sizeof(blank));
  this->initialized_ = true;
}

void M5UnitLCD::dump_config() {
  LOG_DISPLAY("", "M5Stack Unit LCD", this);
  LOG_I2C_DEVICE(this);
  ESP_LOGCONFIG(TAG,
                "  Panel: %dx%d ST7789V2 (I2C bridge)\n"
                "  Firmware: %u.%u\n"
                "  Brightness: %u/255\n"
                "  Invert colors: %s",
                WIDTH, HEIGHT, this->fw_major_, this->fw_minor_, this->brightness_, YESNO(this->invert_));
  LOG_UPDATE_INTERVAL(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "  Setup failed");
  }
}

void M5UnitLCD::update() {
  if (!this->initialized_)
    return;
  this->do_update_();
  this->flush_();
}

void M5UnitLCD::set_brightness(uint8_t brightness) {
  this->brightness_ = brightness;
  if (this->initialized_) {
    const uint8_t cmd[] = {CMD_BRIGHTNESS, brightness};
    this->reserve_(1);
    this->send_(cmd, sizeof(cmd));
  }
}

void M5UnitLCD::set_invert_colors(bool invert) {
  this->invert_ = invert;
  if (this->initialized_) {
    const uint8_t cmd = invert ? CMD_INVON : CMD_INVOFF;
    this->reserve_(1);
    this->send_(&cmd, 1);
  }
}

void M5UnitLCD::set_sleep(bool sleep) {
  if (!this->initialized_)
    return;
  const uint8_t cmd[] = {CMD_SET_SLEEP, static_cast<uint8_t>(sleep ? 1 : 0)};
  this->reserve_(1);
  this->send_(cmd, sizeof(cmd));
}

void M5UnitLCD::fill(Color color) {
  if (this->buffer_ == nullptr)
    return;
  const uint16_t c = display::ColorUtil::color_to_565(color);
  const uint8_t hi = c >> 8, lo = c & 0xFF;
  const size_t len = WIDTH * HEIGHT * BYTES_PER_PIXEL;
  if (hi == lo) {
    memset(this->buffer_, hi, len);
    return;
  }
  for (size_t i = 0; i < len; i += 2) {
    this->buffer_[i] = hi;
    this->buffer_[i + 1] = lo;
  }
}

void HOT M5UnitLCD::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || this->buffer_ == nullptr)
    return;
  const uint16_t c = display::ColorUtil::color_to_565(color);
  const size_t i = (static_cast<size_t>(y) * WIDTH + x) * BYTES_PER_PIXEL;
  this->buffer_[i] = c >> 8;
  this->buffer_[i + 1] = c & 0xFF;
}

bool M5UnitLCD::read_id_() {
  uint8_t buf[4] = {0, 0, 0, 0};
  if (this->write_read(&CMD_READ_ID, 1, buf, sizeof(buf)) != i2c::ERROR_OK)
    return false;
  if (buf[0] != 0x77 || buf[1] != 0x89) {
    ESP_LOGW(TAG, "Unexpected ID bytes: %02X %02X %02X %02X", buf[0], buf[1], buf[2], buf[3]);
    return false;
  }
  this->fw_major_ = buf[2];
  this->fw_minor_ = buf[3];
  return true;
}

bool M5UnitLCD::send_(const uint8_t *data, size_t len) {
  const auto err = this->write(data, len);
  if (err != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "I2C write of %zu bytes failed (%d)", len, static_cast<int>(err));
    this->status_set_warning();
    return false;
  }
  this->status_clear_warning();
  return true;
}

bool M5UnitLCD::send_config_() {
  // Fixed-length commands may be concatenated in one transaction.
  const uint8_t cfg[] = {
      CMD_SET_SLEEP, 0,                            // wake
      CMD_SET_POWER, 1,                            // normal power
      CMD_ROTATE,    0,                            // native portrait; ESPHome rotates in software
      CMD_BRIGHTNESS, this->brightness_,           //
      this->invert_ ? CMD_INVON : CMD_INVOFF,      //
  };
  this->reserve_(5);
  return this->send_(cfg, sizeof(cfg));
}

bool M5UnitLCD::reserve_(int cost) {
  if (this->buf_free_ >= cost + BUFCOUNT_LOW_WATER) {
    this->buf_free_ -= cost;
    return true;
  }
  const uint32_t start = millis();
  while (true) {
    uint8_t free_slots = 0;
    if (this->write_read(&CMD_READ_BUFCOUNT, 1, &free_slots, 1) == i2c::ERROR_OK) {
      if (free_slots >= BUFCOUNT_REFILL) {
        this->buf_free_ = free_slots - cost;
        return true;
      }
    }
    if (millis() - start > BUFCOUNT_TIMEOUT_MS) {
      ESP_LOGW(TAG, "Timed out waiting for panel command buffer (free=%u)", free_slots);
      this->buf_free_ = 0;
      return false;
    }
    delay(2);
    App.feed_wdt();
  }
}

bool M5UnitLCD::row_dirty_(int y, int &x0, int &x1) const {
  const size_t row = static_cast<size_t>(y) * WIDTH * BYTES_PER_PIXEL;
  const uint8_t *cur = this->buffer_ + row;
  const uint8_t *old = this->prev_ + row;
  const size_t row_bytes = WIDTH * BYTES_PER_PIXEL;
  if (memcmp(cur, old, row_bytes) == 0)
    return false;
  size_t first = 0;
  while (cur[first] == old[first])
    first++;
  size_t last = row_bytes - 1;
  while (cur[last] == old[last])
    last--;
  x0 = static_cast<int>(first / BYTES_PER_PIXEL);
  x1 = static_cast<int>(last / BYTES_PER_PIXEL);
  return true;
}

bool M5UnitLCD::emit_tx_(size_t pixels) {
  // Header (if present) is 3 fixed-length commands; image data costs ~1 slot per 4 px.
  const int cost = 3 + static_cast<int>(pixels / 4) + 1;
  this->reserve_(cost);
  const bool ok = this->send_(this->tx_, this->tx_len_);
  // Subsequent chunks continue the same window: just restart WRITE_RAW_16.
  this->tx_[0] = CMD_WRITE_RAW_16;
  this->tx_len_ = 1;
  App.feed_wdt();
  return ok;
}

bool M5UnitLCD::send_region_(int xs, int y0, int xe, int y1) {
  const int w = xe - xs + 1;
  bool ok = true;

  this->tx_[0] = CMD_CASET;
  this->tx_[1] = static_cast<uint8_t>(xs);
  this->tx_[2] = static_cast<uint8_t>(xe);
  this->tx_[3] = CMD_RASET;
  this->tx_[4] = static_cast<uint8_t>(y0);
  this->tx_[5] = static_cast<uint8_t>(y1);
  this->tx_[6] = CMD_WRITE_RAW_16;
  this->tx_len_ = TX_HEADER_LEN;
  size_t px_in_tx = 0;

  for (int y = y0; y <= y1; y++) {
    const uint8_t *src = this->buffer_ + (static_cast<size_t>(y) * WIDTH + xs) * BYTES_PER_PIXEL;
    int x = 0;
    while (x < w) {
      const int n = std::min<int>(w - x, static_cast<int>(MAX_PIXELS_PER_TX - px_in_tx));
      memcpy(this->tx_ + this->tx_len_, src + x * BYTES_PER_PIXEL, n * BYTES_PER_PIXEL);
      this->tx_len_ += n * BYTES_PER_PIXEL;
      px_in_tx += n;
      x += n;
      if (px_in_tx >= MAX_PIXELS_PER_TX) {
        ok = this->emit_tx_(px_in_tx) && ok;
        px_in_tx = 0;
      }
    }
  }
  if (px_in_tx > 0) {
    ok = this->emit_tx_(px_in_tx) && ok;
  }

  if (ok) {
    // Region is now on the panel; remember it so it is not resent.
    for (int y = y0; y <= y1; y++) {
      const size_t off = (static_cast<size_t>(y) * WIDTH + xs) * BYTES_PER_PIXEL;
      memcpy(this->prev_ + off, this->buffer_ + off, w * BYTES_PER_PIXEL);
    }
  }
  return ok;
}

void M5UnitLCD::flush_() {
  const uint32_t start = millis();
  size_t regions = 0, pixels = 0;
  int y = 0;
  while (y < HEIGHT) {
    int x0, x1;
    if (!this->row_dirty_(y, x0, x1)) {
      y++;
      continue;
    }
    // Grow a run of consecutive dirty rows, tracking the union of their spans.
    int y1 = y;
    while (y1 + 1 < HEIGHT) {
      int a, b;
      if (!this->row_dirty_(y1 + 1, a, b))
        break;
      x0 = std::min(x0, a);
      x1 = std::max(x1, b);
      y1++;
    }
    this->send_region_(x0, y, x1, y1);
    regions++;
    pixels += static_cast<size_t>(x1 - x0 + 1) * (y1 - y + 1);
    y = y1 + 1;
  }
  if (regions > 0) {
    ESP_LOGV(TAG, "Flushed %zu region(s), %zu px in %ums", regions, pixels, (unsigned) (millis() - start));
  }
}

}  // namespace esphome::m5unit_lcd
