#include <stdint.h>

class PackedLogEvent;

// this is the recorded data
class LogEvent {
 public:
  uint32_t time;
  uint16_t temperature_indoor;
  uint16_t temperature_outdoor;
  uint8_t door_open;

  PackedLogEvent pack();

  bool operator==(const LogEvent &other) const {
    if(time == other.time &&
       temperature_indoor == other.temperature_indoor &&
       temperature_outdoor == other.temperature_outdoor &&
       door_open == other.door_open)
      return true;
    else
      return false;
  }
 
};

// this is the data actually stored in the EEPROM
// it exploits the fact that we only need
//   32 bits for the time
//   10 bits for the temperature readings
//    1 bit for the door
// total: 53 bits -> 7 bytes
class PackedLogEvent {
 public:
  static const uint64_t TIME_OFFSET                = 0;
  static const uint64_t TEMPERATURE_INDOOR_OFFSET  = 32;
  static const uint64_t TEMPERATURE_OUTDOOR_OFFSET = 42;
  static const uint64_t DOOR_OPEN_OFFSET           = 52;

  static const uint64_t TIME_MASK                = (1 << 32)-1;
  static const uint64_t TEMPERATURE_INDOOR_MASK  = (1 << 10)-1;
  static const uint64_t TEMPERATURE_OUTDOOR_MASK = (1 << 10)-1;
  static const uint64_t DOOR_OPEN_MASK           = (1 << 1)-1;

  uint8_t data[7];


  void pack(const LogEvent* event);
  LogEvent unpack();
};


