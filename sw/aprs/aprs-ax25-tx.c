#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "aprs-ax25-tx.h"
#include "crc-ccitt.h"
#include "wavfile.h"
#include "afsk.h"

void print_binary(uint8_t data[], int size) {
	int i,j;
	for (j = 0; j <= size-1; j++) {
		for (i = 7; i>= 0 ;i--) {
			//printf("%d", data[j]>>i & 0x01 );
		}
		//printf(" ");
	}
}


const unsigned char flip_table[256] = {	
  0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0, 
  0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0, 
  0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8, 
  0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8, 
  0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4, 
  0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4, 
  0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec, 
  0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc, 
  0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2, 
  0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2, 
  0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea, 
  0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa, 
  0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6, 
  0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6, 
  0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee, 
  0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe, 
  0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1, 
  0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1, 
  0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9, 
  0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9, 
  0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5, 
  0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5, 
  0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed, 
  0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd, 
  0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3, 
  0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3, 
  0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb, 
  0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb, 
  0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7, 
  0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7, 
  0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef, 
  0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

unsigned char byte_flip(unsigned char a) {
	//printf("%i\n",flip_table[a]);
	return flip_table[a];
}


unsigned char ascii7(unsigned char a) {
//  unsigned char tmp,i;
//  if (65 <= a <= 90) { // A-Z
  if ((65 <= a) || (a <= 90)) { // A-Z
		return a << 1;
  } else {
    return a;
  }
}


void ax25_pack( uint8_t destination[], uint8_t  source[], uint8_t aprs_data[],
							  uint8_t dataout[], int size ) {
///	printf("ax25_pack()\n");

///	printf("%s\n", destination);
///	printf("%s\n", source);
///	printf("%s\n", aprs_data);

	int i;
//	uint8_t tmp;

	// Flag
	dataout[0] = 0x7e;

	// Addresses (source and destination)
	memcpy(dataout +1,destination,7);
	memcpy(dataout +1+7,source,8);
	memcpy(dataout +1+7+7,&destination[7],2*7);

	// Addreses are in 7 bit ASCII aligned to MSB
	for (i = 1; i<=28;i++){
		dataout[i] = (uint8_t)ascii7((unsigned char)dataout[i]);
	}

 	// LSB bit indicates end of addresses
	dataout[28] = dataout[28] | 0x01;
	
	// AX.25 frame format bytes
	dataout[29] = 0x03; // Control field (UI frame) 
	dataout[30] = 0xF0; // Protocol ID (no layer 3 protocol)

	// Calculating the variable size of the message text
	int msg_size = 0;
	while( aprs_data[msg_size] != 0) {
		msg_size++;
//		printf("APRS Message String Length: %d\n", msg_size);
	}

	// APRS data
	memcpy(dataout +31,aprs_data,msg_size);

	// Calculate FCS (CRC)
	uint16_t crc;
	crc = calc_crc_ccitt(&dataout[1],61);
//	printf("CRC: 0x%x\n",crc);
	// Put FCS in
	dataout[31+msg_size] = (uint8_t) crc & 0x00ff;
	dataout[32+msg_size] = (uint8_t) ((crc >> 8) & 0x00ff);

	// Flip bytes to confrom with HDLC framing
	for (i = 0; i<31+msg_size+2; i++) {
		dataout[i] = (uint8_t)byte_flip((unsigned char)dataout[i]);
	}
///	printf("0x%x\n",calc_crc_ccitt(dataout+1,61));

	// Flag
	dataout[33+msg_size] = 0x7e;
}


void bitstuff( uint8_t input[], uint8_t  output[], int size ) {
///	printf("bitstuff()\n");
	int mm=0, nbyte, b;
	int outbitcnt=0;
	uint8_t bit;
	for (nbyte = 0; nbyte < size; nbyte++) {
		for(b=7;b+1>0;b--){
			// Read current bit
			bit = ((input[nbyte] >> (b)) & 0x01);

			if (mm == 5) { // Number of consequtive ones
				output[(int)(outbitcnt/8)] &= ~(0x01 << (abs(outbitcnt%8-7)));
				mm = 0;
				outbitcnt++;
///				printf("DEBUG: DESTUFFINGERHERE\n");
			}
			
			// Count ones
			if ( bit == 1) {
				mm++;
			//		printf("d=%d\tmm=\t%d\n",b,mm);
			} else {
				mm = 0;
			}
			// Write the read bit to output
			//printf("%i - %i\n",b,bit);
			//printf("byteindex: %d\nshiftindex: %d\n",(int)(outbitcnt/8),abs(outbitcnt%8-7));
			if (bit) {
				output[(int)(outbitcnt/8)] |= 0x01 << (abs(outbitcnt%8-7)); // Write 1
			} else {
				output[(int)(outbitcnt/8)] &= ~(0x01 << (abs(outbitcnt%8-7))); // Write 0
			}
			outbitcnt++;
		}
	}
}


void nrzi_encode( uint8_t input[], uint8_t  output[], int size ) {
///	printf("nrzi_encode()\n");
	int nbyte, b;
	int outbitcnt=0;
	uint8_t nrzistate=0;
	uint8_t bit;
	for (nbyte = 0; nbyte < size; nbyte++) {
		for(b=7;b+1>0;b--) {
			bit = ((input[nbyte] >> (b)) & 0x01);

			if (bit == 0) {
				if (nrzistate == 1) {
					nrzistate = 0;
					output[(int)(outbitcnt/8)] &= ~(0x01 << (abs(outbitcnt%8-7))); // Zero
				} else {
					nrzistate = 1;
					output[(int)(outbitcnt/8)] |= 0x01 << (abs(outbitcnt%8-7)); // One
				}
			} else {
				if (nrzistate == 1)
					output[(int)(outbitcnt/8)] |= 0x01 << (abs(outbitcnt%8-7)); // One
				if (nrzistate == 0)
					output[(int)(outbitcnt/8)] &= ~(0x01 << (abs(outbitcnt%8-7))); // Zero
			}
			outbitcnt++;
		}
	}
}


int mod_afsk( uint8_t input[], int16_t  output[], int *tabpos, int size ) {

//	uint16_t tabpos = 0; // Position in table
	int8_t sgn = 1; // Sign
	uint16_t time = 0; // Time
	int kk,b,nn;
	uint8_t bit;

//	printf("DELTA0:\t %d\n",DELTA0);
//	printf("DELTA1:\t %d\n",DELTA1);
//	printf("SPS:\t %d\n",SPS);

///	printf("input to mod_afsk(): %d\r\n", *tabpos);
if (*tabpos<0){         
	sgn = -1;            
	*tabpos = (*tabpos)*(-1);
}                      

	for (kk = 0;kk<size;kk++) {
		for(b=7;b+1>0;b--){                                                         
		// Read current bit                                                       
		bit = ((input[kk] >> (b)) & 0x01);  
//		printf("%d",bit);
			if (bit==1) {
				for (nn=0;nn<SPS;nn++){
					if (*tabpos >= LOOKUP_SIZE) {
						*tabpos = *tabpos - LOOKUP_SIZE;
						sgn = sgn*(-1);
					}
					output[time] = AMPLITUDE*table[*tabpos]*sgn;
					time++;
					*tabpos += DELTA1;
				}
			} else {
				for (nn=0;nn<SPS;nn++){
					if (*tabpos >= LOOKUP_SIZE) {
						*tabpos = *tabpos - LOOKUP_SIZE;
						sgn = sgn*(-1);
					}
					output[time] = AMPLITUDE*table[*tabpos]*sgn;
					time++;
					*tabpos += DELTA0;
				}
			}
		}
	}
if (sgn<0)              
	*tabpos =  (*tabpos*(-1)); 
else if (sgn>0)         
	*tabpos =  *tabpos;        

///	printf("Output: %d\r\n", *tabpos);

	return time;

                        

}


/*
const int NUM_SAMPLES = (WAVFILE_SAMPLES_PER_SECOND*2);

void wav_write( uint8_t input[], uint8_t  output[] ) {
	short waveform[NUM_SAMPLES];
	double frequency = 440.0;
	int volume = 32000;
	int length = NUM_SAMPLES;

	int i;
	for(i=0;i<length;i++) {
		double t = (double) i / WAVFILE_SAMPLES_PER_SECOND;
		waveform[i] = volume*sin(frequency*t*2*M_PI);
	}

	FILE * f = wavfile_open("sound.wav");
	if(!f) {
		printf("couldn't open sound.wav for writing");
		//printf("couldn't open sound.wav for writing: %s",strerror(errno));
		//return 1;
	}

	wavfile_write(f,waveform,length);
	wavfile_close(f);

	//return 0;
}
*/


void aprs_send( uint8_t input[], uint8_t  output[] ) {

}
