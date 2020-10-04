# Makefile for building and flashing an ArduinoMega

# Copyright (C) 2019, 2020 Eric Herman <eric@freesa.org>

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
CLOCKSPEED=8000000L
else
PORT=/dev/ttyACM0
GCC_MMCU=atmega2560
AVRDUDE_MCU=atmega2560
AVRDUDE_STK=stk500v2
AVRDUDE_BAUD=115200
CLOCKSPEED=16000000L
endif

CC=avr-gcc

CFLAGS=-std=gnu11 \
 -Wall -Wextra -Wpedantic -Werror -Wstrict-prototypes \
 -gstabs -Os \
 -funsigned-char \
 -funsigned-bitfields \
 -fpack-struct \
 -fshort-enums \
 -static \
 -mmcu=$(GCC_MMCU) \
 -DF_CPU=$(CLOCKSPEED)

###############
# Make targets:

default: all

reset-arduino:
	stty -F $(PORT) hupcl # e.g. reset the arduino

%.hex : %.bin
	avr-objcopy -O ihex -R .eeprom $< $@

blink.bin: src/blink.c
	$(CC) $(CFLAGS) -o blink.bin src/blink.c

blink: blink.hex

upload-blink: blink.hex reset-arduino
	avrdude -v \
		-c $(AVRDUDE_STK) \
		-p $(AVRDUDE_MCU) \
		-b $(AVRDUDE_BAUD) \
		-P $(PORT) \
		-U flash:w:blink.hex:i

hello-serial.bin: src/hello-serial.c
	$(CC) $(CFLAGS) -o hello-serial.bin src/hello-serial.c

hello-serial: hello-serial.hex

upload-hello-serial: hello-serial.hex reset-arduino
	avrdude -v \
		-c $(AVRDUDE_STK) \
		-p $(AVRDUDE_MCU) \
		-b $(AVRDUDE_BAUD) \
		-P $(PORT) \
		-U flash:w:hello-serial.hex:i
	@echo "now try: screen -l $(PORT)"

all: blink.hex hello-serial.hex
	@echo "now try 'make upload-blink' or 'make upload-hello-serial'"

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0
tidy:
	$(LINDENT) \
		-T size_t \
		-T uint8_t \
		`find src -type f -name '*.h' -o -name '*.c'`

flags:
	@echo $(CC) $(CFLAGS)

clean:
	rm -f *.o *.a *.hex *.bin
