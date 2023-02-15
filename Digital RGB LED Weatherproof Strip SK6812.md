### Digital RGB LED Weatherproof Strip SK6812

https://shop.m5stack.com/products/digital-rgb-led-weatherproof-strip-sk6812?variant=37765941821612

Should also work for the SK6812 Digital RGB LED Strip - https://shop.m5stack.com/products/sk6812-rgb-led-flex-strip?variant=16804744921178

NOTE: These are rated at 18W/m. You CAN fully power them off an Atom Lite or the PoESP32 however it's going to depend on what your power supply can handle.

If in doubt, or for troubleshooting purposes, set the num_leds: 2

<pre>
light:
  - platform: fastled_clockless
    chipset: SK6812
    pin: GPIO16
    num_leds: 30
    rgb_order: GRB
    name: "LED Strip1"
    id: "ledstrip1"
</pre>

I found a 1.5A USB wall wort could power an Atom Lite (pin: GPIO26) and do 60 LEDs, but it doesn't sem to max out the brightness.

Stopping at ~30 LEDs off a single Atom seems to be the safest
