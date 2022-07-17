
ALTERNATE_CORE = ATTinyCore
ALTERNATE_CORE_PATH = $(HOME)/Library/Arduino15/packages/ATTinyCore/hardware/avr/1.5.2

ISP_PROG = usbtiny
BOARD_TAG = attinyx4
BOARD_SUB = 84
VARIANT = tinyX4_reverse # get the 'proper' pin ordering
BOARD_CLOCK = 8internal
F_CPU = 8000000L

# ATTinyCore's interpolation is not working so we have to set this directly.
# This is the default.
ISP_HIGH_FUSE = 0xDF

# Write to the EEPROM on ispload
# All the music, text and "graphics" are in EEPROM.
ISP_EEPROM  = 1

ARDUINO_LIBS = SevSegShift


include /usr/local/opt/arduino-mk/Arduino.mk
