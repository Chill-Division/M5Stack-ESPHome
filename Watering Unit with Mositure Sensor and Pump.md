### Watering Unit with Mositure Sensor and Pump

https://shop.m5stack.com/products/watering-unit-with-mositure-sensor-and-pump

<pre>
# Pump, toggle it on / off
switch:
  - platform: gpio
    pin: 16
    id: waterpump
    name: "WaterPump"

# Rainfall sensor - digital wet/dry output.
# I only wanted a binary for "has something flooded"
# but you should also be able to use ADC to give you values
binary_sensor:
  - platform: gpio
    pin: 
      number: 17
      inverted: true
    name: "Water sensor"
    device_class: moisture
</pre>
