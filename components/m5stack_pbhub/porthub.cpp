#include "porthub.h"

PortHub::PortHub() {
}

PortHub::PortHub(uint8_t iic_addr, TwoWire *wire_) {
    _iic_addr = iic_addr;
    this->wire = wire_ ;
}

void PortHub::begin() {
    Wire.begin();
}

uint16_t PortHub::hub_a_read_value(uint8_t reg) {
    this->wire->beginTransmission(_iic_addr);
    this->wire->write(reg | 0x06);
    this->wire->endTransmission();

    uint8_t RegValue_L, RegValue_H;

    this->wire->requestFrom((int)_iic_addr, (int)2);
    while (this->wire->available()) {
        RegValue_L = this->wire->read();
        RegValue_H = this->wire->read();
    }

    return (RegValue_H << 8) | RegValue_L;
}

uint8_t PortHub::hub_d_read_value_A(uint8_t reg) {
    this->wire->beginTransmission(_iic_addr);
    this->wire->write(reg | 0x04);
    this->wire->endTransmission();

    uint8_t RegValue;

    this->wire->requestFrom((int)_iic_addr, (int)1);
    while (this->wire->available()) {
        RegValue = this->wire->read();
    }
    return RegValue;
}

uint8_t PortHub::hub_d_read_value_B(uint8_t reg) {
    this->wire->beginTransmission(_iic_addr);
    this->wire->write(reg | 0x05);
    this->wire->endTransmission();

    uint8_t RegValue;

    this->wire->requestFrom((int)_iic_addr, (int)1);
    while (this->wire->available()) {
        RegValue = this->wire->read();
    }
    return RegValue;
}

void PortHub::hub_d_wire_value_A(uint8_t reg, uint16_t level) {
    this->wire->beginTransmission(_iic_addr);
    this->wire->write(reg | 0x00);
    this->wire->write(level & 0xff);
    this->wire->endTransmission();
}

void PortHub::hub_d_wire_value_B(uint8_t reg, uint16_t level) {
    this->wire->beginTransmission(_iic_addr);
    this->wire->write(reg | 0x01);
    this->wire->write(level & 0xff);
    this->wire->endTransmission();
}

void PortHub::hub_a_wire_value_A(uint8_t reg, uint16_t duty) {
    this->wire->beginTransmission(_iic_addr);
    this->wire->write(reg | 0x02);
    this->wire->write(duty & 0xff);
    this->wire->endTransmission();
}

void PortHub::hub_a_wire_value_B(uint8_t reg, uint16_t duty) {
    this->wire->beginTransmission(_iic_addr);
    this->wire->write(reg | 0x03);
    this->wire->write(duty & 0xff);
    this->wire->endTransmission();
}

void PortHub::hub_wire_length(uint8_t reg, uint16_t length) {
    this->wire->beginTransmission(_iic_addr);
    this->wire->write(reg | 0x08);
    this->wire->write(length & 0xff);
    this->wire->write(length >> 8);
    this->wire->endTransmission();
}

void PortHub::hub_wire_index_color(uint8_t reg, uint16_t num, uint8_t r,
                                   int8_t g, uint8_t b) {
    this->wire->beginTransmission(_iic_addr);
    this->wire->write(reg | 0x09);
    this->wire->write(num & 0xff);
    this->wire->write(num >> 8);
    this->wire->write(r);
    this->wire->write(g);
    this->wire->write(b);
    this->wire->endTransmission();
}

void PortHub::hub_wire_fill_color(uint8_t reg, uint16_t first, uint16_t count,
                                  uint8_t r, int8_t g, uint8_t b) {
    this->wire->beginTransmission(_iic_addr);
    this->wire->write(reg | 0x0a);
    this->wire->write(first & 0xff);
    this->wire->write(first >> 8);

    this->wire->write(count & 0xff);
    this->wire->write(count >> 8);

    this->wire->write(r);
    this->wire->write(g);
    this->wire->write(b);
    this->wire->endTransmission();
}

void PortHub::hub_wire_setBrightness(uint8_t reg, uint8_t brightness) {
    this->wire->beginTransmission(_iic_addr);
    this->wire->write(reg | 0x0b);
    this->wire->write(brightness & 0xff);
    this->wire->endTransmission();
}
