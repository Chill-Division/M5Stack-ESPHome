### ENV Pro Unit with Temperature, Humidity, Pressure and Gas Sensor (BME688)

https://shop.m5stack.com/products/env-pro-unit-with-temperature-humidity-pressure-and-gas-sensor-bme688

Thanks to DavidGeorge528 for their assistance with this sensor / config

Example sensor output:

![image](https://github.com/user-attachments/assets/f837c41e-dc31-48cc-a742-28e1b3a8c290)


<pre>
i2c:
  sda: 16
  scl: 17
  frequency: 400kHz

bme68x_bsec2_i2c:
  model: bme688
  state_save_interval: 5min
  address: 0x77

sensor:
  - platform: bme68x_bsec2
    temperature:
      name: "Temperature"
      filters:
        - median
    pressure:
      name: "Pressure"
      filters:
        - median
    humidity:
      name: "Humidity"
      filters:
        - median
    iaq:
      name: "IAQ"
      id: iaq
      filters:
        - median
      on_value:
        - component.update: iaq_classification
    iaq_static:
      name: "Static IAQ"
      id: static_iaq
      filters:
        - median
      on_value:
        - component.update: static_iaq_classification
    co2_equivalent:
      name: "CO2 equivalent"
      filters:
        - median
    breath_voc_equivalent:
      name: "Breath VOC equivalent"
      filters:
        - median
    gas_resistance:
      # Gas resistance in Ω
      name: "Gas resistance"
      filters:
        - median

text_sensor:
  - platform: template
    name: "IAQ classification"
    id: iaq_classification
    icon: "mdi:checkbox-marked-circle-outline"
    lambda: |-
      auto iaq_ = int(id(iaq).state);
      if (iaq_ < 0) {
        // No IAQ sensor.
        return {"unknown"};
      }
      if (iaq_ <= 50) {
        return {"excellent"};
      }
      else if (iaq_ <= 100) {
        return {"good"};
      }
      else if (iaq_ <= 150) {
        return {"lightly polluted"};
      }
      else if (iaq_ <= 200) {
        return {"moderately polluted"};
      }
      else if (iaq_ <= 250) {
        return {"heavily polluted"};
      }
      else if (iaq_ <= 350) {
        return {"severely polluted"};
      }
      else if (iaq_ <= 500) {
        return {"extremely polluted"};
      }
      else {
        // ESP_LOGD("iaq-classification", "IAQ not yet established: %s", id(static_iaq).state);
        return {"unknown"};
      }
    update_interval: never

  - platform: template
    name: "Static IAQ classification"
    id: static_iaq_classification
    icon: "mdi:checkbox-marked-circle-outline"
    lambda: |-
      auto iaq_ = int(id(static_iaq).state);
      if (iaq_ < 0) {
        // No IAQ sensor.
        return {"unknown"};
      }
      if (iaq_ <= 50) {
        return {"excellent"};
      }
      else if (iaq_ <= 100) {
        return {"good"};
      }
      else if (iaq_ <= 150) {
        return {"lightly polluted"};
      }
      else if (iaq_ <= 200) {
        return {"moderately polluted"};
      }
      else if (iaq_ <= 250) {
        return {"heavily polluted"};
      }
      else if (iaq_ <= 350) {
        return {"severely polluted"};
      }
      else if (iaq_ <= 500) {
        return {"extremely polluted"};
      }
      else {
        // ESP_LOGD("iaq-classification", "IAQ not yet established: %s", id(static_iaq).state);
        return {"unknown"};
      }
    update_interval: never
  - platform: bme68x_bsec2
    iaq_accuracy: 
      name: "IAQ accuracy"</pre>
