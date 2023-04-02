#include "m5stack_pbhub.h"
#include "esphome/core/log.h"


static const uint8_t HUB_ADDR[6] = {HUB1_ADDR, HUB2_ADDR, HUB3_ADDR,
                       HUB4_ADDR, HUB5_ADDR, HUB6_ADDR};
namespace esphome {
namespace m5stack_pbhub {

static const char *const TAG = "m5stack_pbhub";

void M5StackPBHUBComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up M5Stack PBHUB...");
  ESP_LOGCONFIG(TAG, "I2C Address : %x ",this->address_);
  ESP_LOGCONFIG(TAG, "SDA : %d ; SCL :  %d ", this->sda_ , this->scl_);
  Wire.begin(this->sda_ , this->scl_) ;
  /*Wire.beginTransmission(this->address_);
  if(Wire.endTransmission()!=0){
    // Check if there is a device connected 
     ESP_LOGE(TAG, "PBHUB not available under 0x%02X", this->address_);
     this->mark_failed();
     return;
  }*/
  this->portHub = new PortHub(this->address_, &Wire);
  
  
}
void M5StackPBHUBComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "PBHUB:");
  //LOG_I2C_DEVICE(this)
  
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with PBHUB failed!");
  }
}
bool M5StackPBHUBComponent::digital_read(uint8_t pin) {
  if (pin==0){
    return portHub->hub_d_read_value_B(HUB_ADDR[pin]);
  }else if(pin==1){
   return  portHub->hub_d_read_value_A(HUB_ADDR[pin]);
  }
  // Find the real pin number from the pin 
  uint8_t pin2 = pin /10 ;
  if(pin%2==0){
   return  portHub->hub_d_read_value_B(HUB_ADDR[pin2]);
  }
  return portHub->hub_d_read_value_A(HUB_ADDR[pin2]);
  
}
void M5StackPBHUBComponent::digital_write(uint8_t pin, bool value) {
  //ESP_LOGCONFIG(TAG, "PIN : %d ", pin) ;
  uint8_t val = value? 0xFF: 0 ;
  /*if (pin==0){
    portHub->hub_d_wire_value_B(HUB_ADDR[pin],val);
    return ;
  }else if(pin==1){
   portHub->hub_d_wire_value_A(HUB_ADDR[0],val);
   return ;
  }/*
if (pin==10){
    portHub->hub_d_wire_value_B(HUB_ADDR[1],val);
    return ;
  }else if(pin==11){
   portHub->hub_d_wire_value_A(HUB_ADDR[1],val);
   return ;
  }*/

  uint8_t channel = pin /10 ;
  //ESP_LOGCONFIG(TAG, "PIN : %d ; channel : %d", pin, channel) ;
  if(pin%2==0){
   portHub->hub_d_wire_value_B(HUB_ADDR[channel],val);
   return;
  }else{
    portHub->hub_d_wire_value_A(HUB_ADDR[channel],val);
    return;
  }
  
}
void M5StackPBHUBComponent::pin_mode(uint8_t pin, gpio::Flags flags) {
 // No pin mode configuration required
}
bool M5StackPBHUBComponent::read_gpio_() {
  //Not implemented
  return true;
}
bool M5StackPBHUBComponent::write_gpio_() {
  //Not implemented
  return true;
}
void M5StackPBHUBComponent::scan_devices(TwoWire *wire_ ){
  uint8_t error, address;
  int nDevices;

  ESP_LOGCONFIG(TAG,"Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    wire_->beginTransmission(address);
    error = wire_->endTransmission();

    if (error == 0)
    {
      ESP_LOGCONFIG(TAG,"I2C device found at address : %x", address);
      nDevices++;
    }
    else if (error==4) 
    {
     ESP_LOGCONFIG(TAG,"Unknown error at address : %x", address);
    }    
  }
  if (nDevices == 0)
   ESP_LOGCONFIG(TAG,"No I2C devices found");
  else
    ESP_LOGCONFIG(TAG, "done Searching \n");
}

float M5StackPBHUBComponent::get_setup_priority() const { return setup_priority::IO; }

void PBHUBGPIOPin::setup() { pin_mode(flags_); }
void PBHUBGPIOPin::pin_mode(gpio::Flags flags) { this->parent_->pin_mode(this->pin_, flags); }
bool PBHUBGPIOPin::digital_read() { return this->parent_->digital_read(this->pin_) != this->inverted_; }
void PBHUBGPIOPin::digital_write(bool value) { this->parent_->digital_write(this->pin_, value != this->inverted_); }
std::string PBHUBGPIOPin::dump_summary() const {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%u via M5Stack PBHUB", pin_);
  return buffer;
}

}  // namespace m5stack_pbhub
}  // namespace esphome
