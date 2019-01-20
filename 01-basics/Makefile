# Makefile for building and flashing an ArduinoMega

# Copyright (C) 2019 Eric Herman <eric@freesa.org>

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.


# port the arduino is connected to
# and CPU type as defined by gcc and AVR-DUDE
ifeq ($(wildcard /dev/ttyACM0),)
PORT=/dev/ttyUSB0
GCC_MMCU=atmega1280
AVRDUDE_MCU=m1280
AVRDUDE_STK=stk500v1
AVRDUDE_BAUD=57600
CLOCKSPEED=8000000
else
PORT=/dev/ttyACM0
GCC_MMCU=atmega2560
AVRDUDE_MCU=atmega2560
AVRDUDE_STK=stk500v2
AVRDUDE_BAUD=115200
CLOCKSPEED=16000000
endif

CC=avr-gcc

CFLAGS=-std=gnu11 \
 -Wall -Wextra -Wpedantic -Werror -Wstrict-prototypes \
 -gstabs -Os \
 -funsigned-char \
 -funsigned-bitfields \
 -fpack-struct \
 -fshort-enums \
 -mmcu=$(GCC_MMCU) \
 -DF_CPU=$(CLOCKSPEED)

###############
# Make targets:

default: all

reset-arduino:
	stty -F $(PORT) hupcl # e.g. reset the arduino

blink.bin: blink.c
	$(CC) $(CFLAGS) -static -o blink.bin blink.c

# %.hex : %.bin
#	#avr-objcopy -O ihex -R .eeprom $< $@

blink.hex: blink.bin
	avr-objcopy -O ihex -R .eeprom blink.bin blink.hex

all: blink.hex
	echo YAY

upload-blink: blink.hex reset-arduino
	avrdude -v \
		-c $(AVRDUDE_STK) \
		-p $(AVRDUDE_MCU) \
		-b $(AVRDUDE_BAUD) \
		-P $(PORT) \
		-U flash:w:blink.hex:i

clean:
	rm -f *.o *.a *.hex *.bin
