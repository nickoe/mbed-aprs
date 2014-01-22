/*
 * This is supposed to be a minimal exampe illustratig odd behaivour
 * of mod_afsk() and seeminly the same input, generates different
 * outputs.
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

Serial pc(USBTX,USBRX);

Ticker Sample_Period;
AnalogIn adcin(p20);

unsigned char byte = 0;

const int interval_us = 100000;
uint16_t adcvalue = 65535; // ADC is 12-bit
int print = 0;

AnalogOut DAC(p18);
volatile int i=0;
float gain = 0.23;
float freq = 1200;


uint8_t datain[1024];
uint8_t dataout[1024]; // size allocation is just big "enough" for now
uint8_t nrzidata[1024]; // size allocation is just big "enough" for now
int16_t afsk[100];

unsigned int length = 0;

uint8_t aprs_destination[] = "APRS  0WIDE1 1WIDE2 1";
uint8_t aprs_source[] = "OZ3RF 1";
uint8_t aprs_string[] = "=5700.00N/01000.00W-Test 001234";

int afsktime;

void Sample_timer_interrupt(void)
{
	DAC.write_u16(afsk[i]+32768);
	if (i >afsktime) {
		trigger = 1;
		i = 0;
		trigger = 0;
	}
	else
		i++;
}

int main() {
	led1 = 1;
	led2 = 0;
	led3 = 0;
	led4 = 0;
	pc.baud(115200);

	datain[0] = 0x00;
	datain[1] = 0xff;

	uint8_t testing[3] = {0x00,0xff,0x00};

	pc.printf("0x%X\r\n",(uint8_t)testing[0]);
	pc.printf("0x%X\r\n",(uint8_t)testing[1]);
	afsktime = mod_afsk(&testing[0],afsk,2);
//afsktime = mod_afsk(&datain[0],afsk,2);

	pc.printf("FOOOO %d\r\n",afsktime);
	pc.printf("0x%X\r\n",(uint8_t)testing[0]);
	pc.printf("0x%X\r\n",(uint8_t)testing[1]);
		
	Sample_Period.attach(&Sample_timer_interrupt, 1.0/(freq*296));
	while (1) {
		led4=1;
		wait(0.5);
		led4=0;
		wait(0.5);
	}
}

