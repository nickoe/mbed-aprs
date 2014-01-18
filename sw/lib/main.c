/* This is to test and develop the lib functions */

#include <stdio.h>
#include <stdint.h>
#include "aprs-ax25-tx.h"

#include "wavfile.h"
#include <math.h>


uint8_t datain[1024];
uint8_t dataout[1024]; // size allocation is just big "enough" for now
uint8_t nrzidata[1024]; // size allocation is just big "enough" for now
int16_t afsk[100000];

unsigned int length = 0;

uint8_t	aprs_destination[] = "APRS  0WIDE1 1WIDE2 1";
uint8_t aprs_source[] = "OZ3RF 1";
uint8_t aprs_string[] = "=5700.00N/01000.00W-Test 001252";

void main( void ) {
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
	printf("NRZI input\n");
	print_binary(dataout,2);
	printf("\nNRZI output\n");
	print_binary(nrzidata,2);
//	printf("\n01100010 00101010\n");
	printf("\n01100010 11001110 1\n");
////	wav_write(nrzidata,nrzidata);
	
	for (i = 0; i < size; i++) {
		printf("'");
		print_binary(&dataout[i],1);
		printf("\n");
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
		printf("");
		print_binary(&dataout[i],1);
		printf("\n");
	}
	nrzi_encode(dataout,nrzidata,size+5);
		for (i = 0; i < size+3; i++) {
		printf("'");
		print_binary(&nrzidata[i],1);
		printf("\n");
	}

	printf("i=%d,size=%d",i,size);
	

	FILE * f = wavfile_open("sound.wav");
	if(!f) {
		printf("couldn't open sound.wav for writing");
		//return 1;
	}
	int time;

	uint8_t foo[1];	
//	foo[0] = 0b01111110;
//	time = mod_afsk(foo,afsk,1);
//	wavfile_write(f,afsk,time-3);


//	time = mod_afsk(nrzidata,afsk,38+31);
//	wavfile_write(f,afsk,time);
	time = mod_afsk(nrzidata,afsk,size+2);
	wavfile_write(f,afsk,time);
//	foo[0] = 0b01111110;
//	time = mod_afsk(foo,afsk,1);
//	wavfile_write(f,afsk,time-3);

	wavfile_close(f);

}
