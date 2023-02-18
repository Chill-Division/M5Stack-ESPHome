### NCIR Non-Contact Infrared Thermometer Sensor Unit (MLX90614)

https://shop.m5stack.com/products/ncir-sensor-unit

Under your "esphome" section of your yaml, you'll need to add some additional includes / libraries:
<pre>
esphome:
  name: "espatom-ir2"
  includes:
    - "mlx90614_sensor.h"
  libraries:
    - SPI
    - Wire
    - adafruit/Adafruit BusIO @ 1.9.6
    - "Adafruit MLX90614 Library"
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

sensor:
  - platform: custom
    lambda: |-
      auto mlx90614_sensor = new MLX90614Sensor();
      App.register_component(mlx90614_sensor);
      return {mlx90614_sensor->ambient_temperature_sensor, mlx90614_sensor->object_temperature_sensor};
    sensors:
      - name: "Ambient Temperature"
        unit_of_measurement: °C
        accuracy_decimals: 1
      - name: "Object Temperature"
        unit_of_measurement: °C
        accuracy_decimals: 1
</pre>
