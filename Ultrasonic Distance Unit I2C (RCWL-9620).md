### Ultrasonic Distance Unit I2C (RCWL-9620)

https://shop.m5stack.com/products/ultrasonic-distance-unit-i2c-rcwl-9620?variant=42084188225793

<b>NOTE:</b> This only works with the I2C unit. The GPIO unit doesn't play nice with ESPHome (I had to install Tasmota instead), so be sure to get the I2C variant which will also then work with the Pa.hub

<pre>
external_components:
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
