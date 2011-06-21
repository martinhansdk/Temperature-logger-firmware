#ifndef LOGEVENT_H
#define LOGEVENT_H

#include <stdint.h>

class PackedLogEvent;

// this is the recorded data
class LogEvent {
 public:
  uint16_t temperature_indoor;
  uint16_t temperature_outdoor;
  uint8_t door_open;

  PackedLogEvent pack();

  bool operator==(const LogEvent &other) const {
    if(temperature_indoor == other.temperature_indoor &&
       temperature_outdoor == other.temperature_outdoor &&
       door_open == other.door_open)
      return true;
    else
      return false;
  }
 
};

// this is the data actually stored in the EEPROM
// it exploits the fact that we only need
//   11 bits for the temperature readings
//    1 bit for the door
// total: 23 bits -> 3 bytes
class PackedLogEvent {
 public:
  static const uint64_t TEMPERATURE_INDOOR_OFFSET  = 0;
  static const uint64_t TEMPERATURE_OUTDOOR_OFFSET = 11;
  static const uint64_t DOOR_OPEN_OFFSET           = 22;

  static const uint64_t TEMPERATURE_INDOOR_MASK  = (1 << 11)-1;
  static const uint64_t TEMPERATURE_OUTDOOR_MASK = (1 << 11)-1;
  static const uint64_t DOOR_OPEN_MASK           = (1 << 1)-1;

  uint8_t data[3];


  void pack(const LogEvent* event);
  LogEvent unpack();
};

#endif
