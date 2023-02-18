### Ultrasonic Distance Unit I2C (RCWL-9620)

https://shop.m5stack.com/products/ultrasonic-distance-unit-i2c-rcwl-9620?variant=42084188225793

<pre>
external_components:
  # use rtttl and dfplayer from ESPHome's dev branch in GitHub
  - source:
      type: git
      url: https://github.com/chill-Division/M5Stack-ESPHome/
      ref: main
    components: sonic_i2c

i2c:
  sda: 16
  scl: 17
  scan: true
  id: bus_1

sensor:
 - platform: sonic_i2c
   i2c_id: bus_1
   address: 0x57
   name: "Ultrasonic Sensor 1"
   id: ultrasonic1
   unit_of_measurement: mm
   update_interval: 5s
</pre>
