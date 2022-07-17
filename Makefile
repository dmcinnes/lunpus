
ALTERNATE_CORE = ATTinyCore
ALTERNATE_CORE_PATH = /Users/doug/Library/Arduino15/packages/ATTinyCore/hardware/avr/1.5.2

ISP_PROG = usbtiny
# AVRDUDE_ARD_PROGRAMMER = usbtiny
# AVRDUDE_ARD_BAUDRATE = 19200
BOARD_TAG = attinyx4
BOARD_SUB = 84
VARIANT = tinyX4_reverse # get the 'proper' pin ordering
F_CPU = 8000000L

ISP_HIGH_FUSE = 0xDF
ISP_LOW_FUSE = 0xE2

ARDUINO_LIBS = SevSegShift

ISP_EEPROM  = 1

include /usr/local/opt/arduino-mk/Arduino.mk

# eeprom: 
# 	avrdude -c usbtiny -p attiny84 -U eeprom:w:/var/folders/8w/cgb3rkx15g1bmpf112vwkjp40000gn/T/arduino_build_464809/wumpus.ino.eep:i
#
