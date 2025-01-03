#include "camera_mlx90640.h"
#include "mlx90640_image.h"
#include "SPIFFS.h"
#include "esphome/components/i2c/i2c.h"

static const char *TAG = "MLX90640";

namespace esphome {
namespace mlx90640_app {

MLX90640::MLX90640(web_server_base::WebServerBase *base) : base_(base), i2c_device_(nullptr) {}

void MLX90640::set_i2c_id(i2c::I2CDevice *i2c_device) {
  this->i2c_device_ = i2c_device;
}

void MLX90640::setup() {
  // Multiplexer support: Set up the I2C device
  if (this->i2c_device_ != nullptr) {
    ESP_LOGCONFIG(TAG, "Using I2C device on multiplexer.");
    this->i2c_device_->setup();  // Ensure the selected I2C channel is configured
  } else {
    ESP_LOGCONFIG(TAG, "Using default I2C bus.");
  }

  // Initialize the sensor
  ESP_LOGI(TAG, "SDA PIN: %d", this->sda_);
  ESP_LOGI(TAG, "SCL PIN: %d", this->scl_);
  ESP_LOGI(TAG, "I2C Frequency: %d", this->frequency_);
  ESP_LOGI(TAG, "Address: %d", this->addr_);
  MLX90640_address = this->addr_;
  MINTEMP = this->mintemp_;
  MAXTEMP = this->maxtemp_;

  Wire.begin((int)this->sda_, (int)this->scl_, (uint32_t)this->frequency_);
  Wire.setClock(this->frequency_);
  MLX90640_I2CInit(&Wire);

  uint16_t eeMLX90640[832];
  if (MLX90640_isConnected(MLX90640_address)) {
    int status = MLX90640_DumpEE(MLX90640_address, eeMLX90640);
    if (status != 0) {
      ESP_LOGE(TAG, "Failed to load system parameters.");
      this->mark_failed();
      return;
    }

    status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
    if (status != 0) {
      ESP_LOGE(TAG, "Parameter extraction failed.");
      this->mark_failed();
      return;
    }

    int refresh_rate = this->refresh_rate_ ? this->refresh_rate_ : 0x05;  // Default to 16Hz
    MLX90640_SetRefreshRate(MLX90640_address, refresh_rate);
    ESP_LOGI(TAG, "Refresh rate set to: %d Hz", (refresh_rate == 0x05) ? 16 : 8);
  } else {
    ESP_LOGE(TAG, "The sensor is not connected.");
    this->mark_failed();
    return;
  }

  if (!SPIFFS.begin(true)) {
    ESP_LOGE(TAG, "Error mounting SPIFFS.");
  }

  this->base_->get_server()->on("/thermal-camera", HTTP_GET, [](AsyncWebServerRequest *request) {
    ESP_LOGI(TAG, "Sending the image.");
    request->send(SPIFFS, "/thermal.bmp", "image/bmp", false);
  });
}

void MLX90640::update() {
  if (this->i2c_device_ != nullptr) {
    this->i2c_device_->select_channel();  // Activate the correct multiplexer channel
  }

  // Sensor update logic
  if (MLX90640_isConnected(MLX90640_address)) {
    this->mlx_update();
  } else {
    ESP_LOGE(TAG, "The sensor is not connected.");
    this->mark_failed();
  }
}

void MLX90640::mlx_update() {
  uint16_t mlx90640Frame[834];
  int status = MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);
  if (status < 0) {
    ESP_LOGE(TAG, "GetFrame Error: %d", status);
    return;
  }

  float emissivity = 0.95;
  float tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TA_SHIFT;
  MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, pixels);

  min_v = MAXTEMP;
  max_v = MINTEMP;
  float total = 0;

  for (int i = 0; i < COLS * ROWS; i++) {
    if (pixels[i] > max_v) max_v = pixels[i];
    if (pixels[i] < min_v) min_v = pixels[i];
    total += pixels[i];
  }

  meanTemp = total / (COLS * ROWS);
  medianTemp = (pixels[165] + pixels[180] + pixels[176] + pixels[192]) / 4.0;

  ESP_LOGI(TAG, "Min Temp: %.2f °C", min_v);
  ESP_LOGI(TAG, "Max Temp: %.2f °C", max_v);
  ESP_LOGI(TAG, "Mean Temp: %.2f °C", meanTemp);
  ESP_LOGI(TAG, "Median Temp: %.2f °C", medianTemp);

  if (this->min_temperature_sensor_ != nullptr)
    this->min_temperature_sensor_->publish_state(min_v);
  if (this->max_temperature_sensor_ != nullptr)
    this->max_temperature_sensor_->publish_state(max_v);
  if (this->mean_temperature_sensor_ != nullptr)
    this->mean_temperature_sensor_->publish_state(meanTemp);
  if (this->median_temperature_sensor_ != nullptr)
    this->median_temperature_sensor_->publish_state(medianTemp);
}

void MLX90640::dump_config() {
  ESP_LOGCONFIG(TAG, "MLX90640:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->addr_);
  ESP_LOGCONFIG(TAG, "  SDA Pin: %d", this->sda_);
  ESP_LOGCONFIG(TAG, "  SCL Pin: %d", this->scl_);
  ESP_LOGCONFIG(TAG, "  Frequency: %d Hz", this->frequency_);
  ESP_LOGCONFIG(TAG, "  Min Temp: %d °C", this->mintemp_);
  ESP_LOGCONFIG(TAG, "  Max Temp: %d °C", this->maxtemp_);
}

}  // namespace mlx90640_app
}  // namespace esphome
