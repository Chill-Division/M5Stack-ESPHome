### NanoC6 Dev Kit

https://shop.m5stack.com/products/m5stack-nanoc6-dev-kit?variant=44674391277825

This is a generic template needed to make it play nice, without any sensors plugged in.

<pre>
  esphome:
  name: m5stack-nanoc6
  friendly_name: M5Stack NanoC6

esp32:
  board: esp32-c6-devkitc-1
  variant: esp32c6
  flash_size: 4MB
  framework:
    type: esp-idf
    version: 5.1.2
    platform_version: 6.5.0 # Need at least 6.4 for ESP32-C6
    sdkconfig_options:
      CONFIG_ESPTOOLPY_FLASHSIZE_4MB: y
</pre>

If you are getting errors such as:
<pre>A fatal error occurred: Contents of segment at SHA256 digest offset 0xb0 are not all zero. Refusing to overwrite.</pre>
Clean build files and ensure the flash_size is set correctly as-per above until full native support is added into ESPHomee
