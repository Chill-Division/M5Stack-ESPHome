### Mini Infrared Emitter & Receiver Unit

https://shop.m5stack.com/products/ir-unit

<pre>
remote_receiver:
  pin:
    number: GPIO32
    inverted: true
  dump: all

remote_transmitter:
  pin: GPIO26
  carrier_duty_percent: 50%

climate:
  - platform: heatpumpir       # adjust to match your AC unit!
    protocol: panasonic_dke
    name: "Lounge AC"
    horizontal_default: mright
    vertical_default: up
    max_temperature: 25
    min_temperature: 16
</pre>
