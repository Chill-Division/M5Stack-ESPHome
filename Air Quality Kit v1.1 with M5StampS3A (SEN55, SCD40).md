### Air Quality Kit v1.1 with M5StampS3A (SEN55, SCD40)

https://shop.m5stack.com/products/air-quality-kit-v1-1-with-m5stamps3a-sen55-scd40

Also known as the AirQ V1.1 (K131-V11). This is a complete config: the SEN55 (PM1.0, PM2.5, PM4.0, PM10, VOC, NOx, temperature and humidity), the SCD40 (CO2), the e-ink screen, both buttons, the buzzer, the RTC and the battery level.

<b>NOTE:</b> Needs ESPHome 2026.9 or newer. It expects <code>wifi_ssid</code>, <code>wifi_password</code> and <code>api_encryption_key</code> in your <code>secrets.yaml</code>. If you created the device in the ESPHome dashboard, you can paste the encryption key it generated in place of the <code>!secret</code>.

For the first flash over USB: turn the AirQ off, hold Button A (G0) while you plug in the USB cable, then let go once it's powered.

- Button A and Button B flip between three screens: an overview, all readings, and status (Wi-Fi, IP, battery, uptime)
- The SEN55 is powered through GPIO10, which is active low. The config turns it on at boot
- GPIO46 holds the power on when running from the battery. Setting it low turns the AirQ off when USB isn't plugged in
- With Wi-Fi on all the time the 600mAh battery only lasts a few hours, so keep it on USB power
- The temperature can read a little high because the sensors share the case with the ESP32. Check it against another thermometer and add an <code>offset</code> filter under <code>temperature:</code> if needed
- The e-ink does a partial refresh every minute and a full refresh every 10 updates to clear ghosting. Change <code>full_update_every</code> to suit
- Grove port: yellow = GPIO13, white = GPIO15. There's a commented-out second I2C bus for it

<pre>
substitutions:
  name: m5stack-airq
  friendly_name: M5Stack AirQ

esphome:
  name: ${name}
  friendly_name: ${friendly_name}
  min_version: 2026.9.0
  on_boot:
    then:
      # Load the time from the battery-backed RTC so the clock is right before Wi-Fi connects
      - bm8563.read_time:

esp32:
  variant: esp32s3
  flash_size: 8MB
  framework:
    type: esp-idf

logger:

api:
  id: airq_api
  encryption:
    key: !secret api_encryption_key

ota:
  - platform: esphome
    # Uses the api key above to secure OTA updates
    encryption:

wifi:
  id: airq_wifi
  ssid: !secret wifi_ssid
  password: !secret wifi_password
  ap: {}

captive_portal:

i2c:
  # Internal bus: SEN55 (0x69), SCD40 (0x62) and the BM8563 RTC (0x51)
  - id: bus_internal
    sda: GPIO11
    scl: GPIO12
  # Grove port on the side: yellow = GPIO13, white = GPIO15
  # Uncomment to plug in another unit, and give that sensor "i2c_id: bus_grove"
  # - id: bus_grove
  #   sda: GPIO13
  #   scl: GPIO15

spi:
  clk_pin: GPIO5
  mosi_pin: GPIO6

switch:
  # Keeps the AirQ powered when it's running on its battery - pulling GPIO46 low cuts the power
  - platform: gpio
    id: power_hold
    pin:
      number: GPIO46
      ignore_strapping_warning: true
    restore_mode: ALWAYS_ON
    internal: true
  # Power to the SEN55 is active low, so "on" drives GPIO10 low
  - platform: gpio
    id: sen55_power
    pin:
      number: GPIO10
      inverted: true
    restore_mode: ALWAYS_ON
    internal: true

# Passive buzzer
output:
  - platform: ledc
    id: buzzer_output
    pin: GPIO9

rtttl:
  id: buzzer
  output: buzzer_output

button:
  - platform: template
    name: "Beep"
    icon: "mdi:bullhorn"
    on_press:
      - rtttl.play: "beep:d=16,o=7,b=120:e,p,e"
  - platform: template
    name: "SEN55 Fan Clean"
    icon: "mdi:fan"
    entity_category: config
    on_press:
      - sen5x.start_fan_autoclean: sen55
  - platform: restart
    name: "Restart"

# Button A = previous page, Button B = next page
binary_sensor:
  - platform: gpio
    id: button_a
    name: "Button A"
    pin:
      number: GPIO0
      inverted: true
      ignore_strapping_warning: true
      mode:
        input: true
        pullup: true
    filters:
      - delayed_on: 20ms
    on_press:
      - display.page.show_previous: epaper
      - component.update: epaper
  - platform: gpio
    id: button_b
    name: "Button B"
    pin:
      number: GPIO8
      inverted: true
      mode:
        input: true
        pullup: true
    filters:
      - delayed_on: 20ms
    on_press:
      - display.page.show_next: epaper
      - component.update: epaper

time:
  - platform: bm8563
    id: rtc_time
    i2c_id: bus_internal
    update_interval: never
  - platform: homeassistant
    id: ha_time
    on_time_sync:
      then:
        # Save the time from Home Assistant into the RTC
        - bm8563.write_time:

sensor:
  - platform: sen5x
    id: sen55
    i2c_id: bus_internal
    update_interval: 30s
    pm_1_0:
      name: "PM1.0"
      id: pm_1_0
      accuracy_decimals: 1
    pm_2_5:
      name: "PM2.5"
      id: pm_2_5
      accuracy_decimals: 1
    pm_4_0:
      name: "PM4.0"
      id: pm_4_0
      accuracy_decimals: 1
    pm_10_0:
      name: "PM10"
      id: pm_10_0
      accuracy_decimals: 1
    temperature:
      name: "Temperature"
      id: temperature
      accuracy_decimals: 1
    humidity:
      name: "Humidity"
      id: humidity
      accuracy_decimals: 1
    voc_index:
      name: "VOC Index"
      id: voc_index
    nox_index:
      name: "NOx Index"
      id: nox_index

  - platform: scd4x
    id: scd40
    i2c_id: bus_internal
    update_interval: 30s
    co2:
      name: "CO2"
      id: co2
    temperature:
      name: "SCD40 Temperature"
      id: scd40_temperature
      accuracy_decimals: 1
      entity_category: diagnostic
    humidity:
      name: "SCD40 Humidity"
      id: scd40_humidity
      accuracy_decimals: 1
      entity_category: diagnostic

  - platform: template
    name: "VPD"
    icon: "mdi:gauge"
    id: vpd
    lambda: |-
          return (((100 - id(humidity).state) / 100.0) * (0.6108 * exp((17.27 * id(temperature).state) / (id(temperature).state + 237.3))));
    update_interval: 30s
    unit_of_measurement: kPa
    accuracy_decimals: 2
    state_class: measurement
    filters:
      - filter_out: nan

  # Built-in 600mAh battery, read through a 1:2 divider
  - platform: adc
    id: battery_voltage
    name: "Battery Voltage"
    pin: GPIO14
    attenuation: 12db
    update_interval: 60s
    accuracy_decimals: 2
    entity_category: diagnostic
    filters:
      - multiply: 2.0
  - platform: copy
    source_id: battery_voltage
    id: battery_level
    name: "Battery Level"
    unit_of_measurement: "%"
    device_class: battery
    accuracy_decimals: 0
    entity_category: diagnostic
    filters:
      # Rough estimate: 3.3V = empty, 4.2V = full
      - lambda: return min(max((x - 3.3) / (4.2 - 3.3) * 100.0, 0.0), 100.0);

  - platform: wifi_signal
    name: "WiFi Signal"
    id: wifi_signal_db
    update_interval: 60s
    entity_category: diagnostic
  - platform: uptime
    name: "Uptime"
    id: uptime_sec

text_sensor:
  - platform: wifi_info
    ip_address:
      name: "IP Address"
      id: airq_ip
    ssid:
      name: "Connected SSID"
      id: airq_ssid
  - platform: version
    name: "ESPHome Version"

font:
  - file: "gfonts://Roboto@700"
    id: font_xl
    size: 56
    glyphs: "-0123456789"
  - file: "gfonts://Roboto@500"
    id: font_l
    size: 26
    glyphs: " %-.0123456789°C"
  - file: "gfonts://Roboto@500"
    id: font_m
    size: 17
    glyphs: ' !%()+,-./0123456789:?ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz°µ³'
  - file: "gfonts://Roboto@400"
    id: font_s
    size: 13
    glyphs: ' !#%&amp;()+,-./0123456789:?@ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz°µ³'

# 1.54" 200x200 e-ink (GDEY0154D67)
display:
  - platform: waveshare_epaper
    id: epaper
    model: 1.54inv2
    cs_pin: GPIO4
    dc_pin:
      number: GPIO3
      ignore_strapping_warning: true
    busy_pin: GPIO1
    reset_pin: GPIO2
    update_interval: 60s
    # Partial refreshes in between, a full (flashing) refresh every 10 updates to clear ghosting
    full_update_every: 10
    pages:
      - id: overview_page
        lambda: |-
          // Shows "--" until a sensor has its first reading
          char buf[16];
          auto val = [&amp;buf](float v, const char *fmt) -&gt; const char * {
            if (std::isnan(v)) return "--";
            snprintf(buf, sizeof(buf), fmt, v);
            return buf;
          };

          // Top line: clock, Wi-Fi state, battery
          auto now = id(rtc_time).now();
          if (now.is_valid()) {
            it.strftime(4, 2, id(font_s), "%H:%M", now);
          } else {
            it.print(4, 2, id(font_s), "--:--");
          }
          if (!id(airq_wifi).is_connected()) {
            it.print(100, 2, id(font_s), TextAlign::TOP_CENTER, "No WiFi");
          }
          it.printf(196, 2, id(font_s), TextAlign::TOP_RIGHT, "Bat %s", val(id(battery_level).state, "%.0f%%"));
          it.line(0, 19, 199, 19);

          // CO2
          it.print(4, 23, id(font_s), "CO2");
          it.print(196, 23, id(font_s), TextAlign::TOP_RIGHT, "ppm");
          it.print(100, 64, id(font_xl), TextAlign::CENTER, val(id(co2).state, "%.0f"));
          it.line(0, 97, 199, 97);

          // Temperature and humidity
          it.print(50, 101, id(font_l), TextAlign::TOP_CENTER, val(id(temperature).state, "%.1f°C"));
          it.print(50, 131, id(font_s), TextAlign::TOP_CENTER, "Temperature");
          it.print(150, 101, id(font_l), TextAlign::TOP_CENTER, val(id(humidity).state, "%.0f%%"));
          it.print(150, 131, id(font_s), TextAlign::TOP_CENTER, "Humidity");
          it.line(100, 97, 100, 199);
          it.line(0, 149, 199, 149);

          // Particulates and gas indexes
          it.print(4, 170, id(font_s), TextAlign::BASELINE_LEFT, "PM2.5");
          it.print(95, 170, id(font_m), TextAlign::BASELINE_RIGHT, val(id(pm_2_5).state, "%.0f"));
          it.print(4, 193, id(font_s), TextAlign::BASELINE_LEFT, "PM10");
          it.print(95, 193, id(font_m), TextAlign::BASELINE_RIGHT, val(id(pm_10_0).state, "%.0f"));
          it.print(105, 170, id(font_s), TextAlign::BASELINE_LEFT, "VOC");
          it.print(196, 170, id(font_m), TextAlign::BASELINE_RIGHT, val(id(voc_index).state, "%.0f"));
          it.print(105, 193, id(font_s), TextAlign::BASELINE_LEFT, "NOx");
          it.print(196, 193, id(font_m), TextAlign::BASELINE_RIGHT, val(id(nox_index).state, "%.0f"));

      - id: details_page
        lambda: |-
          it.filled_rectangle(0, 0, 200, 20);
          it.print(100, 10, id(font_m), COLOR_OFF, TextAlign::CENTER, "All readings");

          // One reading per row: label on the left, value on the right
          char buf[24];
          int row = 0;
          auto print_row = [&amp;](const char *label, float v, const char *fmt) {
            int y = 24 + row++ * 17;
            it.print(4, y, id(font_s), label);
            if (std::isnan(v)) {
              it.print(196, y, id(font_s), TextAlign::TOP_RIGHT, "--");
            } else {
              snprintf(buf, sizeof(buf), fmt, v);
              it.print(196, y, id(font_s), TextAlign::TOP_RIGHT, buf);
            }
          };
          print_row("PM1.0", id(pm_1_0).state, "%.1f µg/m³");
          print_row("PM2.5", id(pm_2_5).state, "%.1f µg/m³");
          print_row("PM4.0", id(pm_4_0).state, "%.1f µg/m³");
          print_row("PM10", id(pm_10_0).state, "%.1f µg/m³");
          print_row("VOC index", id(voc_index).state, "%.0f");
          print_row("NOx index", id(nox_index).state, "%.0f");
          print_row("CO2", id(co2).state, "%.0f ppm");
          print_row("Temperature", id(temperature).state, "%.1f °C");
          print_row("Humidity", id(humidity).state, "%.1f %%");
          print_row("VPD", id(vpd).state, "%.2f kPa");

      - id: status_page
        lambda: |-
          it.filled_rectangle(0, 0, 200, 20);
          it.print(100, 10, id(font_m), COLOR_OFF, TextAlign::CENTER, "Status");

          char buf[32];
          int row = 0;
          auto print_row = [&amp;](const char *label, const char *value) {
            int y = 24 + row++ * 17;
            it.print(4, y, id(font_s), label);
            it.print(196, y, id(font_s), TextAlign::TOP_RIGHT, value);
          };

          bool wifi_ok = id(airq_wifi).is_connected();
          // Shorten long network names so they don't run into the label
          std::string ssid = id(airq_ssid).state;
          if (ssid.length() &gt; 20) ssid = ssid.substr(0, 18) + "..";
          print_row("WiFi", wifi_ok ? ssid.c_str() : "Not connected");
          print_row("IP", wifi_ok ? id(airq_ip).state.c_str() : "--");
          if (wifi_ok &amp;&amp; !std::isnan(id(wifi_signal_db).state)) {
            snprintf(buf, sizeof(buf), "%.0f dBm", id(wifi_signal_db).state);
            print_row("Signal", buf);
          } else {
            print_row("Signal", "--");
          }
          print_row("Home Assistant", id(airq_api).is_connected() ? "Connected" : "Not connected");
          if (!std::isnan(id(battery_voltage).state)) {
            snprintf(buf, sizeof(buf), "%.2f V (%.0f%%)", id(battery_voltage).state, id(battery_level).state);
            print_row("Battery", buf);
          } else {
            print_row("Battery", "--");
          }
          int up = std::isnan(id(uptime_sec).state) ? 0 : (int) id(uptime_sec).state;
          snprintf(buf, sizeof(buf), "%dd %02dh %02dm", up / 86400, (up % 86400) / 3600, (up % 3600) / 60);
          print_row("Uptime", buf);
          print_row("ESPHome", ESPHOME_VERSION);
</pre>
