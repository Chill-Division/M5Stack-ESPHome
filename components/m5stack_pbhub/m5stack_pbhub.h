#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
//#include "esphome/components/i2c/i2c.h"
#include "porthub.h"
namespace esphome {
namespace m5stack_pbhub {

class M5StackPBHUBComponent : public Component/*, public i2c::I2CDevice*/ {
 public:
  M5StackPBHUBComponent() = default;


  /// Check i2c availability and setup masks
  void setup() override;
  /// Helper function to read the value of a pin.
  bool digital_read(uint8_t pin);
  /// Helper function to write the value of a pin.
  void digital_write(uint8_t pin, bool value);
  /// Helper function to set the pin mode of a pin.
  void pin_mode(uint8_t pin, gpio::Flags flags);

  float get_setup_priority() const override;

  void dump_config() override;
  void set_sda_pin(uint8_t sda){this->sda_ = sda ;}
  void set_scl_pin(uint8_t scl){this->scl_ = scl ;}
  void set_address(uint8_t addr){this->address_ = addr ;}
  void scan_devices(TwoWire *wire_ );
  bool read_gpio_();
  PortHub *portHub;
  bool write_gpio_();
  uint8_t sda_ ;
  uint8_t scl_ ;
  uint8_t address_ ;
  /// Mask for the pin mode - 1 means output, 0 means input
  uint16_t mode_mask_{0x00};
  /// The mask to write as output state - 1 means HIGH, 0 means LOW
  uint16_t output_mask_{0x00};
  /// The state read in read_gpio_ - 1 means HIGH, 0 means LOW
  uint16_t input_mask_{0x00};
 
};

/// Helper class to expose a M5Stack PBHUB pin as an internal input GPIO pin.
class PBHUBGPIOPin : public GPIOPin {
 public:
  void setup() override;
  void pin_mode(gpio::Flags flags) override;
  bool digital_read() override;
  void digital_write(bool value) override;
  std::string dump_summary() const override;

  void set_parent(M5StackPBHUBComponent *parent) { parent_ = parent; }
  void set_pin(uint8_t pin) { pin_ = pin; }
  void set_inverted(bool inverted) { inverted_ = inverted; }
  void set_flags(gpio::Flags flags) { flags_ = flags; }

 protected:
  M5StackPBHUBComponent *parent_;
  uint8_t pin_;
  bool inverted_;
  gpio::Flags flags_;
};

}  // namespace m5stack_pbhub
}  // namespace esphome
