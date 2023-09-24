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
)

CONF_SDA_PIN = "sda_pin"
CONF_SCL_PIN = "scl_pin"
CONF_I2C_ADDR = 0x57

CONF_PULSE_TIME = "pulse_time"
MULTI_CONF = True

DEPENDENCIES = ['i2c']
AUTO_LOAD = ["sensor"]
sonic_sensor_ns = cg.esphome_ns.namespace("sonic_i2c_sensor")
SonicI2C = sonic_sensor_ns.class_("SonicI2C",sensor.Sensor, i2c.I2CDevice, cg.PollingComponent)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        SonicI2C,
        unit_of_measurement=UNIT_METER,
        icon=ICON_ARROW_EXPAND_VERTICAL,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(cv.polling_component_schema("60s")
    .extend(i2c.i2c_device_schema(CONF_I2C_ADDR)))
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    #var =  cg.new_Pvariable(config[CONF_ID])
    #await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)


    