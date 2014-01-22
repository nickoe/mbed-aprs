/*
 * This is supposed to be a minimal exampe illustratig odd behaivour
 * of mod_afsk() and seeminly the same input, generates different
 * outputs.
 */  

#include "mbed.h"
extern "C" {
#include "aprs-ax25-tx.h"
}

DigitalOut led4(LED4);
DigitalOut trigger(p10); // Used for scope triggering

Ticker Sample_Period;
AnalogIn adcin(p20);

unsigned char byte = 0;

const int interval_us = 100000;
uint16_t adcvalue = 65535; // ADC is 12-bit
int print = 0;

AnalogOut DAC(p18);
volatile int afskcnt=0;

uint8_t datain[3];
uint8_t testing[3];

int afsktime;
int16_t afsk[1000];

void Sample_timer_interrupt(void)
{
	DAC.write_u16(afsk[afskcnt]+32768);
	if (afskcnt >(afsktime)) {
		trigger = 1;
		afskcnt = 0;
		trigger = 0;
	}
	else
		afskcnt++;
}


int main() {
	led4 = 0;

	datain[0] = 0x00;
	datain[1] = 0xff;
	datain[2] = 0x00;

	testing[0] = 0x00;
	testing[1] = 0xff;
	testing[2] = 0x00;

	// The testing array gives expected results, whilst the datain array
	// does not
//	afsktime = mod_afsk(&testing[0],afsk,2);
//	afsktime = mod_afsk(testing,afsk,2);
//	afsktime = mod_afsk(&datain[0],afsk,2);
	afsktime = mod_afsk(datain,afsk,2);
		
	Sample_Period.attach(&Sample_timer_interrupt, 1.0/(44100*1.83));
	while (1) {
		led4=1;
		wait(0.2);
		led4=0;
		wait(0.2);
	}
}

