/*
 * Senior Design.c
 *
 * Created: 2/27/2016 7:15:24 PM
 * Author : jmsikorski
 */ 
#define F_CPU 8000000UL
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>

void usart_init(int);
void usart_send(int, char*);	

int main(void)
{
	usart_init(1);
	char a = '0';
	char test[20] = "TEST0\n";
    while (1) 
	{
		_delay_ms(1000);
		usart_send(1, test);
		a++;
		test[4] = a;
	}
	return 0;
}

void usart_init(int n)
{
	switch(n)
	{
		case 0:
			UCSR0B = (1<<TXEN0) | (1<<RXEN0); // TX Enable, RX Enable, RX Interrupt enabled
			UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); // 8 bit data segment
			UBRR0 = 0x33; // Baud rate
			break;
		case 1:
			UCSR1B = (1<<TXEN1) | (1<<RXEN1); // TX Enable, RX Enable, RX Interrupt enabled
			UCSR1C = (1<<UCSZ11) | (1<<UCSZ10); // 8 bit data segment
			UBRR1 = 0x33; // Baud rate
			break;
		case 2:
			UCSR2B = (1<<TXEN2) | (1<<RXEN2); // TX Enable, RX Enable, RX Interrupt enabled
			UCSR2C = (1<<UCSZ21) | (1<<UCSZ20); // 8 bit data segment
			UBRR2 = 0x33; // Baud rate
			break;
		case 3:
			UCSR3B = (1<<TXEN3) | (1<<RXEN3); // TX Enable, RX Enable, RX Interrupt enabled
			UCSR3C = (1<<UCSZ31) | (1<<UCSZ30); // 8 bit data segment
			UBRR3 = 0x33; // Baud rate
		break;
	}
}

void usart_send (int n, char* data)
{
	switch(n)
	{
		case 0:
			for(int i = 0; i < strlen(data); i++)
			{
				while (! (UCSR0A & (1<<UDRE0)));
				UDR0 = data[i];
			}
			break;
		case 1:
		for(int i = 0; i < strlen(data); i++)
		{
			while (! (UCSR1A & (1<<UDRE1)));
			UDR1 = data[i];
		}
		break;
		case 2:
		for(int i = 0; i < strlen(data); i++)
		{
			while (! (UCSR2A & (1<<UDRE2)));
			UDR2 = data[i];
		}
		break;
		case 3:
		for(int i = 0; i < strlen(data); i++)
		{
			while (! (UCSR3A & (1<<UDRE3)));
			UDR3 = data[i];
		}
		break;
	}
	return;
}