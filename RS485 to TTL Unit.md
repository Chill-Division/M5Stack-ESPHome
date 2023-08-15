### RS485 to TTL Unit

https://shop.m5stack.com/products/rs485-module

This example includes sensor details for a [THC-S Soil Moisture, Temperature and Conductivity Sensor](https://www.aliexpress.com/item/1005001524845572.html?spm=a2g0o.order_list.order_list_main.5.6a5e1802E8jtxz) as a demonstration of how to connect the device

<pre>
### RS485
uart:
  id: modbus_serial
  rx_pin: 1
  tx_pin: 2
  baud_rate: 4800
  stop_bits: 1

modbus:
  id: modbus1
  uart_id: modbus_serial

modbus_controller:
  - id: con_modbus_name
    update_interval: 11s
    address: 0x1
    command_throttle: 150ms
    setup_priority: -10

sensor:
  - platform: modbus_controller
    name: "Moisture WC"
    id: WC
    address: 0x0000
    unit_of_measurement: "%"
    register_type: read
    value_type: U_WORD
    accuracy_decimals: 1
    filters:
      lambda: return x / 10;
    state_class: measurement
  - platform: modbus_controller
    name: "Temp"
    id: C
    address: 0x0001
    unit_of_measurement: "°C"
    register_type: read
    value_type: U_WORD
    accuracy_decimals: 1
    filters:
      lambda: return x / 10;
    state_class: measurement
  - platform: modbus_controller
    name: "Conductivity 1 dS/m = 1 mS/cm"
    id: EC
    address: 0x0002
    unit_of_measurement: "ms/cm"
    register_type: read
    value_type: U_WORD
    accuracy_decimals: 1
    filters:
      lambda: return x / 100;
    state_class: measurement
</pre>
