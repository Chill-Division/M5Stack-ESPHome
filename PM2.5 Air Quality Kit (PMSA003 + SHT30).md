### PM2.5 Air Quality Kit (PMSA003 + SHT30)

https://shop.m5stack.com/products/pm2-5-air-quality-kit-pmsa003-sht30

We take a rolling average of the last 60s particulate matter readings and update it every 20s on device just to smooth things out a little bit, though this can easily be removed if you want more up-to-date details

![image](https://github.com/Chill-Division/M5Stack-ESPHome/assets/162461/8e2ebe1a-d089-4055-9f8b-a91212942f83)

NOTE: If you try enabling bluetooth_proxy on this then the WiFi will stop working. If you're having troubles with getting it online, try removing that.

<pre>
uart:
  rx_pin: 16
  baud_rate: 9600

i2c:
  sda: 21
  scl: 22
  scan: true
  frequency: 200kHz

spi:
  clk_pin: 18
  mosi_pin: 23
  miso_pin: 19

# Particulate matter sensor and temperature sensor
sensor:
  - platform: pmsx003
    type: PMSX003
    pm_1_0:
      name: "PM1"
      id: sensor_pm1
      filters:
        - sliding_window_moving_average:
            window_size: 60
            send_every: 20
    pm_2_5:
      name: "PM2.5"
      id: sensor_pm25
      filters:
        - sliding_window_moving_average:
            window_size: 60
            send_every: 20
    pm_10_0:
      name: "PM10"
      id: sensor_pm10
      filters:
        - sliding_window_moving_average:
            window_size: 60
            send_every: 20
  - platform: sht3xd
    temperature:
      name: "Temperature"
      id: sensor_temperature
    humidity:
      name: "Humidity"
      id: sensor_humidity
    address: 0x44
    update_interval: 10s
  - platform: wifi_signal # Reports the WiFi signal strength/RSSI in dB
    name: "WiFi Signal dB"
    id: wifi_signal_db
    update_interval: 60s
    entity_category: "diagnostic"
  - platform: copy # Reports the WiFi signal strength in %
    source_id: wifi_signal_db
    name: "WiFi Signal Percent"
    id: wifi_signal_pc
    filters:
      - lambda: return min(max(2 * (x + 100.0), 0.0), 100.0);
    unit_of_measurement: "%"
    entity_category: "diagnostic"



# Button to toggle the display backlight
binary_sensor:
  - platform: gpio
    id: M5_BtnB
    pin:
      number: 38
      inverted: true
    on_click:
      then:
        - light.toggle: back_light

font:
  - file: "gfonts://Roboto@medium"
    id: font_roboto_medium22
    size: 22
    glyphs: '!"%()+,-_.:°0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz/³µ'

color:
  - id: RED
    red: 100%
    green: 0%
    blue: 0%
  - id: BLUE
    red: 0%
    green: 0%
    blue: 100%
  - id: WHITE
    red: 100%
    green: 100%
    blue: 100%
  - id: GREEN
    red: 0%
    green: 100%
    blue: 0%
  - id: YELLOW
    red: 100%
    green: 100%
    blue: 0%

# GPIO pin of the display backlight
output:
  - platform: ledc
    pin: 32
    id: gpio_32_backlight_pwm
    
light:
  - platform: monochromatic
    output: gpio_32_backlight_pwm
    name: "Display Backlight"
    id: back_light
    restore_mode: ALWAYS_ON
    
display:
  - platform: ili9xxx
    id: m5stack_display
    model: M5Stack
    cs_pin: 14
    dc_pin: 27
    reset_pin: 33
    rotation: 0
    lambda: |-
      it.rectangle(0,  0, it.get_width(), it.get_height(), BLUE);
      it.rectangle(0, 22, it.get_width(), it.get_height(), BLUE);   // header bar
      it.print(it.get_width() / 2, 11, id(font_roboto_medium22), YELLOW, TextAlign::CENTER, "Particulate matter");

      it.print(11,  31, id(font_roboto_medium22), WHITE, TextAlign::LEFT, "PM1");
      it.print(11,  53, id(font_roboto_medium22), WHITE, TextAlign::LEFT, "PM2.5");
      it.print(11,  75, id(font_roboto_medium22), WHITE, TextAlign::LEFT, "PM10");

      it.printf(it.get_width() - 11,  31, id(font_roboto_medium22), WHITE, TextAlign::RIGHT, "%.0f µg/m³", id(sensor_pm1).state);
      it.printf(it.get_width() - 11,  53, id(font_roboto_medium22), WHITE, TextAlign::RIGHT, "%.0f µg/m³", id(sensor_pm25).state);
      it.printf(it.get_width() - 11,  75, id(font_roboto_medium22), WHITE, TextAlign::RIGHT, "%.0f µg/m³", id(sensor_pm10).state);

      it.rectangle(0, 106, it.get_width(), 22, BLUE);   // header bar
      it.print(it.get_width() / 2, 117, id(font_roboto_medium22), GREEN, TextAlign::CENTER, "Environment");

      it.print(11, 135, id(font_roboto_medium22), WHITE, TextAlign::LEFT, "Temperature");
      it.print(11, 157, id(font_roboto_medium22), WHITE, TextAlign::LEFT, "Humidity");

      it.printf(it.get_width() - 77, 135, id(font_roboto_medium22), WHITE, TextAlign::RIGHT, "%.0f", id(sensor_temperature).state);
      it.printf(it.get_width() - 77, 157, id(font_roboto_medium22), WHITE, TextAlign::RIGHT, "%.0f", id(sensor_humidity).state);

      it.printf(it.get_width() - 11, 135, id(font_roboto_medium22), WHITE, TextAlign::RIGHT, "°C");
      it.printf(it.get_width() - 11, 157, id(font_roboto_medium22), WHITE, TextAlign::RIGHT, "%%");

      it.rectangle(0, 184, it.get_width(), 22, BLUE);   // header bar
      it.print(it.get_width() / 2, 195, id(font_roboto_medium22), RED, TextAlign::CENTER, "Network");

      it.print(11, 209, id(font_roboto_medium22), WHITE, TextAlign::LEFT, "WiFi Signal");
      it.printf(it.get_width() - 77, 209, id(font_roboto_medium22), WHITE, TextAlign::RIGHT, "%.0f", id(wifi_signal_pc).state);
      it.printf(it.get_width() - 11, 209, id(font_roboto_medium22), WHITE, TextAlign::RIGHT, "%%");
</pre>
