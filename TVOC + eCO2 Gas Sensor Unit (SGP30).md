### TVOC/eCO2 Gas Sensor Unit (SGP30)


https://shop.m5stack.com/products/tvoc-eco2-gas-unit-sgp30

<pre>
i2c:
  sda: 16
  scl: 17
  scan: true
  id: bus_1
    
# TVOC/eCO2
# Keep in mind that eCO2 is "Estimated" based off the TVOC
# Basically, it's not really that reliable
# Ideally you should look at an SCD30, SCD40 or SCD41
sensor:
  - platform: sgp30
    eco2:
      name: "eCO2"
      accuracy_decimals: 1
    tvoc:
      name: "TVOC"
      accuracy_decimals: 1
    store_baseline: yes
    address: 0x58
    update_interval: 1s
</pre>
