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
void parse_GPS(char*, char*);
void parse_dylos(char*, char*);
void strclr(char*);
void init_GPS();

char volatile rec_dylos[10];
char volatile rec_dylos_flag = 0;
char volatile rec_GPS[100];
char volatile rec_GPS_flag = 0;

ISR(USART0_RX_vect)
{
	rec_dylos[strlen(rec_dylos)] = UDR0;
	rec_dylos_flag = 1;
}

ISR(USART1_RX_vect)
{
	rec_GPS[strlen(rec_GPS)] = UDR1;
	rec_GPS_flag = 1;
}

int main(void)
{
	DDRB = 0xFF;
	DDRC = 0xFF;
	DDRD = 0XFF;
	DDRF = 0;
	DDRA = 0;
	char GPS[100];
	char dylos[10];
	char GPS_line = 0;
	char dylos_line = 0;
	char GPS_parsed[100];
	char dylos_parsed[30];
	char buffer[100];
	char tx[150];
	usart_init(0);
	usart_init(1);
	usart_init(2);
	usart_init(3);
	init_GPS();
	int a = 0;
	sei();

    while (1) 
    {
		if(rec_dylos_flag == 1)
		{
			rec_dylos_flag = 0;
			if(rec_dylos[strlen(rec_dylos) - 1] == '\n')
			{
				dylos_line = 1;
				strncpy(dylos, rec_dylos, strlen(rec_dylos));
				strclr(rec_dylos);			
			}
		}

		if(rec_GPS_flag == 1)
		{
			rec_GPS_flag = 0;
			if(rec_GPS[strlen(rec_GPS) - 1] == '\n')
			{
				GPS_line = 1;
				strncpy(GPS, rec_GPS, strlen(rec_GPS));
				strclr(rec_GPS);
			}
		}
		
		if(dylos_line == 1)
		{
			strclr(dylos_parsed);
			parse_dylos(dylos, dylos_parsed);
			dylos_line = 0;
		}
		
		if(GPS_line == 1)
		{
			strclr(GPS_parsed);
			parse_GPS(GPS, GPS_parsed);
			GPS_line = 0;
			strncpy(tx, GPS_parsed, strlen(GPS_parsed));
			strcat(tx, dylos_parsed);
			read_temp(buffer);
			read_MQ2(buffer);
			read_MQ5(buffer);
			read_MQ7(buffer);
			strcat(tx, buffer);
			usart_send(3, tx);
			strclr(tx);
			strclr(buffer);
		}
	}
	return 0;
}

void read_temp(char* buffer)
{
	char temp[10];
	ADCSRA = 0x87;
	ADMUX = 0x04; //Use ADC4
	ADCSRA |= (1<<ADSC);
	while((ADCSRA & (1<<ADIF))==0);
	double t = ADC;	
	t *= 225;
	t /= 256;
	t -= 58;
	strcat(buffer, "Temperature: ");
	sprintf(temp, "%dF\n", (int)t);
	strcat(buffer, temp);
	return;
}

void read_MQ2(char* buffer) //C3H8 sensor
{
	char temp[10];
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
	strcat(buffer, "C3H8: ");
	sprintf(temp, "%d ppm\n", (int)t);
	strcat(buffer, temp);
	return;
}

void read_MQ5(char* buffer) // CH4 sensor
{
	char temp[10];
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
	strcat(buffer, "CH4 : ");
	sprintf(temp, "%d ppm\n", (int)t);
	strcat(buffer, temp);
	return;
}

void read_MQ7(char* buffer) //C0 sensor
{
	char temp[10];
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
	strcat(buffer, "CO  : ");
	sprintf(temp, "%d ppm\n", (int)t);
	strcat(buffer, temp);
	return;
}

void usart_init(int n)
{
	switch(n)
	{
		case 0:
			UCSR0B = (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0); // TX Enable, RX Enable, RX Interrupt enabled
			UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); // 8 bit data segment
			UBRR0 = 0x33; // Baud rate
			break;
		case 1:
			UCSR1B = (1<<TXEN1) | (1<<RXEN1) | (1<<RXCIE1); // TX Enable, RX Enable, RX Interrupt enabled
			UCSR1C = (1<<UCSZ11) | (1<<UCSZ10); // 8 bit data segment
			UBRR1 = 0x33; // Baud rate
			break;
		case 2:
			UCSR2B = (1<<TXEN2) | (1<<RXEN2) | (1<<RXCIE2); // TX Enable, RX Enable, RX Interrupt enabled
			UCSR2C = (1<<UCSZ21) | (1<<UCSZ20); // 8 bit data segment
			UBRR2 = 0x33; // Baud rate
			break;
		case 3:
			UCSR3B = (1<<TXEN3) | (1<<RXEN3) | (1<<RXCIE3); // TX Enable, RX Enable, RX Interrupt enabled
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

void parse_GPS(char* in, char* out)
{
	int p = 0;
	int comma = 0;
	int i = 0;
	int hour, min, sec, fix, latd, lats, latm, lond, lonm, lons, ns, ew;
	char temp[100];
	if(in[1] == 'G')
	{
		while(comma < 12)
		{
			if(in[p] == 44)
			{
				comma++;
				p++;
				switch(comma)
				{
					case 1:
						strcat(out, "\nTime: ");
						temp[0] = in[p++];
						temp[1] = in[p++];
						temp[2] = ':';
						temp[3] = in[p++];
						temp[4] = in[p++];
						temp[5] = ':';
						temp[6] = in[p++];
						temp[7] = in[p++];
						temp[8] = 0;
						sscanf(temp,"%d:%d:%d",&hour,&min,&sec);
						if(hour > 6)
							hour = hour-7;
						else
							hour = hour + 17;
						break;
					case 2:
						temp[0] = in[p++];
						temp[1] = in[p++];
						temp[2] = '.';
						temp[3] = in[p++];
						temp[4] = in[p++];
						temp[5] = in[p++];
						temp[6] = in[p++];
						temp[7] = in[p++];
						temp[8] = in[p++];
						temp[9] = in[p++];
						temp[10] = 0;
						sscanf(temp, "%d.%d.%d", &latd, &latm, &lats);
						break;
					case 3:
						if(in[p++] == 'N')
							ns = 0;
						else
							ns = 1;
						break;
					case 4:
						temp[0] = in[p++];
						temp[1] = in[p++];
						temp[2] = in[p++];
						temp[3] = '.';
						temp[4] = in[p++];
						temp[5] = in[p++];
						temp[6] = in[p++];
						temp[7] = in[p++];
						temp[8] = in[p++];
						temp[9] = in[p++];
						temp[10] = in[p++];
						temp[11] = 0;
						sscanf(temp, "%d.%d.%d", &lond, &lonm, &lons);
						break;
					case 5:
						if(in[p++] == 'E')
							ew = 0;
						else
							ew = 1;
						break;
					case 6:
						if(in[p++] == '1')
							fix = 1;
						else
							fix = 0;
						break;
					case 9:
						while(in[p] != ',')
						{
							temp[i] = in[p++];
							i++;
						}
						temp[i++] = '\n';
						temp[i] = 0;
						break;
				}
			}
			else
				p++;
		}
	}
	else
		return;
	if(fix == 1)
	{
		if(ns == 0 && ew == 0)
			sprintf(out, "\nTime: %.2d:%.2d:%.2d\nLatitude: %d %d.%d\nLongitude: %d %d.%d\nAltitude: ", hour, min, sec, latd, latm, lats, lond, lonm, lons);
		else if(ns == 1 && ew == 0)
			sprintf(out, "\nTime: %.2d:%.2d:%.2d\nLatitude: -%d %d.%d\nLongitude: %d %d.%d\nAltitude: ", hour, min, sec, latd, latm, lats, lond, lonm, lons);
		else if(ns == 0 && ew == 1)
			sprintf(out, "\nTime: %.2d:%.2d:%.2d\nLatitude: %d %d.%d\nLongitude: -%d %d.%d\nAltitude: ", hour, min, sec, latd, latm, lats, lond, lonm, lons);
		else if(ns == 1 && ew == 1)
			sprintf(out, "\nTime: %.2d:%.2d:%.2d\nLatitude: -%d %d.%d\nLongitude: -%d %d.%d\nAltitude: ", hour, min, sec, latd, latm, lats, lond, lonm, lons);
		strcat(out, temp);
	}
	else
	{
		sprintf(out, "\nTime: %.2d:%.2d:%.2d\n", hour, min, sec);
		strcat(out, "NO GPS DATA AVAILABLE\n");
	}

	return;
}

void parse_dylos(char* in, char* out)
{
	int small, large;
	char temp[10];
	sscanf(in, "%d,%d", &small, &large);	
	strcat(out, "Small: ");
	sprintf(temp, "%d\n", small);
	strcat(out, temp);
	strcat(out, "Large: ");
	sprintf(temp,"%d\n", large);
	strcat(out, temp);
	return;
}

void strclr(char* s)
{	
	for(int i = strlen(s); i > 0; i--)
		s[i-1] = 0;
}

void init_GPS()
{
	usart_send(1,"$PGCMD,33,0*6D\r\n");
	usart_send(1,"$PMTK220,1000*1F\r\n");
	usart_send(1,"$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
	return;
}