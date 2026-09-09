### OLED Unit 1.3" 128 × 64 Display

https://shop.m5stack.com/products/oled-unit-1-3-128-64-display?variant=39813420155052

Thanks to @DavidGeorge528 for the initial issue raised with working config.

This demonstration pulls 4x values from Home Assistant to display on-screen.

<img width="1470" height="828" alt="image" src="https://github.com/user-attachments/assets/7d3a4a4c-35e1-4d93-b227-4411e67b980f" />

<pre>
api:   # required for the homeassistant sensor platform (keep your existing api: block if you already have one)

i2c:
  sda: 53
  scl: 54

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
    size: 14
    glyphs: '!"%()+,-_.:°0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz/³µ'

display:
  - platform: ssd1306_i2c
    model: "SH1107 128x64"
    address: 0x3C
    update_interval: 10s
    rotation: 90
    offset_y: 96
    lambda: |-
      if (isnan(id(ha_co2).state)) {
        it.printf(0, 0, id(my_font), "CO2: ---");
      } else {
        it.printf(0, 0, id(my_font), "CO2: %.0f ppm", id(ha_co2).state);
      }

      if (isnan(id(ha_temperature).state)) {
        it.printf(0, 16, id(my_font), "Temp: ---");
      } else {
        it.printf(0, 16, id(my_font), "Temp: %.1f°C", id(ha_temperature).state);
      }

      if (isnan(id(ha_humidity).state)) {
        it.printf(0, 32, id(my_font), "Hum: ---");
      } else {
        it.printf(0, 32, id(my_font), "Hum: %.1f %%", id(ha_humidity).state);
      }

      if (isnan(id(ha_vpd).state)) {
        it.printf(0, 48, id(my_font), "VPD: ---");
      } else {
        it.printf(0, 48, id(my_font), "VPD: %.2f kPa", id(ha_vpd).state);
      }
</pre>


