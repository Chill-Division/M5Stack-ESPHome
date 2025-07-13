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
    automatic_self_calibration: False
    co2:
      name: "CO2"
      id: co2
      accuracy_decimals: 1
    temperature:
      name: "Temperature"
      id: temperature
      accuracy_decimals: 2
    humidity:
      name: "Humidity"
      id: humidity
      accuracy_decimals: 1
    address: 0x62
    update_interval: 10s
  - platform: template
    name: "VPD"
    icon: "mdi:gauge"
    id: vpd
    lambda: |-
          return (((100 - id(humidity).state) / 100.0) * (0.6108 * exp((17.27 * id(temperature).state) / (id(temperature).state + 237.3))));
    update_interval: 10s
    unit_of_measurement: kPa
    accuracy_decimals: 2
    filters:
      - filter_out: nan
  - platform: uptime
    name: Uptime
</pre>
