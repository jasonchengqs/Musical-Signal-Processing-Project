// File name: WP_packet_support
//
// 23 Sep 2016 .. moved get_packet and send_packet into separate file .. KM
//
//	TODO: 	need to properly handle len as a limit

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

#define WP_SOH 0xA5
#define WP_SOH_TRUE 0x00
#define WP_SOH_DATA 0x01
#define WP_SOH_SYNC 0x02

// get_packet state definitions

#define READ_IDLE 1
#define FOUND_SID 2
#define READ_SIZE 3
#define READ_DATA 4

#define uint8_t uint16_t

uint16_t	ReceiveFromRemote(uint8_t *ch);	// read characters from remote
void		SendToRemote(uint8_t value); 	// write characters to remote
void 		mySleep(uint16_t value);		// used if multiple threads present

uint16_t send_packet(uint16_t sid, uint8_t *ptr, uint16_t len);

static uint8_t	*buffer_ptr;
static uint16_t read_state;
static uint16_t read_SID, read_size;
static uint16_t drchar_ctr;

#define SEND_SIZE 252

uint8_t send_buffer[SEND_SIZE+1];
uint8_t *send_ptr=&send_buffer[0];
uint16_t send_ctr=0;

void USART_Send_Flush()
{
	send_packet(3, &send_buffer[0], send_ctr);
	send_ptr = &send_buffer[0];
	send_ctr = 0;
}

void USART_Send_Buffered(uint8_t value)
{
	*send_ptr++ = value;
	send_ctr++;
	if (send_ctr >= SEND_SIZE) {
		USART_Send_Flush();
	} // end packet sent
}

void TX_Put(uint16_t value)
{
	USART_Send_Buffered(value>>8);
	USART_Send_Buffered(value&0x00FF);
}

//*********************************

uint16_t get_packet(uint16_t *sid, uint8_t *packet_buffer, uint16_t len)
{
    uint8_t value, sw;
    
	read_state = READ_IDLE;
	sw = 0;

	while(1) {

		while (ReceiveFromRemote(&value) == 0) mySleep(1);	// wait for an input value

		if (value == WP_SOH) {					// WP_SOH characters are handled separately
			while (ReceiveFromRemote(&value) == 0) mySleep(1);	// get the immediately following character
			switch(value&0x07) {				// have variants on what to do
			case WP_SOH_TRUE:					// have actual WP_SOH	
				read_SID = (value >> 3)&0x1F;
				read_state = FOUND_SID;			// starts the reading of the packet
				break;
			case WP_SOH_DATA:					// have a have a SOH data value
				value = WP_SOH;					// which is a WP_SOH character
				break;
			case WP_SOH_SYNC:					// have a sync .. not presently used at this end
				read_state = READ_IDLE;
				break;
			default:							// have a bogus SOH variant
				printf("WP_SOH problem\n");
				while(1);
				break;
			}
		}

		switch(read_state) {				// a simple state machine
		case READ_IDLE:
			break;
        case FOUND_SID:						// found SOH and SID value
            read_state = READ_SIZE;
            break;
		case READ_SIZE:						// now have the size of data area
			read_size = value;
			drchar_ctr = 0;
			buffer_ptr = packet_buffer;
			read_state = READ_DATA;			// read in the packet
			break;
		case READ_DATA:					// place data values into buffer
			*buffer_ptr++ = value; 			// need to check bounds!!! todo!!
			drchar_ctr++;					// count this character
			if (drchar_ctr == read_size) {	// check if packet is complete
				read_state = READ_IDLE; 	// if so, reset packet support
				sw = 1;
			} // to break out of the loop
            break;
		default:								// should not be possible
			while(1);							// hang here
		} // end of switch

		if (sw != 0) break;						// only happens when data is complete
	}
	*sid = read_SID;
	return drchar_ctr;							// return size of data portion of packet
}

void SendProtectedSOH(uint8_t value)
{	
	SendToRemote(value);
	if (value == WP_SOH) SendToRemote(WP_SOH_DATA);
}

uint16_t send_packet(uint16_t sid, uint8_t *ptr, uint16_t len)
{
	uint16_t utemp, data_ctr;

	SendToRemote(WP_SOH);			// start of header
	SendProtectedSOH((sid<<3)| WP_SOH_TRUE);
	SendProtectedSOH(len);			// amount of data to follow
	for (data_ctr=0; data_ctr<len; data_ctr++) {
		utemp = *ptr++ & 0x00FF;
		SendProtectedSOH(utemp);
	} // end of packet sending
	return len;		// should be data_ctr!!!?
}

// Tells WindowPlot that a this is a program (re)start.

void WindowPlotStart(void)
{
	SendToRemote(WP_SOH);
	SendToRemote(WP_SOH_SYNC);
}
