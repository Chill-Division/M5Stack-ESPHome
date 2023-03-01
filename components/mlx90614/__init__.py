import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor, i2c
from esphome.const import (
    CONF_ID,
    CONF_TIMEOUT,
    STATE_CLASS_MEASUREMENT,
    UNIT_METER, 
    ICON_ARROW_EXPAND_VERTICAL,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    STATE_CLASS_MEASUREMENT,
    
)



CONF_AMBIENT_TEMPERATURE = "ambient_temperature"
CONF_OBJECT_TEMPERATURE = "object_temperature"
AUTO_LOAD = ["sensor"]
mlx90614_ns = cg.esphome_ns.namespace("mlx_90614")
MLX90614 = mlx90614_ns.class_("MLX90614Sensor", cg.PollingComponent)

CONFIG_SCHEMA = (
    cv.Schema({
      cv.GenerateID(): cv.declare_id(MLX90614),
      cv.Optional(CONF_AMBIENT_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
        ),

        cv.Optional(CONF_OBJECT_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
        ),
       }
    )
    .extend(cv.polling_component_schema("60s"))
)


async def to_code(config):
    var =  cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    if CONF_AMBIENT_TEMPERATURE in config:
        conf = config[CONF_AMBIENT_TEMPERATURE]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_ambient_temperature_sensor(sens))
    
    if CONF_OBJECT_TEMPERATURE in config:
        conf = config[CONF_OBJECT_TEMPERATURE]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_object_temperature_sensor(sens))
    

    