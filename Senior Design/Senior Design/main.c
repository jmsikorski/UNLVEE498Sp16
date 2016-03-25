/*
 * Senior Design.c
 *
 * Created: 2/27/2016 7:15:24 PM
 * Author : jmsikorski
 */ 
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>

void read_dylos(char*);
void read_temp(char*);
void read_MQ2(char*);
void read_MQ5(char*);
void read_MQ7(char*);
void usart_init(int);
void usart_send(int, char*);	
void usart_rec(int, char*);

char volatile rec;
char volatile rec_flag;

ISR(USART0_RX_vect)
{
	rec = UDR0;
	rec_flag = 1;
}

int main(void)
{
	DDRB = 0xFF;
	DDRC = 0xFF;
	DDRD = 0XFF;
	DDRF = 0;
	DDRA = 0;
	char buffer[100] = "hello - ";
	char dylos[10];
//	char a = 0x00;
	usart_init(0);
	sei();
	
    while (1) 
    {
		if(rec_flag)
		{
			dylos[strlen(dylos)] = rec;
			rec_flag = 0;
			if(dylos[strlen(dylos)] == '\n')
			{
				usart_send(0, dylos);
				for(int i = strlen(dylos); i > 0; i--)
				{
					dylos[i-1] = 0;
				}
			}
		}
		_delay_ms(1000);
		usart_send(0,buffer);
    }
	return 0;
}
void read_dylos(char* data)
{
	do 
	{
		usart_rec(0,data);
	} while (data[strlen(data)-1] != '\n');
	PORTB = PORTB ^ 0x08;
	return;
}

void read_temp(char* buffer)
{
	ADCSRA = 0x87;
	ADMUX = 0x04; //Use ADC4
	ADCSRA |= (1<<ADSC);
	while((ADCSRA & (1<<ADIF))==0);
	double t = ADC;	
	t *= 225;
	t /= 256;
	t -= 58;
	sprintf(buffer, "%d", (int)t);
	strcat(buffer, ",");
	return;
}

void read_MQ2(char* buffer) //C3H8 sensor
{
	double t; // solution variable
	int RO = 6600; // Output Resistance
	int RL = 9880; // Load Resistance
	float y = 0; // Rs/R0 variable
	float RS = 0; // Variable resistance
	float m = -0.00025; // slope
	float b = 1.1; // intercept
	float vout; // sensor output voltage
	char vin = 5; // sensor input voltage
	ADCSRA = 0x87;
	ADMUX = 0x00; //Use ADC0
	ADCSRA |= (1<<ADSC);
	while((ADCSRA & (1<<ADIF))==0);
	vout = ADC; // solve for Vout
	vout *= 5;
	vout /= 1024;
	RS = (RL*(vin-vout))/vout; // Solve for RS
	y = RS/RO;
	if (y > 2)
		t = 0;
	else
		t = (y-b)/m;
	sprintf(buffer, "%d", (int)t);
	strcat(buffer, ",");
	return;
}

void read_MQ5(char* buffer) // CH4 sensor
{
	double t; // solution variable
	int RO = 15750; // Output Resistance
	int RL = 19790; // Load Resistance
	float y = 0; // Rs/R0 variable
	float RS = 0; // Variable resistance
	float m = -0.000361; // slope
	float b = 1.0222; // intercept
	float vout; // sensor output voltage
	char vin = 5; // sensor input voltage
	ADCSRA = 0x87;
	ADMUX = 0x01; //Use ADC1
	ADCSRA |= (1<<ADSC);
	while((ADCSRA & (1<<ADIF))==0);
	vout = ADC; // solve for Vout
	vout *= 5;
	vout /= 1024;
	RS = (RL*(vin-vout))/vout; // Solve for RS
	y = RS/RO;
	if (y > 1)
	t = 0;
	else
	t = (y-b)/m;
	sprintf(buffer, "%d", (int)t);
	strcat(buffer, ",");
	return;
}

void read_MQ7(char* buffer) //C0 sensor
{
	double t; // solution variable
	int RO = 2000; // Output Resistance
	int RL = 9880; // Load Resistance
	float y = 0; // Rs/R0 variable
	float RS = 0; // Variable resistance
	float m = -0.001298; // slope
	float b = 1.778; // intercept
	float vout; // sensor output voltage
	char vin = 5; // sensor input voltage
	ADCSRA = 0x87;
	ADMUX = 0x02; //Use ADC2
	ADCSRA |= (1<<ADSC);
	while((ADCSRA & (1<<ADIF))==0);
	vout = ADC; // solve for Vout
	vout *= 5;
	vout /= 1024;
	RS = (RL*(vin-vout))/vout; // Solve for RS
	y = RS/RO;
	if (y > 2)
	t = 0;
	else
	t = (y-b)/m;
	sprintf(buffer, "%d", (int)t);
	strcat(buffer, ",");
	return;
}

void usart_init(int n)
{
	switch(n)
	{
		case 0:
			UCSR0B = (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0); // TX Enable, RX Enable
			UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); // 8 bit data segment
			UBRR0 = 0x33; // Baud rate
			break;
		case 1:
			UCSR1B = (1<<TXEN1) | (1<<RXEN1); // TX Enable, RX Enable
			UCSR1C = (1<<UCSZ11) | (1<<UCSZ10); // 8 bit data segment
			UBRR1 = 0x33; // Baud rate
			break;
		case 2:
			UCSR2B = (1<<TXEN2) | (1<<RXEN2); // TX Enable, RX Enable
			UCSR2C = (1<<UCSZ21) | (1<<UCSZ20); // 8 bit data segment
			UBRR2 = 0x33; // Baud rate
			break;
		case 3:
			UCSR3B = (1<<TXEN3) | (1<<RXEN3); // TX Enable, RX Enable
			UCSR3C = (1<<UCSZ31) | (1<<UCSZ30); // 8 bit data segment
			UBRR3 = 0x33; // Baud rate
		break;
	}
}

void usart_send (int n, char* data)
{
	for(int i = 0; i < strlen(data); i++)
	{
		while (! (UCSR0A & (1<<UDRE0)));
		UDR0 = data[i];
	}
	return;
}

void usart_rec (int n, char* data)
{
	int len = strlen(data);
	while(! (UCSR0A & (1<<RXC0)));
	data[len] = UDR0;
}