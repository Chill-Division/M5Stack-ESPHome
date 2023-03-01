/**
   @copyright (C) 2017 Melexis N.V.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/



#include "MLX90640_I2C_Driver.h"
TwoWire *wire;
void MLX90640_I2CInit(TwoWire *wire_)
{
  wire = wire_ ;
}

//Read a number of words from startAddress. Store into Data array.
//Returns 0 if successful, -1 if error
int MLX90640_I2CRead(uint8_t _deviceAddress, unsigned int startAddress, unsigned int nWordsRead, uint16_t *data)
{

  //Caller passes number of 'unsigned ints to read', increase this to 'bytes to read'
  uint16_t bytesRemaining = nWordsRead * 2;

  //It doesn't look like sequential read works. Do we need to re-issue the address command each time?

  uint16_t dataSpot = 0; //Start at beginning of array

  //Setup a series of chunked I2C_BUFFER_LENGTH byte reads
  while (bytesRemaining > 0)
  {
    wire->beginTransmission(_deviceAddress);
    wire->write(startAddress >> 8); //MSB
    wire->write(startAddress & 0xFF); //LSB
    if (wire->endTransmission(false) != 0) //Do not release bus
    {
      //Serial.println("No ack read");
      return (0); //Sensor did not ACK
    }

    uint16_t numberOfBytesToRead = bytesRemaining;
    if (numberOfBytesToRead > I2C_BUFFER_LENGTH) numberOfBytesToRead = I2C_BUFFER_LENGTH;

    wire->requestFrom((int)_deviceAddress,(int) numberOfBytesToRead);
    if (wire->available())
    {
      for (uint16_t x = 0 ; x < numberOfBytesToRead / 2; x++)
      {
        //Store data into array
        data[dataSpot] = wire->read() << 8; //MSB
        data[dataSpot] |= wire->read(); //LSB

        dataSpot++;
      }
    }

    bytesRemaining -= numberOfBytesToRead;

    startAddress += numberOfBytesToRead / 2;
  }

  return (0); //Success
}

//Set I2C Freq, in kHz
//MLX90640_I2CFreqSet(1000) sets frequency to 1MHz
void MLX90640_I2CFreqSet(int freq)
{
  //i2c.frequency(1000 * freq);
  wire->setClock((long)1000 * freq);
}

//Write two bytes to a two byte address
int MLX90640_I2CWrite(uint8_t _deviceAddress, unsigned int writeAddress, uint16_t data)
{
 wire->beginTransmission((uint8_t)_deviceAddress);
  wire->write(writeAddress >> 8); //MSB
  wire->write(writeAddress & 0xFF); //LSB
  wire->write(data >> 8); //MSB
  wire->write(data & 0xFF); //LSB
  if (wire->endTransmission() != 0)
  {
    //Sensor did not ACK
    //Serial.println("Error: Sensor did not ack");
    return (-1);
  }

  uint16_t dataCheck;
  MLX90640_I2CRead(_deviceAddress, writeAddress, 1, &dataCheck);
  if (dataCheck != data)
  {
    //Serial.println("The write request didn't stick");
    return -2;
  }

  return (0); //Success
}

 bool MLX90640_isConnected(uint8_t addr) {
      wire->beginTransmission((uint8_t)addr);
      if (wire->endTransmission() != 0) return (false);  // Sensor did not ACK.
      return (true);
  }

