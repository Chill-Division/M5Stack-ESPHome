#ifndef __MLX90640__
#define __MLX90614__
#pragma once 
#include "esphome.h"
#include <Wire.h>
#include "Adafruit_MLX90614.h"
#include "esphome/components/sensor/sensor.h"
namespace esphome {
namespace mlx_90614{
class MLX90614Sensor : public PollingComponent {
    private:
    Adafruit_MLX90614 mlx;
    sensor::Sensor *ambient_temperature_sensor{nullptr};
    sensor::Sensor *object_temperature_sensor{nullptr};

  public:

   // MLX90614Sensor() : PollingComponent(15000) { }
    void set_ambient_temperature_sensor(sensor::Sensor *ts){this->ambient_temperature_sensor = ts ;}
    void set_object_temperature_sensor(sensor::Sensor *ts){this->object_temperature_sensor = ts ;}
    void setup() override ;
    void update() override;
};

}
}

#endif