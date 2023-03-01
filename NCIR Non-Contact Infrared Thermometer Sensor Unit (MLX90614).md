### NCIR Non-Contact Infrared Thermometer Sensor Unit (MLX90614)

https://shop.m5stack.com/products/ncir-sensor-unit

Under your "esphome" section of your yaml, you'll need to add some additional includes / libraries:
<pre>
esphome:
  name: "espatom-ir2"
  libraries:
    - SPI
    - Wire
    - adafruit/Adafruit BusIO @ 1.9.6
</pre>

Then you can add the main configuration for the unit:
<pre>
external_components:
  - source:
      type: git
      url: https://github.com/chill-Division/M5Stack-ESPHome/
      ref: main
    components: mlx90614

i2c:
  id: bus_a
  scan: True
  sda: 26
  scl: 32

mlx90614:
  ambient_temperature: # Ambient temperature
    name: "Ambient temperature"
    unit_of_measurement: °C
    accuracy_decimals: 1
  object_temperature: # Object temperature
    name: "Object temperature"
    unit_of_measurement: °C
    accuracy_decimals: 1
</pre>
