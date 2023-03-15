### ENV III Unit with Temperature Humidity Air Pressure Sensor (SHT30+QMP6988)

https://shop.m5stack.com/products/env-iii-unit-with-temperature-humidity-air-pressure-sensor-sht30-qmp6988

These units have both an SHT30 and a QMP6988, so you get two temperature sensors. Do with them what you will.

<pre>
i2c:
  sda: 16
  scl: 17
  scan: true
  id: bus_1

#ENV.III
sensor:
  - platform: sht3xd
    temperature:
      name: "Temperature - SHT30"
    humidity:
      name: "Humidity"
    address: 0x44
    update_interval: 10s
  - platform: qmp6988
    temperature:
      name: "Temperature - QMP6988"
      oversampling: 16x
    pressure:
      name: "Pressure"
      oversampling: 16x
    address: 0x70
    update_interval: 10s
    iir_filter: 2x
</pre>
