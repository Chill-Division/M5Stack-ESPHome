### Unit PoE with ESP32-P4

https://shop.m5stack.com/products/unit-poe-with-esp32-p4?variant=48166509805825

This is a generic template needed to make it play nice, without any sensors plugged in, and come up with an empty webserver for you to test the ethernet connectivity.

Be sure to remove the "wifi:" and "captive_portal:" from the defaults too.

It'll then use GPIO pins 53 / 54 for the Grove connector on the side above the USB-C port.

<pre>
esphome:
  name: unit-poe-p4

esp32:
  # Using the generic P4 board target
  board: esp32-p4 
  framework:
    type: esp-idf
  # Unit PoE P4 has 32MB Octal PSRAM. 
  # This config ensures the app doesn't hang trying to init it.
    sdkconfig_options:
      CONFIG_SPIRAM: y
      CONFIG_SPIRAM_MODE_OCT: y
      CONFIG_SPIRAM_SPEED_80M: y
  # Matches the M5Stack hardware spec
  cpu_frequency: 360MHz 
  # Fixes the 4MB/16MB mismatch seen in your boot log
  flash_size: 16MB

# To be able to get logs from the device via serial and api.
logger:

# Enable Home Assistant API
api:
  encryption:
    # Just leave the one it generates for you
    key: "***="

ota:
  - platform: esphome

# Network interface for Unit PoE P4
ethernet:
  # Using JL1101 to fix the "Wrong Chip OUI" error
  type: IP101
  # Verified from Schematic Page 3 Table: MDC=G22, MDIO=G21
  mdc_pin: GPIO31
  mdio_pin: GPIO52
  phy_addr: 1
  # Verified from Schematic Page 1: EN DCDC = G23
  power_pin: GPIO51
  clk: 
    # Verified from Schematic Page 1: REF_CLK = G50
    pin: 50
    mode: CLK_EXT_IN

# i2c:
#   sda: 53
#   scl: 54
#   scan: true
#   id: bus_1

web_server:
  port: 80</pre>
