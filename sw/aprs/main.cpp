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
volatile int afsktime=0;
int tabpos=0;

uint8_t datain[300];
uint8_t dataout[300]; // size allocation is just big "enough" for now
uint8_t nrzidata[300]; // size allocation is just big "enough" for now
uint8_t aprs_destination[] = "APRS  0WIDE1 1WIDE2 1";
uint8_t aprs_source[] = "OZ3RF 1";
uint8_t aprs_string[] = "=5700.00N/01000.00W-Test 001252";

typedef struct {
	int16_t data[AFSKSIZE*2]; // One bit is one symbol, hence one byte takes SPS*8
	uint16_t size;
	uint16_t head; // Put to here
	uint16_t tail; // Get from here
} audiobuf_t;

audiobuf_t afsk;

int ringbuffer_write(audiobuf_t * buf_ptr, int16_t data[], int size) {
	int tmp=0;
	tmp = buf_ptr->head;
	for (int i=0; i < size; i++) {
		buf_ptr->data[tmp+i] = data[i];
	}
	buf_ptr->head = tmp+size;
	return 0;
}

int ringbuffer_read(audiobuf_t * buf_ptr, int size) {
	return 0;
}



void Sample_timer_interrupt(void)
{
	DAC.write_u16(afsk.data[afskcnt]+32768);
	if (afskcnt >afsktime) {
		trigger = 1;
		afskcnt = 0;
		trigger = 0;
	}
	else
		afskcnt++;
}



int main() {
	pc.baud(115200);
	const char buildtime[] =  __DATE__ " " __TIME__;
	const char gitcommit[sizeof(__GIT_COMMIT__)] = __GIT_COMMIT__;
	printf("\r\nAPRS tracker by OZ3RF\r\n");
	printf("%s\r\n%s\r\n", buildtime, gitcommit);
	pc.printf("%d", AFSKSIZE);
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
///	pc.printf("\n\n\n\n\nafsktime: %d\r\nSPS: %d\r\n", afsktime,SPS);
	


	datain[0] = 0x00;
	datain[1] = 0xff;

//	afsktime = mod_afsk(datain,afsk,2);
	pc.printf("\n\n\n\n\nafsktime: %d\r\nSPS: %d\r\n", afsktime,SPS);

	int ready=0; // Set to true when we want to send a packet


	Sample_Period.attach(&Sample_timer_interrupt, 1.0/(44100));
	while (1) {

	// Loop through every byte in our packet
	size =1 ;
	uint16_t j = 0;
	for (j=0; j<(size+2); j++) {
		afsktime = mod_afsk(nrzidata,afsk.data,&tabpos,1);
	}
	pc.printf("%d\r\n", j);

		led1 = 1;
		wait(1.15);
		led1 = 0;
/*
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
	*/	
	}
}

