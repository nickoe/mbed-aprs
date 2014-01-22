/*
 * This is intended to be a proff of consept APRS TNC, that you can connect to your radio
 * Features
 *  AFSK modulation
 *  AFSK demodulation @TODO
 *  NMEA interpretion form GPS @TODO
 */  

#include "mbed.h"
extern "C" {
#include "aprs-ax25-tx.h"
}

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
DigitalOut trigger(p10); // Used for scope triggering
AnalogOut DAC(p18); // Analog out
Serial pc(USBTX,USBRX);

unsigned char byte = 0;

const int interval_us = 100000;
uint16_t adcvalue = 65535; // ADC is 12-bit

Ticker Sample_Period;
volatile int afskcnt=0;

uint8_t datain[300];
uint8_t dataout[300]; // size allocation is just big "enough" for now
uint8_t nrzidata[300]; // size allocation is just big "enough" for now
int16_t afsk[1000];

uint8_t	aprs_destination[] = "APRS  0WIDE1 1WIDE2 1";
uint8_t aprs_source[] = "OZ3RF 1";
uint8_t aprs_string[] = "=5700.00N/01000.00W-Test 001252";

int afsktime;

void Sample_timer_interrupt(void)
{
	DAC.write_u16(afsk[afskcnt]+32768);
	if (afskcnt >afsktime) {
		trigger = 1;
		afskcnt = 0;
		trigger = 0;
	}
	else
		afskcnt++;
}



int main() {
	int i=0;
	int size=0;
	while( aprs_string[i] != 0)
		i++;

	ax25_pack(aprs_destination,
		aprs_source,
		aprs_string,
		dataout,
		size);

	size=i+34; //34+31
	
	// Duplicate start flad and appedn some stop flags (temoporay fix to
	// make some more flags)
	for (i = 0; i < size ; i++ ) {
		datain[i] = dataout[i];
	}
	bitstuff(datain+1,dataout+1,size-2);
	dataout[64] = 0b10111111;
	dataout[65] = 0b00111111;

	for (i = 0; i < 70 ; i++ ) {
		datain[i] = dataout[i];
	}
	for (i = 0; i < 70 ; i++ ) {
		dataout[i+1] = datain[i];
	}


	nrzi_encode(dataout,nrzidata,size+5);

//afsktime = mod_afsk(nrzidata,afsk,size+2);
	afsktime = mod_afsk(nrzidata,afsk,1);

	pc.baud(115200);

//	datain[0] = 0x00;
//	datain[1] = 0xff;

//	afsktime = mod_afsk(datain,afsk,2);
	pc.printf("\n\n\n\n\nafsktime: %d\r\nSPS: %d\r\n", afsktime,SPS);

	Sample_Period.attach(&Sample_timer_interrupt, 1.0/(44100*1.83));
	while (1) {
		led1 = 1;
		wait(0.05);
		led1 = 0;
		led2 = 1;
		wait(0.05);
		led2 = 0;
		led3 = 1;
		wait(0.05);
		led3 = 0;
		led4 = 1;
		wait(0.05);
		led4 = 0;
		led3 = 1;
		wait(0.05);
		led3 = 0;
		led2 = 1;
		wait(0.05);
		led2 = 0;
	}
}

