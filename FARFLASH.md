# AVR Far Flash Crash Course
The AVR 1284p has 128K of flash. Since the AVR architecture was designed with a
16-bit wide address bus for flash, accessing flash above address 64K takes some
acrobatics. 

Since the bootloader section is at the END of flash, this affects a lot of your
code.  Many libraries may have been tested for AVR devices with less than 64K
or were intended to be loaded in the lower addresses of flash -- incorporating
those libraries into your bootloader now places it in far flash and causes
things to break.  What breaks exactly?  Any pre-initialized arrays of data 
declared in the following ways will probably NOT work:

```c
const uint8_t this_is_a_key[] = {0x01, 0x02, 0x03, 0x04};
/* OR */
uint8_t this_is_a_key_also[] = {0x01, 0x02, 0x03, 0x04};
/* OR */
const char this_is_a_release_message[] = "ABCDE";
```
NONE of the data in these arrays/strings is intitialized. You are able to build
the code above, however the data in the arrays/strings will be random since it
is uninitialized. If, for example you run `UART1_putchar(this_is_a_release_message[1])` 
the character 'B' will not be printed to the console, instead you will either 
get a random character or a 0. 

## Solution
There is a workaround to this issue. The solution invlolves placing the data 
in program memory (instead of RAM) by using the `PROGMEM` compiler directive.  
In order to access the `PROGMEM` directive, you must include 
`#include <avr/pgmspace.h>` at the top of your file. 
The code above with this change is shown below. 

```c
const uint8_t this_is_a_key[] PROGMEM = {0x01, 0x02, 0x03, 0x04};
/* OR */

// NOTE: you must always have the const keyword when placing data in PROGMEM. 
const uint8_t this_is_a_key_also[] PROGMEM = {0x01, 0x02, 0x03, 0x04};
/* OR */

const char this_is_a_release_message[] PROGMEM = "ABCDE";
```

Since the following data is no longer stored in SRAM but instead in flash, 
you now need to take an additional step to access the data. There are TWO 
methods for accessing the data as shown below. 

### Method 1 - Single byte access

If you need to access a single byte at a time of your data stored in progmem 
(as you may need to do with AES s-boxes), you can use the code below.

```c
/* INSTEAD OF */
uint8_t data = this_is_a_key[2];

/* USE THIS */
uint8_t data = pgm_get_pgm_read_byte_far(pgm_get_far_address(this_is_a_key)+2);


/* INSTEAD OF */
uint8_t data = this_is_a_key[1];

/* USE THIS */
uint8_t data = pgm_get_pgm_read_byte_far(pgm_get_far_address(this_is_a_key)+1);


/* INSTEAD OF */
uint8_t data = this_is_a_release_message[1];

/* USE THIS */
uint8_t data = pgm_read_byte_far(pgm_get_far_address(this_is_a_release_message)+1);
```

### Method 2 - Full buffer access

In the event you want the entire chunk of data, instead of a single byte at a 
time you can create a local buffer in RAM and use the `memcpy_PF` function to
read the data out of flash and into your local buffer. Examples of this method 
can be seen below. 

```c
const uint8_t data_in_flash[4] PROGMEM = {0x01, 0x02, 0x03, 0x04};

uint8_t data_in_ram[4];

memcpy_PF(data_in_ram, pgm_get_far_address(data_in_flash), 4);


/* SO NOW: */
// data_in_ram[0] = 0x01;
// data_in_ram[1] = 0x02;
// data_in_ram[2] = 0x03;
// data_in_ram[3] = 0x04;
```

## References for Further Reading
* http://www.nongnu.org/avr-libc/user-manual/group__avr__pgmspace.html
* http://www.fourwalledcubicle.com/AVRArticles.php
