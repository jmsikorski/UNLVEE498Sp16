/*
 * Senior Design.c
 *
 * Created: 2/27/2016 7:15:24 PM
 * Author : jmsikorski
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    /* Replace with your application code */
	DDRB = 0xFF;
	DDRD = 0XFF;
	DDRF = 0;
	ADCSRA = 0x87;
	ADMUX = 0xC0;
	int temp = 0;
	
    while (1) 
    {
		_delay_ms(200);
		ADCSRA |= (1<<ADSC);
		while((ADCSRA & (1<<ADIF))==0);
		temp = ADC;
		temp *= 9;
		temp /= 20;
		temp -= 58;
		PORTD = temp % 256;
		PORTB = temp / 256;
    }
	return 0;
}

