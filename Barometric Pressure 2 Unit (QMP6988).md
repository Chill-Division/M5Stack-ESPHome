### Barometric Pressure 2 Unit (QMP6988)

https://shop.m5stack.com/products/barometric-pressure-unitqmp6988

<pre>
i2c:
  sda: 16
  scl: 17
  scan: true
  id: bus_1

#BPS
sensor:
  - platform: qmp6988
    temperature:
      name: "Temperature - BPS"
      oversampling: 16x
    pressure:
      name: "Pressure"
      oversampling: 16x
    address: 0x70
    update_interval: 10s
    iir_filter: 8x
</pre>
