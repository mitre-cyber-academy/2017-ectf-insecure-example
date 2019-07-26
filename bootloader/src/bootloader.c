/*
 * bootloader.c
 *
 * If Port B Pin 2 (PB2 on the protostack board) is pulled to ground the 
 * bootloader will wait for data to appear on UART1 (which will be interpretted
 * as an updated firmware package).
 * 
 * If the PB2 pin is NOT pulled to ground, but 
 * Port B Pin 3 (PB3 on the protostack board) is pulled to ground, then the 
 * bootloader will enter flash memory readback mode. 
 * 
 * If NEITHER of these pins are pulled to ground, then the bootloader will 
 * execute the application from flash.
 *
 * If data is sent on UART for an update, the bootloader will expect that data 
 * to be sent in frames. A frame consists of two sections:
 * 1. Two bytes for the length of the data section
 * 2. A data section of length defined in the length section
 *
 * [ 0x02 ]  [ variable ]
 * ----------------------
 * |  Length |  Data... |
 *
 * Frames are stored in an intermediate buffer until a complete page has been
 * sent, at which point the page is written to flash. See program_flash() for
 * information on the process of programming the flash memory. Note that if no
 * frame is received after 2 seconds, the bootloader will time out and reset.
 *
 */

#ifdef PC
#define EEMEM
#include "e_eeprom.h"
#include "e_io.h"
#include "e_wdt.h"
#include "e_pgmspace.h"
#else
#include <avr/io.h>
#include <avr/boot.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#endif

#include "pin.h"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "uart.h"
#include "flash.h"

#define OK    ((unsigned char)0x00)
#define ERROR ((unsigned char)0x01)

void load_firmware(void);
void boot_firmware(void);
void readback(void);

uint16_t fw_size EEMEM = 0;
uint16_t fw_version EEMEM = 0;

int main(void)
{
    // Init UART1 (virtual com port)
    UART1_init();

    UART0_init();
    wdt_reset();

    PIN_init();

    // If jumper is present on pin 2, load new firmware.
    if(!PIN_update())
    {
        UART1_putchar('U');
        load_firmware();
    }
    else if(!PIN_readback())
    {
        UART1_putchar('R');
        readback();
    }
    else
    {
        UART1_putchar('B');
        boot_firmware();
    }
} // main

/*
 * Interface with host readback tool.
 */
void readback(void)
{
    // Start the Watchdog Timer
    wdt_enable(WDTO_2S);

    // Read in start address (4 bytes).
    uint32_t start_addr = ((uint32_t)UART1_getchar()) << 24;
    start_addr |= ((uint32_t)UART1_getchar()) << 16;
    start_addr |= ((uint32_t)UART1_getchar()) << 8;
    start_addr |= ((uint32_t)UART1_getchar());

    wdt_reset();

    // Read in size (4 bytes).
    uint32_t size = ((uint32_t)UART1_getchar()) << 24;
    size |= ((uint32_t)UART1_getchar()) << 16;
    size |= ((uint32_t)UART1_getchar()) << 8;
    size |= ((uint32_t)UART1_getchar());

    wdt_reset();

    // Read the memory out to UART1.
    for(uint32_t addr = start_addr; addr < start_addr + size; ++addr)
    {
        // Read a byte from flash.
        unsigned char byte = pgm_read_byte_far(addr);
        wdt_reset();

        // Write the byte to UART1.
        UART1_putchar(byte);
        wdt_reset();
    }

    while(1) __asm__ __volatile__(""); // Wait for watchdog timer to reset.
}


/*
 * Load the firmware into flash.
 */
void load_firmware(void)
{
    int frame_length = 0;
    unsigned char rcv = 0;
    unsigned char data[SPM_PAGESIZE]; // SPM_PAGESIZE is the size of a page.
    unsigned int data_index = 0;
    unsigned int page = 0;
    uint16_t version = 0;
    uint16_t size = 0;

    // Start the Watchdog Timer
    wdt_enable(WDTO_2S);

    /* Wait for data */
    while(!UART1_data_available())
    {
        __asm__ __volatile__("");
    }

    // Get version.
    rcv = UART1_getchar();
    version = (uint16_t)rcv << 8;
    rcv = UART1_getchar();
    version |= (uint16_t)rcv;

    // Get size.
    rcv = UART1_getchar();
    size = (uint16_t)rcv << 8;
    rcv = UART1_getchar();
    size |= (uint16_t)rcv;

    // Compare to old version and abort if older (note special case for version
    // 0).
    if (version != 0 && version < eeprom_read_word(&fw_version))
    {
        UART1_putchar(ERROR); // Reject the metadata.
        // Wait for watchdog timer to reset.
        while(1)
        {
            __asm__ __volatile__("");
        }
    }
    else if(version != 0)
    {
        // Update version number in EEPROM.
        wdt_reset();
        eeprom_update_word(&fw_version, version);
    }

    // Write new firmware size to EEPROM.
    wdt_reset();
    eeprom_update_word(&fw_size, size);
    wdt_reset();

    UART1_putchar(OK); // Acknowledge the metadata.

    /* Loop here until you can get all your characters and stuff */
    while (1)
    {
        wdt_reset();

        // Get two bytes for the length.
        rcv = UART1_getchar();
        frame_length = (int)rcv << 8;
        rcv = UART1_getchar();
        frame_length += (int)rcv;

        UART0_putchar((unsigned char)rcv);
        wdt_reset();

        // Get the number of bytes specified
        for(int i = 0; i < frame_length; ++i){
            wdt_reset();
            data[data_index] = UART1_getchar();
            data_index += 1;
        } //for

        // If we filed our page buffer, program it
        if(data_index == SPM_PAGESIZE || frame_length == 0)
        {
            wdt_reset();
            program_flash(page, data);
            page += SPM_PAGESIZE;
            data_index = 0;
#if 1
            // Write debugging messages to UART0.
            UART0_putchar('P');
            UART0_putchar(page>>8);
            UART0_putchar(page);
#endif
            wdt_reset();

        } // if

        UART1_putchar(OK); // Acknowledge the frame.
    } // while(1)
}


/*
 * Ensure the firmware is loaded correctly and boot it up.
 */
void boot_firmware(void)
{
    // Start the Watchdog Timer.
    wdt_enable(WDTO_2S);

    // Write out the release message.
    uint8_t cur_byte;
    uint32_t addr = (uint32_t)eeprom_read_word(&fw_size);

    // Reset if firmware size is 0 (indicates no firmware is loaded).
    if(addr == 0)
    {
        // Wait for watchdog timer to reset.
        while(1) __asm__ __volatile__("");
    }

    wdt_reset();

    // Write out release message to UART0.
    do
    {
        cur_byte = pgm_read_byte_far(addr);
        UART0_putchar(cur_byte);
        ++addr;
    } while (cur_byte != 0);

    // Stop the Watchdog Timer.
    wdt_reset();
    wdt_disable();

    /* Make the leap of faith. */
    asm ("jmp 0000");
}


