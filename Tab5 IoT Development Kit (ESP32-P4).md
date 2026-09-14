### Tab5 IoT Development Kit (ESP32-P4)

https://shop.m5stack.com/products/m5stack-tab5-iot-development-kit-esp32-p4

<b>NOTE:</b> The ESP32-P4 has no radio of its own. Wi-Fi and Bluetooth come from an on-board ESP32-C6 reached over SDIO, so <code>esp32_hosted</code> is required and a <code>wifi:</code> block on its own will not connect. The <code>WiFi Power</code> rail below must also stay on, or the C6 link drops.

Needs ESPHome 2026.4 or newer on the ESP-IDF framework with experimental features enabled.

This is the generic template to bring the board up: radio, PSRAM, 720 x 1280 MIPI-DSI panel, backlight, RTC, battery monitor and the power rails, without any sensors plugged in.

<pre>
substitutions:
  name: m5stack-tab5
  friendly_name: M5Stack Tab5

esphome:
  name: ${name}
  friendly_name: ${friendly_name}
  on_boot:
    then:
      rx8130.read_time:

esp32:
  variant: esp32p4
  flash_size: 16MB
  framework:
    type: esp-idf
    advanced:
      enable_idf_experimental_features: true
  # Drop this line if you have a production-revision board and it won't boot
  engineering_sample: true

# Wi-Fi / BT live on the on-board ESP32-C6, reached over SDIO
esp32_hosted:
  variant: esp32c6
  active_high: true
  clk_pin: GPIO12
  cmd_pin: GPIO13
  d0_pin: GPIO11
  d1_pin: GPIO10
  d2_pin: GPIO9
  d3_pin: GPIO8
  reset_pin: GPIO15
  slot: 1

logger:
  hardware_uart: USB_SERIAL_JTAG

psram:
  mode: hex
  speed: 200MHz

# Board-support I2C bus: RTC, INA226, GPIO expanders
i2c:
  - id: bsp_bus
    sda: GPIO31
    scl: GPIO32
    frequency: 400kHz

# 2.5V rail required by the MIPI-DSI PHY
esp_ldo:
  - voltage: 2.5V
    channel: 3

# GPIO expanders: power rails and display reset
pi4ioe5v6408:
  - id: pi4ioe1
    address: 0x43
  - id: pi4ioe2
    address: 0x44

output:
  - platform: ledc
    pin: GPIO22
    id: backlight_pwm
    frequency: 1000Hz

light:
  - platform: monochromatic
    output: backlight_pwm
    name: "Display Backlight"
    id: backlight
    restore_mode: RESTORE_DEFAULT_ON
    default_transition_length: 250ms

time:
  # Battery-backed RTC, read once at boot so the clock is sane before Wi-Fi
  - platform: rx8130
    id: rtc_time
    i2c_id: bsp_bus
    update_interval: never
    timezone: UTC
  - platform: homeassistant
    id: ha_time

switch:
  # Leave this ON - switching it off kills the C6 radio
  - platform: gpio
    id: wifi_power
    name: "WiFi Power"
    pin:
      pi4ioe5v6408: pi4ioe2
      number: 0
    restore_mode: ALWAYS_ON
  - platform: gpio
    id: charge_enable
    name: "Charge Enable"
    pin:
      pi4ioe5v6408: pi4ioe2
      number: 7
    restore_mode: ALWAYS_ON
  - platform: gpio
    id: external_5v_power
    name: "External 5V Power"
    pin:
      pi4ioe5v6408: pi4ioe1
      number: 2

sensor:
  - platform: ina226
    address: 0x41
    adc_averaging: 16
    max_current: 8.192A
    shunt_resistance: 0.005ohm
    bus_voltage:
      id: battery_voltage
      name: "Battery Voltage"
    current:
      id: battery_current
      name: "Battery Current"

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
  ap: {}

captive_portal:

api:
  encryption:
    key: !secret api_encryption_key

ota:
  - platform: esphome
    password: !secret ota_password

font:
  - file: "gfonts://Roboto@600"
    id: font_title
    size: 34

display:
  - platform: mipi_dsi
    id: tab5_display
    dimensions:
      height: 1280
      width: 720
    model: M5STACK-TAB5-ST7121
    reset_pin:
      pi4ioe5v6408: pi4ioe1
      number: 4
    update_interval: 10s
    lambda: |-
      it.fill(Color(0x0E, 0x11, 0x16));
      it.printf(28, 22, id(font_title), Color(0xEA, 0xF0, 0xF6), "Hello from Tab5");
</pre>

No touch controller is configured above, so the panel is output-only as written. Redraws are a full-screen software render, so keep <code>update_interval</code> at 5s or more.

---

### Optional: sparkline dashboard

LVGL's chart widget is compiled out of ESPHome 2026.4+, so anything graph-like has to be drawn in a display lambda. That turns out to be no bad thing: drawing per-pixel columns gives a gradient area fill the LVGL chart can't produce.

This example pulls six values from Home Assistant and draws each as a filled sparkline card, with an optional "healthy" reference band. Swap the <code>entity_id:</code> values for your own.

History is sampled on-device into ring buffers and is lost on reboot; Home Assistant's long-term history can't be pulled onto the device. 160 points at 30s is about 80 minutes per card.

Add to the config above:

<pre>
globals:
  - id: temp_hist
    type: std::vector&lt;float&gt;
    restore_value: no
  - id: hum_hist
    type: std::vector&lt;float&gt;
    restore_value: no
  - id: press_hist
    type: std::vector&lt;float&gt;
    restore_value: no
  - id: vpd_hist
    type: std::vector&lt;float&gt;
    restore_value: no
  - id: co2_hist
    type: std::vector&lt;float&gt;
    restore_value: no
  - id: vwc_hist
    type: std::vector&lt;float&gt;
    restore_value: no

sensor:
  - platform: homeassistant
    id: ha_temp
    entity_id: sensor.room_temperature
  - platform: homeassistant
    id: ha_humidity
    entity_id: sensor.room_humidity
  - platform: homeassistant
    id: ha_pressure
    entity_id: sensor.room_pressure
  - platform: homeassistant
    id: ha_vpd
    entity_id: sensor.room_vpd
  - platform: homeassistant
    id: ha_co2
    entity_id: sensor.room_co2
  - platform: homeassistant
    id: ha_vwc
    entity_id: sensor.room_vwc

# Sample the current values into the ring buffers
interval:
  - interval: 30s
    then:
      - lambda: |-
          const size_t MAXN = 160;
          if (!std::isnan(id(ha_temp).state)) {
            id(temp_hist).push_back(id(ha_temp).state);
            if (id(temp_hist).size() &gt; MAXN) id(temp_hist).erase(id(temp_hist).begin());
          }
          if (!std::isnan(id(ha_humidity).state)) {
            id(hum_hist).push_back(id(ha_humidity).state);
            if (id(hum_hist).size() &gt; MAXN) id(hum_hist).erase(id(hum_hist).begin());
          }
          if (!std::isnan(id(ha_pressure).state)) {
            id(press_hist).push_back(id(ha_pressure).state);
            if (id(press_hist).size() &gt; MAXN) id(press_hist).erase(id(press_hist).begin());
          }
          if (!std::isnan(id(ha_vpd).state)) {
            id(vpd_hist).push_back(id(ha_vpd).state);
            if (id(vpd_hist).size() &gt; MAXN) id(vpd_hist).erase(id(vpd_hist).begin());
          }
          if (!std::isnan(id(ha_co2).state)) {
            id(co2_hist).push_back(id(ha_co2).state);
            if (id(co2_hist).size() &gt; MAXN) id(co2_hist).erase(id(co2_hist).begin());
          }
          if (!std::isnan(id(ha_vwc).state)) {
            id(vwc_hist).push_back(id(ha_vwc).state);
            if (id(vwc_hist).size() &gt; MAXN) id(vwc_hist).erase(id(vwc_hist).begin());
          }

font:
  - file: "gfonts://Roboto@500"
    id: font_label
    size: 28
  - file: "gfonts://Roboto@700"
    id: font_value
    size: 48
    # Restricted glyph set to save flash. Extend it if you add a unit that
    # uses other characters, or that text will render blank.
    glyphs: " -.:%°CkPapmh0123456789"
</pre>

Then replace the <code>display:</code> lambda with:

<pre>
    lambda: |-
      const Color page_bg = Color(0x0E, 0x11, 0x16);
      const Color card_bg = Color(0x12, 0x16, 0x1E);
      const int W = it.get_width();
      it.fill(page_bg);

      auto lerp = [](Color a, Color b, float t) -&gt; Color {
        if (t &lt; 0) t = 0; if (t &gt; 1) t = 1;
        return Color((uint8_t)(a.r + (b.r - a.r) * t),
                     (uint8_t)(a.g + (b.g - a.g) * t),
                     (uint8_t)(a.b + (b.b - a.b) * t));
      };

      auto fill_round = [&amp;](int x, int y, int w, int h, int r, Color c) {
        it.filled_rectangle(x + r, y, w - 2 * r, h, c);
        it.filled_rectangle(x, y + r, w, h - 2 * r, c);
        it.filled_circle(x + r, y + r, r, c);
        it.filled_circle(x + w - r - 1, y + r, r, c);
        it.filled_circle(x + r, y + h - r - 1, r, c);
        it.filled_circle(x + w - r - 1, y + h - r - 1, r, c);
      };

      auto draw_card = [&amp;](int x, int y, int w, int h,
                           std::vector&lt;float&gt; &amp;hist, const char *label,
                           const char *value_str, Color line_col, Color mid_col,
                           Color value_col, float fix_min, float fix_max,
                           float band_lo, float band_hi) {
        const int r = 22;
        fill_round(x, y, w, h, r, card_bg);

        const int pad = 26;
        const int gx = x + pad;
        const int gw = w - 2 * pad;
        const int bot = y + h - 14;
        const int y_hi = y + (int)(h * 0.15f);
        const int y_lo = y + (int)(h * 0.50f);

        size_t n = hist.size();

        // Fixed range when fix_max &gt; fix_min, otherwise auto-scale to the data
        bool have_scale = false;
        float mn = 0, mx = 1;
        if (fix_max &gt; fix_min) {
          mn = fix_min; mx = fix_max; have_scale = true;
        } else if (n &gt;= 1) {
          mn = hist[0]; mx = hist[0];
          for (size_t i = 0; i &lt; n; i++) {
            if (hist[i] &lt; mn) mn = hist[i];
            if (hist[i] &gt; mx) mx = hist[i];
          }
          if (mx - mn &lt; 0.5f) mx = mn + 0.5f;
          have_scale = true;
        }

        auto yat = [&amp;](float v) -&gt; int {
          float f = (v - mn) / (mx - mn);
          if (f &lt; 0) f = 0; if (f &gt; 1) f = 1;
          return (int)(y_lo - f * (y_lo - y_hi));
        };

        // Healthy reference band behind the curve (fixed scale only)
        if (have_scale &amp;&amp; band_hi &gt; band_lo) {
          int by_top = yat(band_hi);
          int by_bot = yat(band_lo);
          if (by_top &lt; y_hi) by_top = y_hi;
          if (by_bot &gt; bot) by_bot = bot;
          if (by_bot &gt; by_top) {
            it.filled_rectangle(gx, by_top, gw, by_bot - by_top, Color(0x16, 0x2A, 0x1C));
            it.line(gx, by_top, gx + gw, by_top, Color(0x3A, 0x6B, 0x45));
            it.line(gx, by_bot, gx + gw, by_bot, Color(0x3A, 0x6B, 0x45));
          }
        }

        // Gradient area fill, then the line on top
        if (n &gt;= 2 &amp;&amp; have_scale) {
          auto grad = [&amp;](float g) -&gt; Color {
            if (g &lt; 0.5f) return lerp(line_col, mid_col, g / 0.5f);
            return lerp(mid_col, card_bg, (g - 0.5f) / 0.5f);
          };

          int prev_cy = 0;
          for (int px = gx; px &lt; gx + gw; px++) {
            float fi = (float)(px - gx) / (float)(gw - 1) * (float)(n - 1);
            int i0 = (int)fi;
            int i1 = (i0 + 1 &lt; (int)n) ? i0 + 1 : (int)n - 1;
            float ft = fi - i0;
            float v = hist[i0] + (hist[i1] - hist[i0]) * ft;
            int cy = yat(v);

            int hh = bot - cy; if (hh &lt; 1) hh = 1;
            const int K = 20;
            for (int j = 0; j &lt; K; j++) {
              int ya = cy + (int)(((float)j / K) * hh);
              int yb = cy + (int)(((float)(j + 1) / K) * hh);
              it.line(px, ya, px, yb, grad((j + 0.5f) / K));
            }

            if (px &gt; gx) {
              it.line(px - 1, prev_cy, px, cy, line_col);
              it.line(px - 1, prev_cy - 1, px, cy - 1, line_col);
              it.line(px - 1, prev_cy - 2, px, cy - 2, line_col);
            }
            prev_cy = cy;
          }
        }

        it.printf(x + 30, y + h - 80, id(font_label), Color(0xC5, 0xD0, 0xDD), "%s", label);
        it.printf(x + 30, y + h - 52, id(font_value), value_col, "%s", value_str);
      };

      // Header. Change "Room 1" to suit.
      it.printf(28, 22, id(font_title), Color(0xEA, 0xF0, 0xF6), "Room 1");
      auto t = id(ha_time).now();
      if (t.is_valid()) {
        char cbuf[8];
        snprintf(cbuf, sizeof(cbuf), "%02d:%02d", t.hour, t.minute);
        it.printf(W - 28, 26, id(font_label), Color(0x8F, 0xA0, 0xB5),
                  TextAlign::TOP_RIGHT, "%s", cbuf);
      }

      char tbuf[24], hbuf[24], vbuf[24], obuf[24], pbuf[24], wbuf[24];
      if (std::isnan(id(ha_temp).state)) snprintf(tbuf, sizeof(tbuf), "--.- °C");
      else snprintf(tbuf, sizeof(tbuf), "%.1f °C", id(ha_temp).state);
      if (std::isnan(id(ha_humidity).state)) snprintf(hbuf, sizeof(hbuf), "-- %%");
      else snprintf(hbuf, sizeof(hbuf), "%.0f %%", id(ha_humidity).state);
      if (std::isnan(id(ha_vpd).state)) snprintf(vbuf, sizeof(vbuf), "--.-- kPa");
      else snprintf(vbuf, sizeof(vbuf), "%.2f kPa", id(ha_vpd).state);
      if (std::isnan(id(ha_co2).state)) snprintf(obuf, sizeof(obuf), "--- ppm");
      else snprintf(obuf, sizeof(obuf), "%.0f ppm", id(ha_co2).state);
      if (std::isnan(id(ha_pressure).state)) snprintf(pbuf, sizeof(pbuf), "---- hPa");
      else snprintf(pbuf, sizeof(pbuf), "%.0f hPa", id(ha_pressure).state);
      if (std::isnan(id(ha_vwc).state)) snprintf(wbuf, sizeof(wbuf), "--.- %%");
      else snprintf(wbuf, sizeof(wbuf), "%.1f %%", id(ha_vwc).state);

      // VPD text goes green inside the band, amber outside
      Color white = Color(0xFF, 0xFF, 0xFF);
      Color vpd_col = white;
      if (!std::isnan(id(ha_vpd).state)) {
        float vv = id(ha_vpd).state;
        vpd_col = (vv &gt;= 0.8f &amp;&amp; vv &lt;= 1.2f) ? Color(0x74, 0xE0, 0x8A)
                                             : Color(0xFF, 0x9A, 0x5B);
      }

      // x, y, w, h, history, label, value, line_col, mid_col, value_col,
      // fix_min, fix_max, band_lo, band_hi
      // (fix_min/max both 0 = auto-scale; band_lo/hi both 0 = no band)
      draw_card(24,   62, W - 48, 190, id(temp_hist),  "Temperature", tbuf,
                Color(0xD8, 0xCF, 0x3A), Color(0x3A, 0x7D, 0x2C),
                white, 0, 0, 0, 0);
      draw_card(24,  262, W - 48, 190, id(hum_hist),   "Humidity",    hbuf,
                Color(0x35, 0xD0, 0xD0), Color(0x10, 0x5A, 0x7A),
                white, 0, 0, 0, 0);
      draw_card(24,  462, W - 48, 190, id(press_hist), "Pressure",    pbuf,
                Color(0x9F, 0xB0, 0xC4), Color(0x35, 0x41, 0x4F),
                white, 0, 0, 0, 0);
      draw_card(24,  662, W - 48, 190, id(vpd_hist),   "VPD",         vbuf,
                Color(0xC5, 0x9C, 0xFF), Color(0x4B, 0x2A, 0x8A),
                vpd_col, 0.4f, 1.8f, 0.8f, 1.2f);
      draw_card(24,  862, W - 48, 190, id(co2_hist),   "CO2",         obuf,
                Color(0xFF, 0x7A, 0x5B), Color(0x7A, 0x24, 0x24),
                white, 0, 0, 0, 0);
      draw_card(24, 1062, W - 48, 190, id(vwc_hist),   "VWC",         wbuf,
                Color(0x5B, 0x9B, 0xFF), Color(0x1B, 0x3A, 0x8A),
                white, 0, 0, 0, 0);
</pre>

<code>draw_card()</code> parameters:

| Param | Notes |
| ------------- | ------------- |
| hist | The history vector to plot |
| line_col | Colour of the 3px curve and the top of the gradient |
| mid_col | Gradient mid-tone; fades line_col to mid_col to card_bg |
| value_col | Colour of the large value text |
| fix_min / fix_max | Fixed Y range. Pass 0, 0 to auto-scale to the data instead (minimum span 0.5 units) |
| band_lo / band_hi | Shaded reference band behind the curve. Only drawn when a fixed scale is set and band_hi is above band_lo |

Only the VPD card uses either here: a 0.4 to 1.8 kPa scale with a 0.8 to 1.2 kPa band. The value colour is decided separately in <code>vpd_col</code> using the same thresholds, so if you change the band, change both. Flower rooms often target higher (around 1.2 to 1.6 kPa).

The six cards at 190px tall fill the 1280px screen exactly. To fit more, reduce the card height and the 200px stride in the y positions.

To widen the time window, raise <code>MAXN</code> or the <code>interval:</code>. 30s x 160 is about 80 minutes; 60s x 240 is 4 hours. Buffers wider than the graph area (about 620px) are linearly interpolated, so larger values still render fine.
