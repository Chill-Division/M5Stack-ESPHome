### Dlight Unit - Ambient Light Sensor (BH1750FVI-TR)

https://shop.m5stack.com/products/dlight-unit-ambient-light-sensor-bh1750fvi-tr

<pre>
i2c:
  sda: 16
  scl: 17
  scan: true
  id: bus_1

sensor:
  - platform: bh1750
    name: "Illuminance"
    id: dlight_illuminance
    address: 0x23
    update_interval: 10s
</pre>
