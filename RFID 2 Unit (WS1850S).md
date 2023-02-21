### RFID 2 Unit (WS1850S)

https://shop.m5stack.com/products/rfid-unit-2-ws1850s?variant=40753463885996

<pre>
external_components:
  - source:
      type: git
      url: https://github.com/chill-Division/M5Stack-ESPHome/
      ref: main
    components: mfrc522_i2c

i2c:
  sda: 16
  scl: 17
  scan: true
  id: bus_1

mfrc522_i2c:
  i2c_id: bus_1
  address: 0x28 # I2C adress 
  update_interval: 200ms # It seems happiest with 200ms minimum
  on_tag:
    then:
      - homeassistant.tag_scanned: !lambda 'return x;'

# Link the RFID reader to binary sensor
binary_sensor:
  - platform: rc522
    uid: 83-08-60-1A
    name: "NFC Card 1"
</pre>
