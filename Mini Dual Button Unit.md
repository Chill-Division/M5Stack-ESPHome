### Mini Dual Button Unit

https://shop.m5stack.com/products/mini-dual-button-unit

<pre>
binary_sensor:
  - platform: gpio
    pin:
      number: 32
      inverted: true
    name: "BlueButton"
    id: "BlueButton"
    on_press:
    - logger.log: "Blue button pressed"
  - platform: gpio
    pin: 
      number: 26
      inverted: true
    name: "RedButton"
    id: "RedButton"
    on_press:
    - logger.log: "Red button pressed"
</pre>
