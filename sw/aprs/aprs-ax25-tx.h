#ifndef	APRS_AX25_TH_H 
#define	APRS_AX25_TX_H 

/*
 * Defines specifying the properties of the AFSK format
 */
#define BAUD 1200
#define SPACE 1200
#define MARK 2200
#define LOOKUP_SIZE 1024
#define SAMP_FREQ 44100
#define AMPLITUDE 0.5
#define DELTA0 (int) round(2*LOOKUP_SIZE*SPACE/SAMP_FREQ)
#define DELTA1 (int) round(2*LOOKUP_SIZE*MARK/SAMP_FREQ)
#define SPS (SAMP_FREQ/BAUD) // Samples per symbol
#define AFSKSIZE ((SPS+1)*8) // The +1 is just to conform with MATLAB for the moment

/*
 * Just a function that prints ones and zeroes in the bytes it
 * recieves with ptintf()
 */
void print_binary(uint8_t data[], int size);

/*
 * Packs a AX.25 packet by getting the AX.25 frame addresses and APRS
 * formatted data section
 */
void ax25_pack( uint8_t destination[], uint8_t  source[], uint8_t aprs_data[],
                uint8_t dataout[], int size );

/*
 * Flip the MSB first to LSB first alignment for a byte
 */
unsigned char byte_flip(unsigned char a);

/* Callsigns are 7 bit standard ASCII (Caps only), and is put in the MSB part of the
 octet (byte), that means that we need to bitshift the characters A-Z 1
 bit left. Dash (-) and integers is the same in 8 bit and 7 bit ASCII. */
unsigned char ascii7(unsigned char a);

/*
 * Loops evey bit in input to get the var bit as a "boolean", then it
 * maintains a seperate counter indicating the output bit.
 * One could also go around to do the bitstuffing in the actual tx
 * path, i.e. in the modulator. It might be simpler.
 */
void bitstuff( uint8_t input[], uint8_t  output[], int size );

/*
 * NRZI (Non-return-to-zero inverted) encoding
 */
void nrzi_encode( uint8_t input[], uint8_t  output[], int size );

/*
 * AFSK modulate the bitstuffed and NRZI encoded data. It might be
 * wise for this function to also return the current state tabpos
 * variable, so one can start it where it ended last time. This is
 * usefull to output partial packets.
 */
int mod_afsk( uint8_t input[], int16_t  output[], int *tabpos, int size );

/*
 * This function is used to output the signal in a wav file when on
 * workstation, instead of the analog output on the microcontroller.
 */
//void wav_write( uint8_t input[], uint8_t  output[] );

/*
 * Not implemented yet, was supposed to be an interface to construft
 * packets easily
 */
void aprs_send( uint8_t input[], uint8_t  output[] );
 
#endif 
