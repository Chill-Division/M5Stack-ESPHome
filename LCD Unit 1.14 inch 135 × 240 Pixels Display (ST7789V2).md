### LCD Unit 1.14" 135 × 240 Pixels Display (ST7789V2)

https://shop.m5stack.com/products/lcd-unit-1-14-135-240-pixels-display

<b>NOTE:</b> Although this is an ST7789V2 panel, it is <b>not</b> wired as a plain SPI display. The unit has its own ESP32-PICO inside that takes drawing commands over I2C (address 0x3E), so ESPHome's built-in <code>st7789v</code> platform won't drive it. The <code>m5unit_lcd</code> external component in this repo speaks the unit's I2C protocol instead, and gives you the normal ESPHome display features (fonts, pages, images, colour) on a 135 x 240 canvas.

Tested with ESPHome 2026.8.0 on the Unit PoE with ESP32-P4 (Grove pins 53 / 54). Set the I2C bus to 400kHz; ESPHome's default of 50kHz makes a full-screen redraw take over ten seconds.

Same demonstration as the OLED Unit: 4x values pulled from Home Assistant. <code>rotation: 90</code> gives a 240 x 135 landscape canvas (use 270 if it's upside down for how you've mounted it), and <code>brightness:</code> controls the backlight. From lambdas you can also call <code>id(unit_lcd).set_brightness(0-255)</code> and <code>id(unit_lcd).set_sleep(true/false)</code>.

<pre>
api:   # required for the homeassistant sensor platform (keep your existing api: block if you already have one)

external_components:
  - source:
      type: git
      url: https://github.com/chill-Division/M5Stack-ESPHome/
      ref: main
    components: m5unit_lcd

i2c:
  sda: 53
  scl: 54
  frequency: 400kHz

sensor:
  - platform: homeassistant
    id: ha_co2
    entity_id: sensor.gr2_co2_1_co2
  - platform: homeassistant
    id: ha_humidity
    entity_id: sensor.gr2_co2_1_humidity
  - platform: homeassistant
    id: ha_temperature
    entity_id: sensor.gr2_co2_1_temperature
  - platform: homeassistant
    id: ha_vpd
    entity_id: sensor.gr2_co2_1_vpd

font:
  - file: "gfonts://Roboto@medium"
    id: my_font
    size: 24
    glyphs: '!"%()+,-_.:°0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz/³µ'

display:
  - platform: m5unit_lcd
    id: unit_lcd
    address: 0x3E
    update_interval: 10s
    rotation: 90
    brightness: 100%
    lambda: |-
      if (isnan(id(ha_co2).state)) {
        it.printf(0, 0, id(my_font), "CO2: ---");
      } else {
        it.printf(0, 0, id(my_font), "CO2: %.0f ppm", id(ha_co2).state);
      }

      if (isnan(id(ha_temperature).state)) {
        it.printf(0, 33, id(my_font), "Temp: ---");
      } else {
        it.printf(0, 33, id(my_font), "Temp: %.1f°C", id(ha_temperature).state);
      }

      if (isnan(id(ha_humidity).state)) {
        it.printf(0, 66, id(my_font), "Hum: ---");
      } else {
        it.printf(0, 66, id(my_font), "Hum: %.1f %%", id(ha_humidity).state);
      }

      if (isnan(id(ha_vpd).state)) {
        it.printf(0, 99, id(my_font), "VPD: ---");
      } else {
        it.printf(0, 99, id(my_font), "VPD: %.2f kPa", id(ha_vpd).state);
      }
</pre>

Component options:

| Option | Default | Notes |
| ------------- | ------------- | ------------- |
| address | 0x3E | Unit LCD default I2C address |
| rotation | 0 | 0 / 90 / 180 / 270. 90 and 270 give a 240 x 135 landscape canvas |
| brightness | 100% | Backlight level |
| invert_colors | false | Invert panel colours |
| update_interval | 1s | How often the lambda is re-run. Only changed rows are sent to the unit |

How it works: the lambda draws into a local RGB565 framebuffer, which is diffed against the last frame sent. Only the changed row spans are pushed over I2C (CASET / RASET / WRITE_RAW_16), with the unit's command-buffer count checked between transfers the same way M5GFX's Panel_M5UnitLCD driver does. A full 65 KB redraw takes about 1.7 s at 400kHz; typical text updates are tens of milliseconds.

Protocol reference: https://docs.m5stack.com/en/unit/lcd
