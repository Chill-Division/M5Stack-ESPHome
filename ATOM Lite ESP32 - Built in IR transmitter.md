### ATOM Lite ESP32 - Built in IR transmitter

https://shop.m5stack.com/products/atom-lite-esp32-development-kit?variant=32259605200986

This is an example for how to use the in-built IR transmitter on the Atom Lite

You can record remote signals with an additional device such as the Mini Infrared Emitter & Receiver Unit - https://shop.m5stack.com/products/ir-unit

However, for this example, no additional hardware is required. Range of up to ~3.5 meters

<pre>
remote_transmitter:
  pin: 12
  carrier_duty_percent: 50%

switch:
  - platform: template
    name: "LG TV On Off"
    turn_on_action:
      remote_transmitter.transmit_nec:
        address: 0xBF40
        command: 0xF40B
</pre>
