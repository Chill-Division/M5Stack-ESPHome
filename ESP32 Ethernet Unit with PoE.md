### ESP32 Ethernet Unit with PoE

https://shop.m5stack.com/products/esp32-ethernet-unit-with-poe

This is a generic template needed to make it play nice, without any sensors plugged in.

It'll then use Pins G16 / G17 for the Grove connector

<pre>
esphome:
  name: m5stack-poesp32-01

esp32:
  board: m5stack-core-esp32
  framework:
    type: arduino

# Enable logging
logger:

# Keep this, coz you can't flash shit remotely without it
ota:

# Network interface for PoESP32
ethernet:
  type: IP101
  mdc_pin: GPIO23
  mdio_pin: GPIO18
  clk_mode: GPIO0_IN
  phy_addr: 1
  power_pin: GPIO5

# Enable Home Assistant API
api:
  encryption:
    # Just leave the one it generates for you
    key: "***"
</pre>
