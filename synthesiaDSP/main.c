
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include "PlotSupport.h"
#include "WindowPlot.h"
#include "hannWin.h"

#define uint8_t uint16_t // C5515 hardware does not support uint8_t

void cfft32_NOSCALE(int32_t *, uint16_t);
void cbrev32(int32_t *, int32_t *, uint16_t);
void SendToRemote(uint16_t value);
void C55_setup();

uint16_t ADC_Fifo_Init(void);
uint16_t ADC_Fifo_Get(int32_t *);


static uint16_t ctr;
static int32_t ADC_input;

#define N_TONE 48
#define PI 3.1415926536
#define fs 48000
#define NS 1000
#define FFT_SIZE 2048
#define NUM_HARMONICS 13

#pragma DATA_ALIGN(FFT_L_array, 8192); 	// can do upto 8192 points
static int32_t FFT_L_array[2*FFT_SIZE];
//#pragma DATA_ALIGN(FFT_R_array, 8192); 	// can do upto 8192 points
//static int32_t FFT_R_array[2*FFT_SIZE];

int main()
{


    C55_setup();    		// start what this system needs to be started
	_enable_interrupts();	// enable C5515 interrupt system

	ADC_Fifo_Init();

	int32_t *rptr, *lptr, new, old;

	uint8_t STARTBIT = 0xFF;
	uint8_t WAITBIT = 0xFE;
	uint16_t Lmag, Rmag;


	while (1) {
		// receive ADC input
		ADC_Fifo_Init();

		while(ADC_Fifo_Get(&old)==0){
			SendToRemote(WAITBIT);
		}
		while(1) {			// if not equal to 0 trying to start on a carrier zero crossing!
			while(ADC_Fifo_Get(&new)==0);
			if ((new>=0) && (old<0)) break;
			old = new;
		}


		SendToRemote(STARTBIT); 		// start bit is 0xff
		//printf("Packet start\n");
		lptr = &FFT_L_array[0];
//		rptr = &FFT_R_array[0];
		for (ctr=0; ctr<FFT_SIZE; ctr++) {
			while(ADC_Fifo_Get(&ADC_input)==0);
			*lptr++ = ((int32_t)ADC_input>>16 * (int32_t)WIN[ctr]) >> 16;	// 16 bit sample from left channel
			*lptr++ = 0 ; 			//imaginary part is 0
//			*rptr++ = ADC_input&(0xFFFF);	// 16 bit sample from left channel
//			*rptr++ = 0 ; 			//imaginary part is 0
		}


		// calculate FFT
		cfft32_NOSCALE(FFT_L_array, FFT_SIZE);
		cbrev32(FFT_L_array, FFT_L_array, FFT_SIZE);  	// put into normal order
//		cfft32_NOSCALE(FFT_R_array, FFT_SIZE);
//		cbrev32(FFT_R_array, FFT_R_array, FFT_SIZE);  	// put into normal order

		for (ctr = 0; ctr<FFT_SIZE/2; ctr++) {
			FFT_L_array[2*ctr] = FFT_L_array[2*ctr]>>14;		// scale real terms
			FFT_L_array[2*ctr+1] = FFT_L_array[2*ctr+1]>>14;	// scale imaginary terms
			FFT_L_array[ctr] = sqrt(pow(FFT_L_array[2*ctr],2) + pow(FFT_L_array[2*ctr+1],2));
			Lmag = FFT_L_array[ctr];
			if(Lmag > 253) Lmag = 253;
			SendToRemote(Lmag);
			/*
			FFT_R_array[2*ctr] = FFT_R_array[2*ctr]>>14;		// scale real terms
			FFT_R_array[2*ctr+1] = FFT_R_array[2*ctr+1]>>14;	// scale imaginary terms
			FFT_R_array[ctr] = sqrt(pow(FFT_R_array[2*ctr],2) + pow(FFT_R_array[2*ctr+1],2));
			Rmag = FFT_R_array[ctr];
			//printf("%d, %d\n", Lmag, Rmag);
			if(Rmag > 253) Rmag = 253;
			SendToRemote(Rmag);
			*/
		}
		//printf("Packet sent\n");
	}
}
