#pragma once 

#include <esphome.h>
#include "esphome/core/gpio.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome{
namespace sonic_i2c_sensor{
 
class SonicI2C: public i2c::I2CDevice, public sensor::Sensor, public PollingComponent {
    private:

    public:
     void setup() override ;
     void dump_config() override;
     void update() override ;
     float getDistance();

};

}

}