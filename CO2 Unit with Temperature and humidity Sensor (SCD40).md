### CO2 Unit with Temperature and humidity Sensor (SCD40)

https://shop.m5stack.com/products/co2-unit-with-temperature-and-humidity-sensor-scd40

Also works for SCD41, even the i2c address is the same - https://shop.m5stack.com/products/co2l-unit-with-temperature-and-humidity-sensor-scd41

<pre>
i2c:
  sda: 16
  scl: 17

sensor:
  - platform: scd4x
    id: scd40
    co2:
      name: "Server rack CO2"
      accuracy_decimals: 1
    temperature:
      name: "Server rack Temperature"
      accuracy_decimals: 2
    humidity:
      name: "Server rack Humidity"
      accuracy_decimals: 1
    address: 0x62
    update_interval: 10s
</pre>
