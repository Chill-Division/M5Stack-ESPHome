### ATOM PWM Kit (FDD8447L)

https://shop.m5stack.com/products/atom-pwm-kit-fdd8447l

This is a demonstration of how to use the ledc function with the "OUT" and "GND" pins on certain LED growlights
<pre>
output:
  - platform: ledc
    pin: GPIO22
    frequency: 1000 hz
    id: pwm_outputg22
    inverted: true

light:
  - platform: monochromatic
    output: pwm_outputg22
    gamma_correct: 1.0
    name: "Light-PWMdemo1"
    id: "Light_PWMdemo1"
</pre>
