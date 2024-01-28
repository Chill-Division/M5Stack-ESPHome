#include "tdr_soil_sensor.h"
#include "esphome/core/log.h"
static const char *TAG = "TDR Soil Sensor" ;
static const char* TAG_TEMP = "tdr_temperature_sensor ";
static const char* TAG_EC = "tdr_ec_sensor ";
static const char* TAG_WC = "tdr_wc_sensor ";


const float TEMP_CORRECTION = 0.5;
const float HUM_CORRECTION = 0.8;
const float EC_SLOPE = 1.93;
const float EC_INTERCEPT = -270.8;
const float EC_TEMP_COEFF = 0.019;

namespace esphome {
    namespace tdr_soil_sensor__{
           struct SensorData sensor_data ={0.0};
            void TDR_Soil_Sensor::setup(){
                ESP_LOGI(TAG, "Initialization");
            }
            void TDR_Soil_Sensor::update() {
                ESP_LOGI(TAG, "Computing sensor data");
                

                float soil_hum = 0.1 * int(sensor_data.soil_hum);
                float soil_temp = 0.1 * int(sensor_data.soil_temp);
                int soil_ec = int(sensor_data.soil_ec);

                // EC, Humidity, and Temperature Correction
                // Soil EC Equations obtained from calibration using distilled water and a 1.1178 mS/cm solution
                soil_ec = EC_SLOPE * soil_ec + EC_INTERCEPT;
                soil_ec = soil_ec / (1.0 + EC_TEMP_COEFF * (soil_temp - 25));

                // Study by Shang et al. 2020 showed that the temperature readings of THC-S Sensor were consistently lower than those of Teros 11 by 0.5 degrees Celsius
                soil_temp = soil_temp + TEMP_CORRECTION;
                
                // 80% soil humidity correction for rockwool
                soil_hum = soil_hum * HUM_CORRECTION;

                // Updated Formula for calculating the apparent dielectric constant of soil based on Briciu-Burghina 2022. Corrected for Rockwool
                float soil_apparent_dieletric_constant = 1.68 + 0.267 * soil_hum - 0.00476 * soil_temp + 0.000101 * soil_temp * soil_temp;
                float soil_bulk_permittivity = soil_apparent_dieletric_constant;
                float soil_pore_permittivity = 80.3 / (1.0 + 0.06 * (soil_temp - 25.0));

                // Calculate soil pore water EC using the Briciu-Burghina 2022 model
                float soil_pore_water_ec;
                if (soil_bulk_permittivity > 5.5)
                soil_pore_water_ec = (soil_ec * (soil_pore_permittivity - 5.5)) / (soil_bulk_permittivity - 5.5) / 1000.0;
                else
                soil_pore_water_ec = 0.0;



                soil_temperature_sensor_->publish_state(soil_temp);
                soil_humidity_sensor_->publish_state(soil_hum);
                soil_pw_ec_sensor_->publish_state(soil_pore_water_ec/10.0);
           
            }
            void TDR_Soil_Sensor::dump_config(){
                
            };
          
          void TDR_Temp_Sensor::dump_config(){LOG_SENSOR(TAG_TEMP, "Modbus Controller Sensor", this);};

          void TDR_Temp_Sensor::parse_and_publish(const std::vector<uint8_t> &data) {
              
            float result = payload_to_float(data, *this);
            // Is there a lambda registered
            // call it with the pre converted value and the raw data array
            if (this->transform_func_.has_value()) {
                // the lambda can parse the response itself
                auto val = (*this->transform_func_)(this, result, data);
                if (val.has_value()) {
                ESP_LOGV(TAG_TEMP, "Value overwritten by lambda");
                result = val.value();
                }
            }
            
            sensor_data.soil_temp = result ;
            result = result/10.0 ;
           
            ESP_LOGD(TAG_TEMP, "Sensor new state: %.02f", result);
            // this->sensor_->raw_state = result;
            this->publish_state(result);


        }



        void TDR_EC_Sensor::parse_and_publish(const std::vector<uint8_t> &data) {
              
            float result = payload_to_float(data, *this);
            // Is there a lambda registered
            // call it with the pre converted value and the raw data array
            if (this->transform_func_.has_value()) {
                // the lambda can parse the response itself
                auto val = (*this->transform_func_)(this, result, data);
                if (val.has_value()) {
                ESP_LOGV(TAG_EC, "Value overwritten by lambda");
                result = val.value();
                }
            }
            
            sensor_data.soil_ec = result ;

            result = result/100.0 ;
           
            ESP_LOGD(TAG_EC, "Sensor new state: %.02f", result);
            // this->sensor_->raw_state = result;
            this->publish_state(result);


        }
        void TDR_EC_Sensor::dump_config() { LOG_SENSOR(TAG_EC, "Modbus Controller Sensor", this); }


        void TDR_WC_Sensor::parse_and_publish(const std::vector<uint8_t> &data){
              
            float result = payload_to_float(data, *this);
            // Is there a lambda registered
            // call it with the pre converted value and the raw data array
            if (this->transform_func_.has_value()) {
                // the lambda can parse the response itself
                auto val = (*this->transform_func_)(this, result, data);
                if (val.has_value()) {
                ESP_LOGV(TAG_WC, "Value overwritten by lambda");
                result = val.value();
                }
            }

            sensor_data.soil_hum = result ;
            result = result/10.0 ;
           
            ESP_LOGD(TAG_WC, "Sensor new state: %.02f", result);
            // this->sensor_->raw_state = result;
            this->publish_state(result);


        }

        void TDR_WC_Sensor::dump_config() { LOG_SENSOR(TAG_WC, "Modbus Controller Sensor", this); }

    };

};
