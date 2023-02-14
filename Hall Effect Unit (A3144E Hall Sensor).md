### Hall Effect Unit (A3144E Hall Sensor)

https://shop.m5stack.com/products/hall-effect-unit-a3144e

<pre>
# A3144E, magnet over the "I" or "GND"
# Has approx 3mm of tolerance with the included magnet
# Polarity also matters
binary_sensor:
  - platform: gpio
    pin:
      number: 32
      mode: INPUT_PULLUP
      inverted: true
    name: "Security Door"
    id: "security_door"
    on_press:
    - logger.log: "Door closed"
    icon: "mdi:magnet-on"
    device_class: door
</pre>
