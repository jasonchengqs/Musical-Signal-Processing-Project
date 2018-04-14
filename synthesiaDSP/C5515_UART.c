/*
 * C5515_UART.c
 *
 *  Created on: Feb 10, 2012
 *	03Jun2015 .. updated for RetroPlot use
 *	02Oct2016 .. updated for fall 2016 use
 *
 *      Author: Kurt Metzger
 */

#include <stdint.h>
#include "..\C5505_support\C5505.h"

void InitUART(unsigned int baud_divisor)
{
	PWREMU_MGMT = 0x6001;  // enable rcvr, xmtr and free run
	IER = 0x0000;  // disable UART interrupts
	FCR = 0x0000;  // disable FIFOs
	LCR = 0x0007;  // 8-bit characters, 2 stop bits
	MCR = 0x0020;
	DLL = baud_divisor;       // divisor low 8 bits
	DLH = baud_divisor >> 8;  // divisor high 8 bits
}

unsigned int UART_rcv()
{
	while ((LSR & 0x0001)== 0);   // wait until character has been received
	return RBR;                   // read and return character
}

void UART_xmit(unsigned int value)
{
	//while((LSR&0x0020) == 0);    // wait until it has gone
	THR = value&0x00FF;          // transmit a character
	while((LSR&0x0020) == 0);  // wait until it has gone
	//while(1);
}

//***********************************************************

#define uint8_t uint16_t // needed for C55xx only !!!!!!

uint16_t	ReceiveFromRemote(uint8_t *ch)
{
	*ch = UART_rcv();
	return 1;
}


void		SendToRemote(uint8_t value)
{
	UART_xmit(value);
}


void 		mySleep(uint16_t value){}		// used if multiple threads present
