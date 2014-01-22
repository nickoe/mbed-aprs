/*
 * This is intended to be a proff of consept APRS TNC, that you can connect to your radio
 * Features
 *  AFSK modulation @TODO
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
DigitalOut trigger(p10);

Serial pc(USBTX,USBRX);

Ticker ticker;
AnalogIn adcin(p20);

unsigned char byte = 0;

const int interval_us = 100000;
uint16_t adcvalue = 65535; // ADC is 12-bit
int print = 0;
int gonote =0;

Ticker Sample_Period;
AnalogOut DAC(p18);
volatile int i=0;
float Analog_out_data[128];
float gain = 0.23;
float	freq = 1200;


uint8_t datain[1024];
uint8_t dataout[1024]; // size allocation is just big "enough" for now
uint8_t nrzidata[1024]; // size allocation is just big "enough" for now
int16_t afsk[100];

unsigned int length = 0;

uint8_t	aprs_destination[] = "APRS  0WIDE1 1WIDE2 1";
uint8_t aprs_source[] = "OZ3RF 1";
uint8_t aprs_string[] = "=5700.00N/01000.00W-Test 001252";

int afsktime;

void Sample_timer_interrupt(void)
{
//	DAC = Analog_out_data[i];
	//DAC.write_u16(Analog_out_data[i]);
	DAC.write_u16(afsk[i]+32768);
	//i = (i+1) & 0x007F;
//	i = (i+1) & 0x1F;
	if (i >afsktime) {
		trigger = 1;
		i = 0;
		trigger = 0;
	}
	else
		i++;
}

Ticker twoTone;

void sample(void)
{
	adcvalue = adcin;
	print = 1;
}

void set_freq(void)
{
	if (freq == (float) 800)
		freq = 969;
	else
		freq = 800;
}


int main() {
	// Find lenght of data
	int i = 0;
	int size = 0;
	while( aprs_string[i] != 0 ) {
		i++;
//		printf("%d\n",i);
	}

	ax25_pack(aprs_destination,
		aprs_source,
		aprs_string,
		dataout,
		size);
	size=i+34; //34+31
	  // NRZI test
//	dataout[0] = 0b01011001;
//	dataout[1] = 0b10000000;
//	dataout[0] = 0b01011000;
//	dataout[1] = 0b10101100;
//	nrzi_encode(dataout,nrzidata,2);
///	printf("NRZI input\n");
///	print_binary(dataout,2);
///	printf("\nNRZI output\n");
///	print_binary(nrzidata,2);
//	printf("\n01100010 00101010\n");
///	printf("\n01100010 11001110 1\n");
////	wav_write(nrzidata,nrzidata);
	
	for (i = 0; i < size; i++) {
///		printf("'");
///		print_binary(&dataout[i],1);
///		printf("\n");
	}
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


	for (i = 0; i < size+3; i++) {
		//printf("");
///		print_binary(&dataout[i],1);
///		printf("\n");
	}
	nrzi_encode(dataout,nrzidata,size+5);
		for (i = 0; i < size+3; i++) {
///		printf("'");
///		print_binary(&nrzidata[i],1);
///		printf("\n");
	}

///	printf("i=%d,size=%d",i,size);
	


//	uint8_t foo[1];	
//	foo[0] = 0b01111110;
//	time = mod_afsk(foo,afsk,1);
//	wavfile_write(f,afsk,time-3);


//	time = mod_afsk(nrzidata,afsk,38+31);
//	wavfile_write(f,afsk,time);
//	time = mod_afsk(nrzidata,afsk,size+2);
//	mod_afsk(nrzidata,afsk,size+2);













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


		pc.printf("hej\nhej\nhej\nhej");

//		for(int k=0; k<128; k++) {
//			Analog_out_data[k]=((1.0 + sin((float(k)/128.0*6.28318530717959)))/2.0);
	for(int k=0; k<32; k++) {                                               
		Analog_out_data[k] = int (65535.0 * ((1.0 + sin((float(k)/32.0*6.28318530717959)))/2.0)) * gain;
    }
		
	ticker.attach_us(sample, interval_us);
//	twoTone.attach_us(set_freq, 500000);
  while (1) {
		if (print == 1) {
			adcvalue = adcin.read_u16();
			pc.printf("%u",adcvalue);
			print = 0;
			led1 = !led1;
			pc.printf("\r\n");
			if (adcvalue >= 1000) {
				led2 = 1;
				gonote = 0;
			} else {
				led2 =0;
				gonote = 1;
			}
		}
		led4=1;
		wait(0.5);
		led4=0;
		wait(0.5);
		if (gonote ==1) {
			Sample_Period.attach(&Sample_timer_interrupt, 1.0/(freq*296));
		} else {
			DAC.write_u16(0); // Not to load the transistor with DC
			Sample_Period.detach();
		}
  }
}

