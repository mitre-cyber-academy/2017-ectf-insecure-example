# Hardware configuration settings.
MCU = atmega1284p
F_CPU = 20000000
BAUD = 115200

# Secret password default value.
PASSWORD ?= password

# Tool aliases.
CC = avr-gcc
STRIP  = avr-strip
OBJCOPY = avr-objcopy
PROGRAMMER = dragon_jtag

# Compiler configurations.
# Description of CDEFS options
# -g3 -- turns on  the highest level of debug symbols.
# -ggdb3 -- turns on the highest level of debug symbols for the gdb debugger.
#
#  NOTE: The debug options shoud only affect the .elf file. Any debug symbols are stripped 
#  from the .hex file so no debug info is actually loaded on the AVR. This means that removing 
#  debug symbols should not affect the size of the firmware.
CDEFS = -g3 -ggdb3 -mmcu=${MCU} -DF_CPU=${F_CPU} -DBAUD=${BAUD} -DRB_PASSWORD=\"${PASSWORD}\"

# Description of CLINKER options:
# 	-Wl,--section-start=.text=0x1E000 -- Offsets the code to the start of the bootloader section
# 	-Wl,-Map,bootloader.map -- Created an additional file that lists the locations in memory of all functions.
CLINKER = -nostartfiles -Wl,--section-start=.text=0x1E000 -Wl,-Map,bootloader.map

CWARN =  -Wall
COPT = -std=gnu99 -Os -fno-tree-scev-cprop -mcall-prologues \
       -fno-inline-small-functions -fsigned-char

CFLAGS  = $(CDEFS) $(CLINKER) $(CWARN) $(COPT)

# Include file paths.
INCLUDES = -I./include

# Run clean even when all files have been removed.
.PHONY: clean

all:    flash.hex eeprom.hex
	@echo  Simple bootloader has been compiled and packaged as intel hex.

pin.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c src/pin.c

flash.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c src/flash.c

uart.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c src/uart.c

sys_startup.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c src/sys_startup.c

bootloader.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c src/bootloader.c

bootloader_dbg.elf: pin.o flash.o uart.o sys_startup.o bootloader.o
        # Create an .elf file for the bootloader with all debug symbols included.
	$(CC) $(CFLAGS) $(INCLUDES) -o bootloader_dbg.elf pin.o flash.o uart.o sys_startup.o bootloader.o

strip: bootloader_dbg.elf
	# Create a version of the bootloder .elf file with all the debug symbols stripped.
	$(STRIP) bootloader_dbg.elf -o bootloader.elf

flash.hex: strip
	$(OBJCOPY) -R .eeprom -O ihex bootloader.elf flash.hex
	avr-size flash.hex

eeprom.hex: strip
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex bootloader.elf eeprom.hex

flash: flash.hex eeprom.hex
	 avrdude -P usb -p m1284p -c $(PROGRAMMER)  -u -U flash:w:flash.hex:i \
						    -U eeprom:w:eeprom.hex:i \
						    -U lfuse:w:lfuse.hex:i \
						    -U hfuse:w:hfuse.hex:i \
						    -U efuse:w:efuse.hex:i

debug: flash.hex eeprom.hex
	# Launch avarice: a tool that creates a debug server for the AVR and Dragon
	avarice -R -g :4242 &
	# Launch the avr debugger avr-gdb. The configuation for this tool is included 
	# in .gdbinit
	avr-gdb

clean:
	$(RM) -v *.hex *.o *.elf $(MAIN)

