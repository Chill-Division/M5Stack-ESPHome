#ifndef __SOIL_SENSOR__
#define __SOIL_SENSOR__
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/log.h"
#include "esphome/components/modbus_controller/modbus_controller.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include <vector>

namespace esphome{
namespace tdr_soil_sensor__{
   struct SensorData
    {
        float soil_ec ;
        float soil_temp ;
        float soil_hum ;
    };
    
    class TDR_Temp_Sensor;
    class TDR_Soil_Sensor :public PollingComponent {
         private:
            sensor::Sensor *soil_temperature_sensor_{nullptr} ;
            sensor::Sensor *soil_humidity_sensor_{nullptr} ;
            sensor::Sensor *soil_pw_ec_sensor_{nullptr} ;
            public:
            void setup() override;
            void update() override ;
            void dump_config() override;
            float get_setup_priority() const override { return  -300; }
            void set_soil_temperature_sensor(sensor::Sensor *s){this->soil_temperature_sensor_ = s ;}
            void set_soil_humidity_sensor(sensor::Sensor *s){this->soil_humidity_sensor_ = s ;}
            void set_soil_pw_ec_sensor(sensor::Sensor *s){this->soil_pw_ec_sensor_ = s ;}
            

    };

    class TDR_Temp_Sensor: public Component, public sensor::Sensor, public modbus_controller::SensorItem{
        public:
            TDR_Temp_Sensor(modbus_controller::ModbusRegisterType register_type, uint16_t start_address, uint8_t offset, uint32_t bitmask,
               modbus_controller::SensorValueType value_type, int register_count, uint8_t skip_updates, bool force_new_range) {
                this->register_type = register_type;
                this->start_address = start_address;
                this->offset = offset;
                this->bitmask = bitmask;
                this->sensor_value_type = value_type;
                this->register_count = register_count;
                this->skip_updates = skip_updates;
                this->force_new_range = force_new_range;
            }

            void parse_and_publish(const std::vector<uint8_t> &data) override;
            void dump_config() override;
            using transform_func_t = std::function<optional<float>(TDR_Temp_Sensor *, float, const std::vector<uint8_t> &)>;

            void set_template(transform_func_t &&f) { this->transform_func_ = f; }

        protected:
            optional<transform_func_t> transform_func_{nullopt};
    };


    class TDR_EC_Sensor: public Component, public sensor::Sensor, public modbus_controller::SensorItem{
        public:
            TDR_EC_Sensor(modbus_controller::ModbusRegisterType register_type, uint16_t start_address, uint8_t offset, uint32_t bitmask,
               modbus_controller::SensorValueType value_type, int register_count, uint8_t skip_updates, bool force_new_range) {
                this->register_type = register_type;
                this->start_address = start_address;
                this->offset = offset;
                this->bitmask = bitmask;
                this->sensor_value_type = value_type;
                this->register_count = register_count;
                this->skip_updates = skip_updates;
                this->force_new_range = force_new_range;
            }

            void parse_and_publish(const std::vector<uint8_t> &data) override;
            void dump_config() override;
            using transform_func_t = std::function<optional<float>(TDR_EC_Sensor *, float, const std::vector<uint8_t> &)>;

            void set_template(transform_func_t &&f) { this->transform_func_ = f; }

        protected:
            optional<transform_func_t> transform_func_{nullopt};
    };

   




    class TDR_WC_Sensor: public Component, public sensor::Sensor, public modbus_controller::SensorItem{
        public:
            TDR_WC_Sensor(modbus_controller::ModbusRegisterType register_type, uint16_t start_address, uint8_t offset, uint32_t bitmask,
               modbus_controller::SensorValueType value_type, int register_count, uint8_t skip_updates, bool force_new_range) {
                this->register_type = register_type;
                this->start_address = start_address;
                this->offset = offset;
                this->bitmask = bitmask;
                this->sensor_value_type = value_type;
                this->register_count = register_count;
                this->skip_updates = skip_updates;
                this->force_new_range = force_new_range;
            }

            void parse_and_publish(const std::vector<uint8_t> &data) override;
            void dump_config() override;
            using transform_func_t = std::function<optional<float>(TDR_WC_Sensor *, float, const std::vector<uint8_t> &)>;

            void set_template(transform_func_t &&f) { this->transform_func_ = f; }

        protected:
            optional<transform_func_t> transform_func_{nullopt};
    };

    



};

};

#endif