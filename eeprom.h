#ifndef EEPROM_H
#define EEPROM_H
#include <stdint.h>
#include "WProgram.h"

// The read and write handlers for the database
extern void EEwriter(unsigned long address, uint8_t data);
extern byte EEreader(unsigned long address);

#endif
