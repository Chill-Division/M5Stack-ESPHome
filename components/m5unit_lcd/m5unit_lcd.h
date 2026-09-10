#pragma once

#include "esphome/components/display/display_buffer.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"

namespace esphome::m5unit_lcd {

/// M5Stack Unit LCD (U120): 135x240 ST7789V2 behind an I2C command interpreter.
///
/// Rendering happens into a local RGB565 framebuffer (ESPHome's DisplayBuffer),
/// and on each update only the rows that changed since the previous flush are
/// pushed to the unit with CASET/RASET + WRITE_RAW_16 commands. Rotation is
/// handled in software by DisplayBuffer, so the panel itself stays in its
/// native portrait orientation.
class M5UnitLCD : public display::DisplayBuffer, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::PROCESSOR; }
  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_COLOR; }

  /// Backlight brightness, 0-255. Applied immediately if the display is already running.
  void set_brightness(uint8_t brightness);
  /// Invert panel colours.
  void set_invert_colors(bool invert);
  /// Put the panel to sleep (true) or wake it (false).
  void set_sleep(bool sleep);

  void fill(Color color) override;

 protected:
  static constexpr int WIDTH = 135;
  static constexpr int HEIGHT = 240;
  static constexpr size_t BYTES_PER_PIXEL = 2;
  /// Pixels of raw data per I2C transaction (512 bytes). M5GFX sends up to a
  /// 240px line per transaction, so this is within what the firmware handles.
  static constexpr size_t MAX_PIXELS_PER_TX = 256;
  static constexpr size_t TX_HEADER_LEN = 7;  // CASET(3) + RASET(3) + WRITE_RAW_16(1)

  int get_width_internal() override { return WIDTH; }
  int get_height_internal() override { return HEIGHT; }
  void draw_absolute_pixel_internal(int x, int y, Color color) override;

  bool read_id_();
  bool send_(const uint8_t *data, size_t len);
  bool send_config_();
  /// Make sure the unit's command buffer has room for a command of the given
  /// cost, polling READ_BUFCOUNT when our running estimate gets low.
  bool reserve_(int cost);
  bool row_dirty_(int y, int &x0, int &x1) const;
  bool send_region_(int xs, int y0, int xe, int y1);
  bool emit_tx_(size_t pixels);
  void flush_();

  uint8_t *prev_{nullptr};
  uint8_t tx_[TX_HEADER_LEN + MAX_PIXELS_PER_TX * BYTES_PER_PIXEL];
  size_t tx_len_{0};
  uint8_t brightness_{255};
  bool invert_{false};
  bool initialized_{false};
  int buf_free_{0};
  uint8_t fw_major_{0};
  uint8_t fw_minor_{0};
};

}  // namespace esphome::m5unit_lcd
