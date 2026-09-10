"""ESPHome display platform for the M5Stack Unit LCD (U120).

The Unit LCD is a 1.14" 135x240 ST7789V2 panel driven by an internal
ESP32-PICO that exposes a drawing command set over I2C (default 0x3E).
"""

import esphome.codegen as cg
from esphome.components import display, i2c
import esphome.config_validation as cv
from esphome.const import (
    CONF_BRIGHTNESS,
    CONF_ID,
    CONF_INVERT_COLORS,
    CONF_LAMBDA,
    CONF_PAGES,
)

DEPENDENCIES = ["i2c"]

m5unit_lcd_ns = cg.esphome_ns.namespace("m5unit_lcd")
M5UnitLCD = m5unit_lcd_ns.class_(
    "M5UnitLCD", cg.PollingComponent, display.DisplayBuffer, i2c.I2CDevice
)

CONFIG_SCHEMA = cv.All(
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(M5UnitLCD),
            cv.Optional(CONF_BRIGHTNESS, default=1.0): cv.percentage,
            cv.Optional(CONF_INVERT_COLORS, default=False): cv.boolean,
        }
    ).extend(i2c.i2c_device_schema(0x3E)),
    cv.has_at_most_one_key(CONF_PAGES, CONF_LAMBDA),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_brightness(int(round(config[CONF_BRIGHTNESS] * 255))))
    cg.add(var.set_invert_colors(config[CONF_INVERT_COLORS]))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
