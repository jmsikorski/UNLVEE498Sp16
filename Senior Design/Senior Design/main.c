/*
 * Senior Design.c
 *
 * Created: 2/24/2016 9:32:01 AM
 * Author : jmsikorski
 */ 

#include <avr/io.h>

void read_temp(int t);

int main(void)
{
	int temp = 0;
	while(1)
	{
		read_temp(temp);
		DDRD = 0xFF;
		DDRD = temp % 255;
	}
	return 0;
}

void read_temp(int t)
{
	DDRF &= (0<<ADC0D);	//make Port F[0] an input for ADC input
	DIDR0 &= (1<<ADC0D);//disable digital input buffer on F[0]
	ADCSRA = 0x87;		//make ADC enable and select ck/128
	ADMUX = 0xC0;		//2.56V Vref, ADC0 single ended input
						//data will be right-justified
	ADCSRA |= (1<<ADSC);	//start conversion
	while((ADCSRA & (1<<ADIF))==0);	//wait for conversion to finish
	t = ADC;
}

