### VH3.96 - 4Pin Transfer Module Unit

https://shop.m5stack.com/products/3-96-transfer-unit

This unit is perfect for connecting an SDI-12 substrate sensor such as the BGT-SEC(Z2) by [BGT Technology](https://www.alibaba.com/product-detail/China-low-price-CE-IP68-SID12_1600643601689.html) available from Alibaba, or alternatively works with the [Aroya Teros12](https://metergroup.com/products/teros-12/) as well.

![image](https://github.com/user-attachments/assets/969091b2-eace-43b3-a0e3-930949916697)

For the BGT-SEC(Z2), the unshielded cable is ground, white is power (3.6-16v DC) and the red cable is signal.

There are two parts, the first is replacing the esp32 part and adding the external components. **NOTE:** You _must_ be running the esp-idf framework or the sensor won't give any readings.

<pre>
esp32:
  board: m5stack-core-esp32
  framework:
    type: esp-idf
external_components:
  - source: github://ssieb/esphome@uarthalf
    components: [ uart ]
    refresh: 1min
  - source: github://ssieb/esphome_components@sdi12
    components: [ sdi12 ]
    refresh: 1min
</pre>

The second part is the half-duplex UART and SDI12 implementation:
<pre>
uart:
  - id: sdi12uart
    tx_pin:
      number: GPIO16
      inverted: true
    baud_rate: 1200
    data_bits: 7
    parity: even
    half_duplex: true
    debug:
      direction: BOTH
      after:
        timeout: 20ms
      sequence:
        - lambda: UARTDebug::log_string(direction, bytes);

sdi12:
  - uart_id: sdi12uart
    id: sdibus

sensor:
  - platform: sdi12
    address: 0
    update_interval: 10s
    sensors:
      - index: 3
        name: pwEC
        icon: mdi:electron-framework
        accuracy_decimals: 2
        unit_of_measurement: 'EC (ppm500)'
        filters:
          -  lambda: return x / 1000;
      - index: 2
        name: Temperature
        accuracy_decimals: 1
        unit_of_measurement: '°C'
      - index: 1  # Corrected index for vWC
        name: vWC
        accuracy_decimals: 2
        unit_of_measurement: '%'
        filters:
          - lambda: |-
              float RAW = x;
              float vwc = 6.771e-10 * RAW * RAW * RAW - 5.105e-6 * RAW * RAW + 1.302e-2 * RAW - 10.848;
              // Scale the vWC value from 0-100 to 0-80
              vwc = vwc * 0.8;
              // Multiply by 100 to get the percentage
              return vwc * 100;
</pre>
