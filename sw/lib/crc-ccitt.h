#ifndef	CRC_CCITT_H 
#define	CRC_CCITT_H 

//#define CRC16 0x8005
//#define CRC16 0x1021
#define CRC16 0x31C3

extern unsigned short calc_crc_ccitt(const unsigned char *buffer, int len); 
extern void append_crc_ccitt(unsigned char *buffer, int len); 
extern int check_crc_ccitt(const unsigned char *buffer, int len); 


#endif 
