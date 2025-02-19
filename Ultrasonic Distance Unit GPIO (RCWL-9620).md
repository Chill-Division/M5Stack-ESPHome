### Ultrasonic Distance Unit GPIO (RCWL-9620)

https://shop.m5stack.com/products/ultrasonic-distance-unit-i-o-rcwl-9620

This is for the GPIO version, not I2C. Many thanks to @Cenedd for opening issue #10 and providing the solution:

<pre>
sensor:
  - platform: ultrasonic
    trigger_pin: GPIO2
    echo_pin: GPIO1
    name: "Distance"
    id: fill_level
    update_interval: 1s
    timeout: 8m     #8m minimum
    unit_of_measurement: "m"
</pre>
