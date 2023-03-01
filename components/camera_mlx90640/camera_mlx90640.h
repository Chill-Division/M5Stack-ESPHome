#ifndef __MLX90640__
#define __MLX90640__
#include<esphome.h>
#include "esphome/components/web_server_base/web_server_base.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"
#include <Wire.h>


namespace esphome {
    namespace mlx90640_app{
         //class MLXDriver ;
         //class MLXApi ;

         class MLX90640: public PollingComponent {
              private:
                TwoWire *wire ;
                uint8_t addr_ ;
                uint8_t sda_ ;
                uint8_t scl_ ;
                float mintemp_;
                float maxtemp_; 
                int frequency_ ;
                int refresh_rate_ = -1 ;
                float filter_level_= 10.0 ;
                web_server_base::WebServerBase *base_;
                sensor::Sensor *min_temperature_sensor_{nullptr} ;
                sensor::Sensor *max_temperature_sensor_{nullptr};
                sensor::Sensor *mean_temperature_sensor_{nullptr};
                sensor::Sensor *median_temperature_sensor_{nullptr};
                //sensor::Sensor *min_index ;
                // sensor::Sensor *max_index ;
              public:
                MLX90640(web_server_base::WebServerBase *base);
               float get_setup_priority() const override { return setup_priority::LATE; }
               void setup() override ;
               void update() override ;
               void create_image();
               void mlx_update() ;
               void set_min_temperature_sensor(sensor::Sensor *ts){this->min_temperature_sensor_ = ts;}
               void set_max_temperature_sensor(sensor::Sensor *ts){this->max_temperature_sensor_= ts;};
               void set_mean_temperature_sensor(sensor::Sensor *ts){this->mean_temperature_sensor_= ts;};
               void set_median_temperature_sensor(sensor::Sensor *ts){this->median_temperature_sensor_= ts;};
               void set_addr(uint8_t addr){this->addr_ = addr;}
               void set_sda(uint8_t sda){this->sda_ = sda ;}
               void set_scl(uint8_t scl){this->scl_ = scl ;}
               void set_frequency(int freq){this->frequency_ = freq ;}
               void set_mintemp(float min ){this->mintemp_ = min ;}
               void set_maxtemp(float max ){this->maxtemp_ = max ;}
               void set_refresh_rate(int refresh){this->refresh_rate_ = refresh;}
              
               // filtering function
               void set_filter_level(float level){this->filter_level_ = level ;}
               void filter_outlier_pixel(float *pixels , int size , float level);
               
        };
    }
}


#endif