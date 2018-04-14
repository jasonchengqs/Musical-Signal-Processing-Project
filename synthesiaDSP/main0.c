/*
// File name: OverlapAdd.c
//
// 27 Sep 2016 .. coding started .. KM
// 20 Oct 2016 .. demonstrating use of the whatever output stream

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include "WindowPlot.h"
#include "PlotSupport.h"
#include "hannWin.h"

#define uint8_t uint16_t // C5515 hardware does not support uint8_t

#define XS  PLT_X_SIZE
#define YS  PLT_Y_SIZE

void cfft32_NOSCALE(int32_t *, uint16_t);
void cbrev32(int32_t *, int32_t *, uint16_t);

// 	ADC FIFO call definitions

uint16_t ADC_Fifo_Init(void);
uint16_t ADC_Fifo_Get(int32_t *);
//uint16_t ADC_Fifo_Put(int32_t);
uint16_t ADC_Fifo_Size(void);

// DAC FIFO call definitions

uint16_t DAC_Fifo_Init(void);
//uint16_t DAC_Fifo_Get(int32_t *);
uint16_t DAC_Fifo_Put(int32_t);
uint16_t DAC_Fifo_Size(void);

void C55_setup();		// initialize the C5515 clocks, etc.
void UART_xmit(uint16_t value);
uint16_t send_packet(uint16_t sid, uint8_t *ptr, uint16_t len);

static uint16_t test;
static uint16_t ctr;

static int32_t ADC_input;
static int32_t r;

#define N_TONE 48
#define PI 3.1415926536
#define fs 48000
#define NS 1000
#define FFT_SIZE 1024
#define NUM_HARMONICS 13
#define GPIO_OUT_0	*(volatile ioport uint16_t *)(0x1c0a) 	// GPIO out register 0
#define GPIO_OUT_1  *(volatile ioport uint16_t *)(0x1c0b)	// GPIO out register 1
int16_t tone[N_TONE], modulated;
int16_t tone_ctr;

//#pragma DATA_ALIGN(signal, 4*FFT_SIZE); // size of complex fft
#pragma DATA_ALIGN(signal, 4096); 	// can do upto 8192 points
static int32_t signal[FFT_SIZE];
#pragma DATA_ALIGN(FFT_array, 4096); 	// can do upto 8192 points
static int32_t FFT_array[2*FFT_SIZE];

float sinv[NS];
float cosv[NS];

void 		mySleep(uint16_t value);
uint16_t 	ReceiveFromRemote(uint16_t *value);
void 		SendToRemote(uint16_t value);
void		USART_Send_Flush(void);

extern int16_t volatile sw_ADC_DDS, sw_DDS_INTERP;
char buffer[128];

int f2bin(int f){
	// *2 to account for imaginary bins
	return (int)((double)f * (2.0*(double)FFT_SIZE/(double)fs));
}

/*
 * Computes the discrete Fourier transform (DFT) of the given vector.
 * All the array arguments must have the same length.
 */
/*
double compute_dft(int32_t sig[], uint16_t freq) {
	double k = (double)freq/(double)fs;
	double sumreal = 0;
	double sumimag = 0;
	int n;
	double angle;


	for (n = 0; n < FFT_SIZE; n++) {  /* For each input element */
/*		angle = 2 * PI * n * k;
		sumreal +=  sig[n] * cos(angle);
		sumimag += -sig[n] * sin(angle);
	}
	double out = (double)sqrt(sumreal*sumreal+sumimag*sumimag);
	//out = sqrt(pow(sumreal,2)+pow(sumimag,2));
	return out;
}

// Return fundamental value(s)
uint16_t Praat(int32_t sig[]){

	uint16_t fundamental = 1000;
	return fundamental;
}

void send16bits(uint16_t data){
	//data = (int16_t) data;
	//SendToRemote(data >> 8);
	//SendToRemote(data & 0x00FF);
	uint16_t m;
	m = 2000;
	SendToRemote(data >> 8);
	SendToRemote(data & 0x00FF);
	//SendToRemote(dat >> 8);
	//SendToRemote(dat & 0x00FF);
}

int main()
{
	uint16_t len;
	uint32_t counter=0;

    C55_setup();    		// start what this system needs to be started
	_enable_interrupts();	// enable C5515 interrupt system

	DAC_Fifo_Init();
	ADC_Fifo_Init();

	// demonstrates simple display of FFT generated spectrum

	int32_t real, imag, *sigptr, *ptr, new, old;

	uint8_t STARTBIT = 0xFF;
	double scalar;
	double amp;
	uint8_t amp8;
	uint16_t fundamental;
	int fund_amp;
	int fbin;
	int famp;
	int num_notes;
	int i;

	while (1) {
		// receive ADC input
		ADC_Fifo_Init();	// reset prior to each display capture
		while(ADC_Fifo_Get(&old)==0);
		while(1) {			// if not equal to 0 trying to start on a carrier zero crossing!
			while(ADC_Fifo_Get(&new)==0);
			if ((new>=0) && (old<0)) break;
			old = new;
		}
		SendToRemote(STARTBIT); 		// start bit is 0xff
		ptr = &FFT_array[0];
		sigptr = &signal[0];
		for (ctr=0; ctr<FFT_SIZE; ctr++) {
			while(ADC_Fifo_Get(&ADC_input)==0);
				//hanning window
				*sigptr++ = ((int32_t)(ADC_input>>16) * (int32_t)WIN[ctr]) >> 16;


			send16bits(ADC_input >> 16);
			//printf("%d\n",ADC_input);
		}
			// FFT
			//FFT_array = signal;
			//cfft32_NOSCALE(FFT_array, FFT_SIZE);      // form the DFT
			//cbrev32(FFT_array, FFT_array, FFT_SIZE);  // put into normal order
			//send16bits(FFT_array);

//			*sigptr++ = ADC_input>>16;
//			*ptr++ = ADC_input>>16;	// 16 bit sample from left channel
//			*ptr++ = 0 ; 			//imaginary part is 0
		}



		/*
		// calculate FFT
		cfft32_NOSCALE(FFT_array, FFT_SIZE);
		cbrev32(FFT_array, FFT_array, FFT_SIZE);  	// put into normal order
		for (ctr = 0; ctr<FFT_SIZE; ctr++) {
			FFT_array[2*ctr] = FFT_array[2*ctr]>>14;		// scale real terms
			FFT_array[2*ctr+1] = FFT_array[2*ctr+1]>>14;	// scale imaginary terms
		}


		// begin sending to Pi
		SendToRemote(STARTBIT); 		// start bit is 0xff
		fundamental = Praat(signal);
		num_notes = 1;
		//SendToRemote(num_notes); // only implement after changing receiving code
		for(i = 0; i < num_notes; i++){
			//put all code below in this loop, but 'fundamental' will be an array
		}
		SendToRemote(fundamental >> 8);
		SendToRemote(fundamental & 0x00FF);
		fbin = f2bin(fundamental);
		fund_amp = sqrt(pow(FFT_array[fbin],2) + pow(FFT_array[fbin+1],2));
		scalar = 200.0/fund_amp;		// constrain all harmonic amplitudes to 8 bits
		// send amplitude of first 13 harmonics

		for(i = 1; i <= NUM_HARMONICS; i++){
//			amp = compute_dft(signal, fundamental*i)*scalar;
			fbin = f2bin(fundamental*i);
			famp = sqrt(pow(FFT_array[fbin],2) + pow(FFT_array[fbin+1],2))*scalar;
			if(famp > 254) famp = 254;
			if(famp < 0) famp = 0;
			amp8 = (uint8_t)famp;
			printf("Harmonic %d: %d\n", i, famp);
			SendToRemote(amp8);
		}
		*/
//}
