### PIR Motion Sensor (AS312)

https://shop.m5stack.com/products/pir-module

<pre>
binary_sensor:
  - platform: gpio
    pin: G17
    name: "PIR Sensor1"
    id: pir_sensor1
    device_class: motion
</pre>
