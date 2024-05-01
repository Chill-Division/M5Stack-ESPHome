### 4-Relay Unit

https://shop.m5stack.com/products/4-relay-unit

Big thanks to KoenBreeman for this custom component!

<pre>
external_components:
  - source:
      type: git
      url: https://github.com/KoenBreeman/ESPHome_custom_components/
      ref: main
    components: m5stack4relay

i2c:
  - id: i2c_0
    sda: 2
    scl: 1

switch:
  - platform: m5stack4relay
    sync_mode: true
    id: fourRelays
    relay1:
      id: relay_1
      name: "Relay 1"
    relay2:
      id: relay_2
      name: "Relay 2"
    relay3:
      id: relay_3
      name: "Relay 3"
    relay4:
      id: relay_4
      name: "Relay 4"
</pre>
