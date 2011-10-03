#include "eeprom.h"
#include <Wire.h>
#include <TimeAlarms.h>

//----------------- low level functions for talking to the AT24C256 eeproms
void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, uint8_t data ) {
  int rdata = data;
  Wire.beginTransmission(deviceaddress);
  Wire.send((int)(eeaddress >> 8)); // MSB
  Wire.send((int)(eeaddress & 0xFF)); // LSB
  Wire.send(rdata);
  Wire.endTransmission();
  Alarm.delay(11); // give the eeprom time to finish one write before issuing another one. Value was obtained from the datasheet.
  
}

byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress ) {
  uint8_t rdata = 0xFF;
  Wire.beginTransmission(deviceaddress);
  Wire.send((int)(eeaddress >> 8)); // MSB
  Wire.send((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,1);
  if (Wire.available()) rdata = Wire.receive();
  return rdata;
}


// The read and write handlers for the database
void EEwriter(unsigned long address, uint8_t data)
{
  int deviceaddress;
  
  deviceaddress=0x50 + (address/32768);

  if(deviceaddress > 0x50) deviceaddress++; // FIXME, workaround eeprom with address 0x51 doesn't work

  address=address % 32768;
        
  i2c_eeprom_write_byte( deviceaddress, address, data );
}

uint8_t EEreader(unsigned long address)
{
  int deviceaddress;
      
  deviceaddress=0x50 + (address/32768);

  if(deviceaddress > 0x50) deviceaddress++; // FIXME, workaround eeprom with address 0x51 doesn't work

  address=address % 32768;
      
  return i2c_eeprom_read_byte( deviceaddress, address );
}






