#include "camera_mlx90640.h"
#include "mlx90640_image.h"
#include "SPIFFS.h"

static const char *TAG = "MLX90640";
static float pixels[32 * 24]; // Thermal data array
paramsMLX90640 mlx90640;

namespace esphome {
namespace mlx90640_app {

MLX90640::MLX90640(web_server_base::WebServerBase *base) : base_(base) {}

void MLX90640::setup() {
  // Set up I2C device (multiplexer support)
  if (this->i2c_device_ != nullptr) {
    ESP_LOGCONFIG(TAG, "Using I2C device on multiplexer.");
    this->i2c_device_->setup();
  } else {
    ESP_LOGCONFIG(TAG, "Using default I2C bus.");
  }

  ESP_LOGI(TAG, "SDA PIN: %d, SCL PIN: %d", this->sda_, this->scl_);
  ESP_LOGI(TAG, "Frequency: %d Hz, Address: 0x%02X", this->frequency_, this->addr_);
  Wire.begin(this->sda_, this->scl_, this->frequency_);
  MLX90640_I2CInit(&Wire);

  uint16_t eeMLX90640[832];
  if (MLX90640_isConnected(this->addr_)) {
    MLX90640_DumpEE(this->addr_, eeMLX90640);
    MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
    MLX90640_SetRefreshRate(this->addr_, this->refresh_rate_);
  } else {
    ESP_LOGE(TAG, "MLX90640 not detected at address 0x%02X", this->addr_);
    this->mark_failed();
    return;
  }

  if (!SPIFFS.begin(true)) {
    ESP_LOGE(TAG, "Error mounting SPIFFS.");
  }

  this->base_->get_server()->on("/thermal-camera", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/thermal.bmp", "image/bmp", false);
  });
}

void MLX90640::update() {
  // Select I2C channel (multiplexer support)
  if (this->i2c_device_ != nullptr) {
    this->i2c_device_->select_channel();
  }

  uint16_t mlx90640Frame[834];
  if (MLX90640_GetFrameData(this->addr_, mlx90640Frame) < 0) {
    ESP_LOGE(TAG, "Failed to read frame data.");
    return;
  }

  float emissivity = 0.95;
  float tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TA_SHIFT;
  MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, pixels);

  float min_temp = this->maxtemp_;
  float max_temp = this->mintemp_;
  float total = 0;
  for (int i = 0; i < 32 * 24; i++) {
    if (pixels[i] < min_temp) min_temp = pixels[i];
    if (pixels[i] > max_temp) max_temp = pixels[i];
    total += pixels[i];
  }
  float mean_temp = total / (32 * 24);

  if (this->min_temperature_sensor_ != nullptr)
    this->min_temperature_sensor_->publish_state(min_temp);
  if (this->max_temperature_sensor_ != nullptr)
    this->max_temperature_sensor_->publish_state(max_temp);
  if (this->mean_temperature_sensor_ != nullptr)
    this->mean_temperature_sensor_->publish_state(mean_temp);
}

void MLX90640::dump_config() {
  ESP_LOGCONFIG(TAG, "MLX90640:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->addr_);
  ESP_LOGCONFIG(TAG, "  SDA Pin: %d", this->sda_);
  ESP_LOGCONFIG(TAG, "  SCL Pin: %d", this->scl_);
  ESP_LOGCONFIG(TAG, "  Frequency: %d Hz", this->frequency_);
  ESP_LOGCONFIG(TAG, "  Min Temp: %.2f", this->mintemp_);
  ESP_LOGCONFIG(TAG, "  Max Temp: %.2f", this->maxtemp_);
}

} // namespace mlx90640_app
} // namespace esphome
