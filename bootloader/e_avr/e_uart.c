/*
 * UART driver code.
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "uart.h"

/* init UART1
 * BAUD must be set and setbaud imported before calling this
 */
#define UART1_TX "/tmp/uart1_tx"
#define UART1_RX "/tmp/uart1_rx"

int UART1_tx;
int UART1_rx;
int UART1_rc;

void UART1_init(void)
{
	unlink(UART1_TX);
	int result = mkfifo (UART1_TX, S_IRUSR | S_IWUSR);
	UART1_tx = open(UART1_TX, O_WRONLY); 
	
	unlink(UART1_RX);
	result = mkfifo (UART1_RX, S_IRUSR | S_IWUSR);
	UART1_rx = open(UART1_RX, O_RDONLY);
}	

void UART1_putchar(unsigned char data)
{
	while (write(UART1_tx, &data, 1) != 1){}
}

bool UART1_data_available(void)
{
	int count;
	ioctl(UART1_rx, FIONREAD, &count);

	return count;
}

unsigned char UART1_getchar(void)
{
	char c;
    while ( (UART1_rc = read(UART1_rx,&c,sizeof(c))) == 0) {}

    return c;
}

void UART1_flush(void)
{

}

void UART1_putstring(char* str)
{
    int i = 0;
    while(str[i] != 0){
        UART1_putchar(str[i]);
        i += 1;
    }
    UART1_putchar((unsigned char)0);  // make sure we send out the null terminator
}


/* init UART0
 * BAUD must be set and setbaud imported before calling this
 */
#define UART0_TX "/tmp/uart0_tx"
#define UART0_RX "/tmp/uart0_rx"

int UART0_tx;
int UART0_rx;
int UART0_rc;

void UART0_init(void)
{
	unlink(UART0_TX);
	int result = mkfifo (UART0_TX, S_IRUSR | S_IWUSR);
	UART0_tx = open(UART0_TX, O_WRONLY); 
	
	unlink(UART0_RX);
	result = mkfifo (UART0_RX, S_IRUSR | S_IWUSR);
	UART0_rx = open(UART0_RX, O_RDONLY);
}	

void UART0_putchar(unsigned char data)
{
	while (write(UART0_tx, &data, 1) != 1){}
}

bool UART0_data_available(void)
{
	int count;
	ioctl(UART0_rx, FIONREAD, &count);

	return count;
}

unsigned char UART0_getchar(void)
{
	char c;
    while ( (UART0_rc = read(UART0_rx,&c,sizeof(c))) == 0) {}

    return c;
}

void UART0_flush(void)
{

}

void UART0_putstring(char* str)
{
    int i = 0;
    while(str[i] != 0){
        UART0_putchar(str[i]);
        i += 1;
    }
    UART0_putchar((unsigned char)0);  // make sure we send out the null terminator
}