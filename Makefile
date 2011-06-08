ARDUINO_DIR = /usr/share/arduino
AVR_TOOLS_PATH = /usr/bin
AVRDUDE_CONF   = /etc/avrdude.conf

TARGET       = logger
MCU          = atmega168
F_CPU        = 16000000
ARDUINO_PORT = /dev/ttyUSB0

ARDUINO_LIBS = Time TimeAlarms Wire EDB Messenger

include ../Arduino.mk
