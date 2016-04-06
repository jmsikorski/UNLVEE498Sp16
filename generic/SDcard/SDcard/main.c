/*----------------------------------------------------------------------*/
/* Foolproof FatFs sample project for AVR              (C)ChaN, 2014    */
/*----------------------------------------------------------------------*/
#define F_CPU 8000000UL
#include <avr/io.h>	/* Device specific declarations */
#include <util/delay.h>
#include "ff.h"	/* Declarations of FatFs API */
#include "diskio.h"

FATFS FatFs;		/* FatFs work area needed for each volume */
FIL Fil;			/* File object needed for each open file */

int main (void)
{
	UINT bw;
	DDRK |= 0xFF;
	FRESULT fr;
	
	f_mount(&FatFs, "", 0);		/* Give a work area to the default drive */

	PORTK |= 0x00;	/* Set PB4 high */
	_delay_ms(1000);
	PORTK |= 0xFF;

	disk_initialize(0);
	
	fr = f_open(&Fil, "newfile.txt", FA_WRITE | FA_CREATE_ALWAYS);	/* Create a file */

	if (fr)
	{
		PORTK = 0x00;
		_delay_ms(1000);
		PORTK = (int)fr;
		_delay_ms(5000);
		while(1);
	}
	PORTK |= 0x55;	/* Set PB4 high */
	_delay_ms(1000);
	PORTK ^= 0x55;

	f_write(&Fil, "It works! Good\r\n", 15, &bw);	/* Write data to the file */
	if (bw == 15) {		/* Lights green LED if data written well */
		PORTK |= 0xEF;	/* Set PB4 high */			
		}
	f_close(&Fil);								/* Close the file */

	PORTK |= 0x81;	/* Set PB4 high */
	_delay_ms(1000);
	PORTK |= 0xFF;

	for (;;) ;	
}
