### SK6812 Digital RGB LED Strip

https://shop.m5stack.com/products/sk6812-rgb-led-flex-strip?variant=16804744921178

NOTE: These are rated at 18W/m. You CAN fully power them off an Atom Lite or the PoESP32 however it's going to depend on what your power supply can handle.

If in doubt, or for troubleshooting purposes, set the num_leds: 2

<pre>
light:
  - platform: fastled_clockless
    chipset: SK6812
    pin: GPIO16
    num_leds: 15
    rgb_order: GRB
    name: "LED Strip1"
    id: "ledstrip1"
</pre>

Table for your varying lengths:
<pre>
# Length: 10cm
num_leds: 15

# Length: 20cm
num_leds: 29

# Length: 50cm
num_leds: 72

# Length: 100cm
num_leds: 144

# Length: 200cm
num_leds: 288
</pre>
