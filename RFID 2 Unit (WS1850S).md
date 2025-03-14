### RFID 2 Unit (WS1850S)

https://shop.m5stack.com/products/rfid-unit-2-ws1850s?variant=40753463885996

<pre>

i2c:
  sda: GPIOx
  scl: GPIOy

rc522_i2c:
  address: 0x28
  on_tag:
    then:
      - homeassistant.tag_scanned: !lambda 'return x;'

# Link the RFID reader to binary sensor
binary_sensor:
  - platform: rc522
    uid: 83-08-60-1A
    name: "NFC Card 1"
</pre>
