#include "logevent.h"

PackedLogEvent LogEvent::pack() { 
  PackedLogEvent pe;
  pe.pack(this);
  return pe;
}

void PackedLogEvent::pack(const LogEvent* event) {
  uint64_t value; // big integer to ease translation

  value = 0;
  value |= (((uint64_t)event->temperature_indoor)  << TEMPERATURE_INDOOR_OFFSET);
  value |= (((uint64_t)event->temperature_outdoor) << TEMPERATURE_OUTDOOR_OFFSET);
  value |= (((uint64_t)event->door_open)           << DOOR_OPEN_OFFSET);

  data[0] = (uint8_t)value;
  data[1] = (uint8_t)(value >> 8);
  data[2] = (uint8_t)(value >> 16);
}

LogEvent PackedLogEvent::unpack() {
  uint64_t value; // big integer to ease translation
  LogEvent event;
  
  value = ((((uint64_t)data[0]) << 0) |
	   (((uint64_t)data[1]) << 8) |
	   (((uint64_t)data[2]) << 16));
  
  event.temperature_indoor  = (value >> TEMPERATURE_INDOOR_OFFSET) & TEMPERATURE_INDOOR_MASK;
  event.temperature_outdoor = (value >> TEMPERATURE_OUTDOOR_OFFSET) & TEMPERATURE_OUTDOOR_MASK;
  event.door_open           = (value >> DOOR_OPEN_OFFSET) & DOOR_OPEN_MASK;

  return event;
}

