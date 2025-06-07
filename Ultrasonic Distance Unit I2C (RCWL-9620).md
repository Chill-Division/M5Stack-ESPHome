### Ultrasonic Distance Unit I2C (RCWL-9620)

https://shop.m5stack.com/products/ultrasonic-distance-unit-i2c-rcwl-9620?variant=42084188225793

<b>NOTE:</b> This only works with the I2C unit. The GPIO unit doesn't play nice with ESPHome (I had to install Tasmota instead), so be sure to get the I2C variant which will also then work with the Pa.hub

The template sensor is completely optional, in my instance I have it on the lid of a wheelie-bin that is used as a reservoir so it's helpful for me

The sliding_window_moving_average and other lambda filters are also optional too, but it definitely helps to have nan filtered out etc.

<pre>
external_components:
  - source:
      type: git
      url: https://github.com/chill-Division/M5Stack-ESPHome/
      ref: main
    components: sonic_i2c

i2c:
  sda: 16
  scl: 17
  scan: true
  id: bus_1

sensor:
 - platform: sonic_i2c
   i2c_id: bus_1
   address: 0x57
   name: "Ultrasonic Sensor 1"
   id: ultrasonic1
   unit_of_measurement: mm
   update_interval: 5s
   filters:
   - filter_out: nan
   - lambda: |-
      if (x == 0) {
      return {};  // This filters out the reading
      } else {
      return x;   // This passes the reading through
      }
   - sliding_window_moving_average:
       window_size: 10
       send_every: 20
 - platform: template
   name: "Reservoir Level"
   unit_of_measurement: "%"
   icon: "mdi:waves"
   update_interval: 10s # Or whatever update interval you prefer
   lambda: |-
    auto x = id(ultrasonic_gr1_res).state;
    const float min_distance = 190.0;
    const float max_distance = 880.0;
    if (isnan(x)) {
     return NAN; // Don't publish a value if the sensor reading is invalid
    }
    // Clamp the sensor reading to be within your defined min/max range
    x = clamp(x, min_distance, max_distance);
    // Calculate the percentage
    float percentage = 100.0 - ((x - min_distance) / (max_distance - min_distance) * 100.0);
    return percentage;
</pre>
