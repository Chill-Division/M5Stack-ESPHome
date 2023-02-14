### Mechanical Key Button Unit

https://shop.m5stack.com/products/mechanical-key-button-unit

<pre>
# The big red button itself
binary_sensor:
  - platform: gpio
    pin:
      number: 17
      inverted: true
    name: "RedKeyboardButton1"
    id: "RedKeyboardButton1"
    on_press:
    - logger.log: "Big red button pressed"

# LED under the key
light:
  - platform: fastled_clockless
    chipset: SK6812
    pin: GPIO16
    num_leds: 1
    rgb_order: GRB
    name: "RedKeyboardButton1-Light"
    </pre>
