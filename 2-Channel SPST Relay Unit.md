### 2-Channel SPST Relay Unit

https://shop.m5stack.com/products/2-channel-spst-relay-unit

This unit has obnoxiously bright blue LEDs that seemingly cannot be disabled when the relay is on.

<pre>
# Relays, toggle it on / off
switch:
  - platform: gpio
    pin: 17 # On PoESP32 this is the White pin, controls the relay furthest from the Grove connector
    id: Relay1
    name: "Relay1"
    inverted: false
  - platform: gpio
    pin: 16 # On PoESP32 this is the Yellow pin, controls the relay closest to the Grove connector
    id: Relay2
    name: "Relay2"
    inverted: false
</pre>
