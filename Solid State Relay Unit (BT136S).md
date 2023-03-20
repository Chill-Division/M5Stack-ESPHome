### Solid State Relay Unit (BT136S)

https://shop.m5stack.com/products/solid-state-relay-unit-bt136s

<pre>
# Relay, toggle it on / off
switch:
  - platform: gpio
    pin: 26
    id: SSRelay1
    name: "SSRelay1"
    #inverted: true
</pre>

#### Wiring example

When splicing a cable, such as an extension lead, the Earth should always remain connected (Green, or Green/Yellow).

Active will usually be Brown (or Red), and will be connected to the two outer pins on the SSR.

Neutral will usually be Blue (or Black), and will be connected to the two inner pins on the SSR.

This is not specific to Australia / New Zealand, however your country may have different standards worth being mindful of.

There is also a light on the SSR unit to indicate when it is _powered on_ however it can be dull and difficult to see from underneath the SSR housing:
![SSR-Light2](https://user-images.githubusercontent.com/162461/226445385-82207163-01fb-47d3-8475-a492ed2ecf77.png)
