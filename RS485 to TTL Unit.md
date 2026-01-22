### RS485 to TTL Unit

https://shop.m5stack.com/products/rs485-module

This example includes sensor details for a [THC-S Soil Moisture, Temperature and Conductivity Sensor](https://www.aliexpress.com/item/1005001524845572.html?spm=a2g0o.order_list.order_list_main.5.6a5e1802E8jtxz) as a demonstration of how to connect the device

<pre>
# Setup the UART bus for RS485
uart:
  id: uart_bus
  tx_pin: 16 # < for PoESP32 ... #26  # Standard M5Stack Atom Grove Pin
  rx_pin: 17 # < for PoESP32 ... #32  # Standard M5Stack Atom Grove Pin
  baud_rate: 4800 # Default per THC-S manual 
  stop_bits: 1

# Configure Modbus
modbus:
  uart_id: uart_bus
  id: modbus_hub

modbus_controller:
  - id: substrate_probe
    address: 0x01 # Default Device Address is 1
    modbus_id: modbus_hub
    setup_priority: -10
    update_interval: 10s

sensor:
  # Humidity
  # Register: 0x0000 | Unit: 0.1%RH 
  - platform: modbus_controller
    modbus_controller_id: substrate_probe
    name: "Substrate Humidity"
    id: substrate_humidity
    address: 0x0000
    register_type: read
    value_type: U_WORD
    unit_of_measurement: "%"
    accuracy_decimals: 1
    filters:
      - multiply: 0.1

  # Temperature
  # Register: 0x0001 | Unit: 0.1°C 
  # Note: Manual states negative values use complement, so we use S_WORD (Signed) 
  - platform: modbus_controller
    modbus_controller_id: substrate_probe
    name: "Substrate Temperature"
    id: substrate_temperature
    address: 0x0001
    register_type: read
    value_type: S_WORD
    unit_of_measurement: "°C"
    accuracy_decimals: 1
    filters:
      - multiply: 0.1

  # Electrical Conductivity (EC)
  # Register: 0x0002 | Unit: 1 us/cm 
  - platform: modbus_controller
    modbus_controller_id: substrate_probe
    name: "Substrate Conductivity"
    id: substrate_conductivity
    address: 0x0002
    register_type: read
    value_type: U_WORD
    unit_of_measurement: "µS/cm"
    accuracy_decimals: 0
    # No multiplier needed as manual says "0-20000us/cm" with value 1 = 1

# The THC-S sensor reads Bulk EC (the conductivity of the soil + water + air mix).
# If you are trying to measure the exact nutrient levels available to the plant roots (Pore Water EC),
# the number will naturally appear lower than what you might see in a hydroponic reservoir tester.
# We derive the Bulk EC from the Conductivity value
  - platform: template
    name: "Substrate Bulk EC"
    id: substrate_bulkec_ppm500
    unit_of_measurement: "ppm500" # Standard unit for "EC"
    accuracy_decimals: 2         # e.g., will show 1.50 mS/cm
    lambda: |-
      if (id(substrate_conductivity).state == NAN) return NAN;
      return id(substrate_conductivity).state / 1000.0;

# The THC-S sensor measures Bulk EC (the conductivity of the entire soil + water + air matrix).
# get Pore Water EC (the conductivity of just the water available to the roots),
# you normally need a more advanced calculation (like the Hilhorst equation) which requires the Dielectric Permittivity (epsilon) of the soil.
# However we can estimate it using pwEC = BulkEC / Moisture
  - platform: template
    name: "Substrate Estimated pwEC"
    id: estimated_pwec
    unit_of_measurement: "ppm500"
    accuracy_decimals: 2
    lambda: |-
      // Prevent division by zero
      if (id(substrate_humidity).state <= 0.1) return 0.0;
      
      // Convert humidity (e.g. 45%) to decimal (0.45)
      float vwc = id(substrate_humidity).state / 100.0;
      
      // Get Bulk EC in mS/cm (using your converted sensor)
      float bulk_ec = id(substrate_bulkec_ppm500).state;

      // Simple pore water approximation
      return bulk_ec / vwc;</pre>
