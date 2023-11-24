### Thermal Camera Unit (MLX90640)

https://shop.m5stack.com/products/thermal-camera


Under your "esphome" section of your yaml, you'll need to add some additional includes / libraries:
<pre>
esphome:
  name: "espatom-thermalcamera"
  libraries:
    - FS
    - Wire
    - SPIFFS
</pre>

Then you can add the main configuration for the unit:
<pre>
external_components:
  - source:
      type: git
      url: https://github.com/chill-Division/M5Stack-ESPHome/
      ref: main
    components: camera_mlx90640

# This is neccessary to load the text_sensor library, can be left empty though
text_sensor:

# The Thermal Camera sensor itself
sensor:
camera_mlx90640:
  id: thermal_cam
  update_interval: 5s
  sda: 26  # I2C SDA PIN
  scl: 32  # I2C SCL PIN
  frequency: 400000  # I2C Clock Frequency
  address: 0x33 # MLX90640 Address
  mintemp: 15 # Minimal temperature for color mapping
  maxtemp: 40 # Maximal temperature for color mapping
  refresh_rate: 0x04 # 0x05 For 16Hz or 0x04 for 8Hz
  min_temperature:
      name: "MLX90640 Min temp"
  max_temperature:
      name: "MLX90640 Max temp"
  mean_temperature:
      name: "MLX90640 Mean temp"
  median_temperature:
      name: "MLX90640 Median temperature"

web_server:
  port: 80
</pre>

You can also browse to the device with the URL:
<pre>/thermal-camera</pre>
It will draw a bmp image (You'll need to zoom right in) of the heatmap the camera can see:
![image](https://github.com/Chill-Division/M5Stack-ESPHome/assets/162461/8b2eca54-a286-4f0d-8359-084e0a500b2d)

Note: This is a BMP image if you save it directly
