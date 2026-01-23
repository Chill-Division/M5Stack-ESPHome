### Ultrasonic Distance Unit GPIO (RCWL-9620)

https://shop.m5stack.com/products/ultrasonic-distance-unit-i-o-rcwl-9620

This is for the GPIO version, not I2C. Many thanks to @Cenedd for opening issue #10 and providing the solution. Timeout is no longer needed as of 2026.1:

<pre>
sensor:
  - platform: ultrasonic
    trigger_pin: GPIO2
    echo_pin: GPIO1
    name: "Distance"
    id: fill_level
    update_interval: 1s
    unit_of_measurement: "m"
</pre>
