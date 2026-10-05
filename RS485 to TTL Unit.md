### RS485 to TTL Unit

https://shop.m5stack.com/products/rs485-module

This example includes sensor details for a [THC-S Soil Moisture, Temperature and Conductivity Sensor](https://www.aliexpress.com/item/1005001524845572.html?spm=a2g0o.order_list.order_list_main.5.6a5e1802E8jtxz) as a demonstration of how to connect the device

<pre>

# Setup the UART bus for RS485
uart:
  id: uart_bus
  tx_pin: 53
  rx_pin: 54
  baud_rate: 4800 # THC-S default
  stop_bits: 1

# Configure Modbus
modbus:
  uart_id: uart_bus
  id: modbus_hub

modbus_controller:
  - id: substrate_probe
    address: ${modbus_address}
    modbus_id: modbus_hub
    setup_priority: -10
    update_interval: ${poll_interval}
    on_online:
      - logger.log: "Substrate probe is responding on Modbus"
    on_offline:
      - logger.log:
          level: WARN
          format: "Substrate probe stopped responding on Modbus"

number:
  # Register 0x0022, "Conductivity factor": 0-100 = 0.0-10.0 %/°C (probe default 0).
  # Set at every boot from ec_temp_coeff above; shown here so you can see and adjust it.
  - platform: modbus_controller
    modbus_controller_id: substrate_probe
    name: "Substrate EC Temp Coefficient"
    id: substrate_ec_temp_coeff
    address: 0x0022
    register_type: holding
    value_type: U_WORD
    multiply: 10          # 2.0 %/°C <-> register value 20
    min_value: 0
    max_value: 10
    step: 0.1
    unit_of_measurement: "%/°C"
    entity_category: config

sensor:
  # Moisture. The THC-S calls it "Humidity" (0.1 % steps). It is the probe's own soil
  # scale, not true moisture in coco, until calibrated below.
  # Keep this name: it is what keeps the Home Assistant entity id.
  - platform: modbus_controller
    modbus_controller_id: substrate_probe
    name: "Substrate Humidity"
    id: substrate_humidity
    address: 0x0000
    register_type: read   # input registers (function 0x04), as this device already reads them
    value_type: U_WORD
    unit_of_measurement: "%"
    device_class: moisture
    state_class: measurement
    accuracy_decimals: 1
    filters:
      - multiply: 0.1
      # Coco calibration goes here once measured: probe reading -> true moisture.
      # MADE-UP NUMBERS: replace them with yours before uncommenting.
      # - calibrate_linear:
      #     method: least_squares
      #     datapoints:
      #       - 87.0 -> 60.0   # drained
      #       - 70.0 -> 42.0   # dried back

  # Temperature, 0.1 °C steps. Below 0 °C the probe sends two's complement, hence S_WORD.
  - platform: modbus_controller
    modbus_controller_id: substrate_probe
    name: "Substrate Temperature"
    id: substrate_temperature
    address: 0x0001
    register_type: read
    value_type: S_WORD
    unit_of_measurement: "°C"
    device_class: temperature
    state_class: measurement
    accuracy_decimals: 1
    filters:
      - multiply: 0.1

  # Bulk EC as the probe reports it, 1 µS/cm steps.
  - platform: modbus_controller
    modbus_controller_id: substrate_probe
    name: "Substrate Conductivity"
    id: substrate_conductivity
    address: 0x0002
    register_type: read
    value_type: U_WORD
    unit_of_measurement: "µS/cm"
    state_class: measurement
    accuracy_decimals: 0

  # Bulk EC in mS/cm: Substrate Conductivity ÷ 1000.
  # (It used to be labelled ppm500, but the value was always mS/cm.)
  - platform: template
    name: "Substrate Bulk EC"
    id: substrate_bulk_ec
    unit_of_measurement: "mS/cm"
    state_class: measurement
    accuracy_decimals: 2
    update_interval: ${poll_interval}
    lambda: |-
      const float ec_us = id(substrate_conductivity).state;
      if (std::isnan(ec_us)) return NAN;
      return ec_us / 1000.0f;

  # Estimated pore-water EC in mS/cm: bulk EC ÷ moisture. A rough estimate (a proper
  # one needs permittivity, which this probe doesn't report), so read it as a trend.
  # It uses the moisture above, so it improves once that is calibrated.
  - platform: template
    name: "Substrate Estimated pwEC"
    id: estimated_pwec
    unit_of_measurement: "mS/cm"
    state_class: measurement
    accuracy_decimals: 2
    update_interval: ${poll_interval}
    lambda: |-
      const float ec_us = id(substrate_conductivity).state;
      const float vwc_pct = id(substrate_humidity).state;
      if (std::isnan(ec_us) || std::isnan(vwc_pct)) return NAN;
      // Below ~5 % moisture the ratio means nothing: report unknown, never 0.
      if (vwc_pct < 5.0f) return NAN;
      return (ec_us / 1000.0f) / (vwc_pct / 100.0f);

  # The same estimate on the ppm500 scale, to compare with a runoff or reservoir
  # TDS meter set to 500. Not for Crop Steering, which reads mS/cm.
  - platform: template
    name: "Substrate Estimated pwEC (ppm500)"
    id: estimated_pwec_ppm500
    unit_of_measurement: "ppm"
    state_class: measurement
    accuracy_decimals: 0
    update_interval: ${poll_interval}
    lambda: |-
      const float pwec = id(estimated_pwec).state;
      if (std::isnan(pwec)) return NAN;
      return pwec * 500.0f;
</pre>
