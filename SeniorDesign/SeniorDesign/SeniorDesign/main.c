/*
 * Senior Design.c
 *
 * Created: 2/27/2016 7:15:24 PM
 * Author : jmsikorski
 */ 
//#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
#include "ff.h"
//#include "diskio.h"

int read_temp();
int read_MQ2();
int read_MQ5();
int read_MQ7();
void usart_init(int);
void usart_send(int, char*);	
void parse_GPS(char*, char*);
void parse_dylos(char*, char*);
void strclr(char*);
void init_GPS();
FATFS FatFs;
FIL Fil;

char volatile rec_dylos[10];
char volatile rec_dylos_flag = 0;
char volatile rec_GPS[100];
char volatile rec_GPS_flag = 0;
char volatile ov_flag = 0;

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

ISR(TIMER1_OVF_vect)
{
	ov_flag = 1;
	TCNT1 = 57723;
}

int main(void)
{
	DDRF = 0;
	DDRK = 0xFF;
	char GPS[100];
	char dylos[10];
	char GPS_line = 0;
	char dylos_line = 0;
	char GPS_parsed[100];
	char dylos_parsed[30];
	char buffer[150];
	int C3H8, CH4, CO, H2S, F;
	char new_GPS_data = 0;
	char new_dylos_data = 0;
	UINT byte_width;
	FRESULT fr;
	TCNT1 = 57723;
	TCCR1B = (1<<CS12) | (1<<CS10);
	TIMSK1 = (1<<TOIE1);
	usart_init(0);
	usart_init(1);
	usart_init(2);
	usart_init(3);
	init_GPS();

	UINT bw;
	DWORD siz;
	
	fr = f_mount(&FatFs, "", 0);		/* Give a work area to the default drive */

	sei();

    while (1) 
	{
		if(rec_dylos_flag == 1)
		{
			rec_dylos_flag = 0;
			new_dylos_data = 1;
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
			new_GPS_data = 1;
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
		}
	
		if(ov_flag == 1)
		{
			ov_flag = 0;
			if(new_GPS_data == 1)
			{
				usart_send(3, GPS_parsed);
				fr = f_open(&Fil, "dronedat.txt", FA_WRITE | FA_OPEN_ALWAYS);	/* Create a file */
				f_lseek(&Fil, f_size(&Fil));
				f_write(&Fil, GPS_parsed, strlen(GPS), &byte_width);
				f_close(&Fil);								/* Close the file */
				new_GPS_data = 0;
			}
			if(new_dylos_data == 1)
			{
				usart_send(3, dylos_parsed);
				new_dylos_data = 0;
			}
			F = read_temp();
			sprintf(buffer, "Temperature %dF\n\r", F);
			usart_send(3, buffer);
			C3H8 = read_MQ2();
			if(C3H8 < 0)
			usart_send(3, "C3H8: ERROR - MQ2 reading out of range\n\r");
			else
			{
				sprintf(buffer, "C3H8: %d ppm \n\r",C3H8);
				usart_send(3, buffer);
			}
			CH4 = read_MQ5();
			if(CH4 < 0)
			usart_send(3, "CH4: ERROR - MQ5 reading out of range\n\r");
			else
			{
				sprintf(buffer, "CH4: %d ppm \n\r",CH4);
				usart_send(3, buffer);
			}
			CO = read_MQ7();
			if(CO < 0)
			usart_send(3, "CO: ERROR - MQ7 reading out of range\n\r");
			else
			{
				sprintf(buffer, "CO: %d ppm \n\r",CO);
				usart_send(3, buffer);
			}			
		}
	}
	return 0;
}

int read_temp()
{
	ADCSRA = 0x87;
	ADMUX = 0x04; //Use ADC4
	ADCSRA |= (1<<ADSC);
	while((ADCSRA & (1<<ADIF))==0);
	double t = ADC;	
	t *= 225;
	t /= 256;
	t -= 58;
	return t;
}

int read_MQ2() //C3H8 sensor
{
	long rs = -1;
	int adc = 0;
	int RL = 9800;
	int RO = 6600;
	int ppm[151];
	ppm[0] =	10478;
	ppm[1] =	9706;
	ppm[2] =	9015;
	ppm[3] =	8394;
	ppm[4] =	7834;
	ppm[5] =	7327;
	ppm[6] =	6868;
	ppm[7] =	6449;
	ppm[8] =	6068;
	ppm[9] =	5718;
	ppm[10] =	5398;
	ppm[11] =	5103;
	ppm[12] =	4831;
	ppm[13] =	4581;
	ppm[14] =	4349;
	ppm[15] =	4133;
	ppm[16] =	3934;
	ppm[17] =	3748;
	ppm[18] =	3575;
	ppm[19] =	3413;
	ppm[20] =	3262;
	ppm[21] =	3121;
	ppm[22] =	2988;
	ppm[23] =	2864;
	ppm[24] =	2747;
	ppm[25] =	2637;
	ppm[26] =	2533;
	ppm[27] =	2435;
	ppm[28] =	2343;
	ppm[29] =	2256;
	ppm[30] =	2173;
	ppm[31] =	2095;
	ppm[32] =	2021;
	ppm[33] =	1951;
	ppm[34] =	1884;
	ppm[35] =	1821;
	ppm[36] =	1760;
	ppm[37] =	1703;
	ppm[38] =	1648;
	ppm[39] =	1596;
	ppm[40] =	1546;
	ppm[41] =	1499;
	ppm[42] =	1454;
	ppm[43] =	1410;
	ppm[44] =	1369;
	ppm[45] =	1329;
	ppm[46] =	1291;
	ppm[47] =	1254;
	ppm[48] =	1220;
	ppm[49] =	1186;
	ppm[50] =	1154;
	ppm[51] =	1123;
	ppm[52] =	1093;
	ppm[53] =	1065;
	ppm[54] =	1037;
	ppm[55] =	1011;
	ppm[56] =	985;
	ppm[57] =	961;
	ppm[58] =	937;
	ppm[59] =	914;
	ppm[60] =	892;
	ppm[61] =	871;
	ppm[62] =	851;
	ppm[63] =	831;
	ppm[64] =	812;
	ppm[65] =	793;
	ppm[66] =	775;
	ppm[67] =	758;
	ppm[68] =	741;
	ppm[69] =	725;
	ppm[70] =	710;
	ppm[71] =	694;
	ppm[72] =	680;
	ppm[73] =	667;
	ppm[74] =	652;
	ppm[75] =	638;
	ppm[76] =	625;
	ppm[77] =	613;
	ppm[78] =	601;
	ppm[79] =	589;
	ppm[80] =	577;
	ppm[81] =	566;
	ppm[82] =	555;
	ppm[83] =	545;
	ppm[84] =	534;
	ppm[85] =	524;
	ppm[86] =	515;
	ppm[87] =	505;
	ppm[88] =	496;
	ppm[89] =	487;
	ppm[90] =	478;
	ppm[91] =	470;
	ppm[92] =	461;
	ppm[93] =	453;
	ppm[94] =	446;
	ppm[95] =	438;
	ppm[96] =	430;
	ppm[97] =	423;
	ppm[98] =	416;
	ppm[99] =	409;
	ppm[100] =	402;
	ppm[101] =	396;
	ppm[102] =	389;
	ppm[103] =	383;
	ppm[104] =	377;
	ppm[105] =	371;
	ppm[106] =	365;
	ppm[107] =	359;
	ppm[108] =	354;
	ppm[109] =	348;
	ppm[110] =	343;
	ppm[111] =	338;
	ppm[112] =	333;
	ppm[113] =	328;
	ppm[114] =	323;
	ppm[115] =	318;
	ppm[116] =	313;
	ppm[117] =	309;
	ppm[118] =	304;
	ppm[119] =	300;
	ppm[120] =	296;
	ppm[121] =	292;
	ppm[122] =	287;
	ppm[123] =	283;
	ppm[124] =	279;
	ppm[125] =	276;
	ppm[126] =	272;
	ppm[127] =	268;
	ppm[128] =	265;
	ppm[129] =	261;
	ppm[130] =	257;
	ppm[131] =	254;
	ppm[132] =	251;
	ppm[133] =	247;
	ppm[134] =	244;
	ppm[135] =	241;
	ppm[136] =	238;
	ppm[137] =	235;
	ppm[138] =	232;
	ppm[139] =	229;
	ppm[140] =	226;
	ppm[141] =	223;
	ppm[142] =	220;
	ppm[143] =	218;
	ppm[144] =	215;
	ppm[145] =	212;
	ppm[146] =	210;
	ppm[147] =	207;
	ppm[148] =	205;
	ppm[149] =	202;
	ppm[150] =	200;
	ADCSRA = 0x87;
	ADMUX = 0x00; //Use ADC0
	ADCSRA |= (1<<ADSC);
	while((ADCSRA & (1<<ADIF))==0);
	adc = ADC;
	if(adc != 0)
	{
		adc *= 5;
		rs = 5*1024;
		rs -= adc;
		rs *= RL;
		rs /= adc;
		rs *= 100;
		rs /= RO;
		rs -= 27;
	}
	if(rs < 0)
		return -1;
	else if(rs > 150)
		return 0;
	else
		return ppm[rs];
}

int read_MQ5() // CH4 sensor
{
	long rs = -1;
	int adc = 0;
	int RL = 19900;
	int RO = 15750;
	int ppm[151];
	ppm[0] =	9867;
	ppm[1] =	9291;
	ppm[2] =	8760;
	ppm[3] =	8271;
	ppm[4] =	7819;
	ppm[5] =	7400;
	ppm[6] =	7012;
	ppm[7] =	6652;
	ppm[8] =	6317;
	ppm[9] =	6005;
	ppm[10] =	5714;
	ppm[11] =	5443;
	ppm[12] =	5189;
	ppm[13] =	4951;
	ppm[14] =	4728;
	ppm[15] =	4518;
	ppm[16] =	4322;
	ppm[17] =	4137;
	ppm[18] =	3963;
	ppm[19] =	3799;
	ppm[20] =	3644;
	ppm[21] =	3498;
	ppm[22] =	3360;
	ppm[23] =	3229;
	ppm[24] =	3106;
	ppm[25] =	2988;
	ppm[26] =	2772;
	ppm[27] =	2672;
	ppm[28] =	2577;
	ppm[29] =	2486;
	ppm[30] =	2400;
	ppm[31] =	2318;
	ppm[32] =	2240;
	ppm[33] =	2165;
	ppm[34] =	2094;
	ppm[35] =	2026;
	ppm[36] =	1961;
	ppm[37] =	1899;
	ppm[38] =	1839;
	ppm[39] =	1783;
	ppm[40] =	1728;
	ppm[41] =	1676;
	ppm[42] =	1626;
	ppm[43] =	1578;
	ppm[44] =	1532;
	ppm[45] =	1488;
	ppm[46] =	1446;
	ppm[47] =	1405;
	ppm[48] =	1366;
	ppm[49] =	1328;
	ppm[50] =	1292;
	ppm[51] =	1257;
	ppm[52] =	1223;
	ppm[53] =	1191;
	ppm[54] =	1160;
	ppm[55] =	1130;
	ppm[56] =	1101;
	ppm[57] =	1073;
	ppm[58] =	1046;
	ppm[59] =	1020;
	ppm[60] =	995;
	ppm[61] =	971;
	ppm[62] =	947;
	ppm[63] =	924;
	ppm[64] =	903;
	ppm[65] =	881;
	ppm[66] =	861;
	ppm[67] =	841;
	ppm[68] =	822;
	ppm[69] =	803;
	ppm[70] =	785;
	ppm[71] =	768;
	ppm[72] =	751;
	ppm[73] =	734;
	ppm[74] =	718;
	ppm[75] =	703;
	ppm[76] =	688;
	ppm[77] =	673;
	ppm[78] =	659;
	ppm[79] =	645;
	ppm[80] =	632;
	ppm[81] =	619;
	ppm[82] =	606;
	ppm[83] =	594;
	ppm[84] =	582;
	ppm[85] =	571;
	ppm[86] =	559;
	ppm[87] =	549;
	ppm[88] =	538;
	ppm[89] =	528;
	ppm[90] =	517;
	ppm[91] =	508;
	ppm[92] =	498;
	ppm[93] =	489;
	ppm[94] =	480;
	ppm[95] =	471;
	ppm[96] =	462;
	ppm[97] =	454;
	ppm[98] =	446;
	ppm[99] =	438;
	ppm[100] =	430;
	ppm[101] =	422;
	ppm[102] =	415;
	ppm[103] =	408;
	ppm[104] =	401;
	ppm[105] =	394;
	ppm[106] =	387;
	ppm[107] =	380;
	ppm[108] =	374;
	ppm[109] =	368;
	ppm[110] =	362;
	ppm[111] =	356;
	ppm[112] =	350;
	ppm[113] =	344;
	ppm[114] =	339;
	ppm[115] =	333;
	ppm[116] =	328;
	ppm[117] =	323;
	ppm[118] =	317;
	ppm[119] =	312;
	ppm[120] =	308;
	ppm[121] =	303;
	ppm[122] =	298;
	ppm[123] =	294;
	ppm[124] =	289;
	ppm[125] =	285;
	ppm[126] =	280;
	ppm[127] =	276;
	ppm[128] =	272;
	ppm[129] =	268;
	ppm[130] =	264;
	ppm[131] =	260;
	ppm[132] =	256;
	ppm[133] =	253;
	ppm[134] =	249;
	ppm[135] =	246;
	ppm[136] =	242;
	ppm[137] =	239;
	ppm[138] =	235;
	ppm[139] =	232;
	ppm[140] =	229;
	ppm[141] =	226;
	ppm[142] =	222;
	ppm[143] =	219;
	ppm[144] =	216;
	ppm[145] =	214;
	ppm[146] =	211;
	ppm[147] =	208;
	ppm[148] =	205;
	ppm[149] =	202;
	ppm[150] =	200;
	ADCSRA = 0x87;
	ADMUX = 0x01; //Use ADC1
	ADCSRA |= (1<<ADSC);
	while((ADCSRA & (1<<ADIF))==0);
	adc = ADC; // solve for Vout
	if(adc != 0)
	{
		adc *= 5;
		rs = 5*1024;
		rs -= adc;
		rs *= RL;
		rs /= adc;
		rs *= 1000;
		rs /= RO;
		rs /= 5;
		rs -= 42;
	}
	if(rs < 0)
		return -1;
	else if(rs > 150)
		return 0;
	else
		return ppm[rs];
}

int read_MQ7() //C0 sensor
{
	long rs = -1;
	int adc = 0;
	int RL = 9900;
	int RO = 2000;
	int ppm[151];
	ppm[0] = 4112;
	ppm[1] = 3495;
	ppm[2] = 3017;
	ppm[3] = 2638;
	ppm[4] = 2332;
	ppm[5] = 2080;
	ppm[6] = 1870;
	ppm[7] = 1692;
	ppm[8] = 1541;
	ppm[9] = 1411;
	ppm[10] = 1298;
	ppm[11] = 1199;
	ppm[12] = 1112;
	ppm[13] = 1035;
	ppm[14] = 967;
	ppm[15] = 905;
	ppm[16] = 850;
	ppm[17] = 800;
	ppm[18] = 755;
	ppm[19] = 714;
	ppm[20] = 676;
	ppm[21] = 641;
	ppm[22] = 610;
	ppm[23] = 581;
	ppm[24] = 554;
	ppm[25] = 529;
	ppm[26] = 506;
	ppm[27] = 484;
	ppm[28] = 464;
	ppm[29] = 445;
	ppm[30] = 428;
	ppm[31] = 412;
	ppm[32] = 396;
	ppm[33] = 382;
	ppm[34] = 368;
	ppm[35] = 355;
	ppm[36] = 343;
	ppm[37] = 332;
	ppm[38] = 321;
	ppm[39] = 311;
	ppm[40] = 301;
	ppm[41] = 292;
	ppm[42] = 283;
	ppm[43] = 275;
	ppm[44] = 267;
	ppm[45] = 259;
	ppm[46] = 252;
	ppm[47] = 245;
	ppm[48] = 238;
	ppm[49] = 232;
	ppm[50] = 226;
	ppm[51] = 220;
	ppm[52] = 215;
	ppm[53] = 209;
	ppm[54] = 204;
	ppm[55] = 199;
	ppm[56] = 195;
	ppm[57] = 190;
	ppm[58] = 186;
	ppm[59] = 181;
	ppm[60] = 177;
	ppm[61] = 174;
	ppm[62] = 170;
	ppm[63] = 166;
	ppm[64] = 163;
	ppm[65] = 159;
	ppm[66] = 156;
	ppm[67] = 153;
	ppm[68] = 150;
	ppm[69] = 147;
	ppm[70] = 144;
	ppm[71] = 141;
	ppm[72] = 139;
	ppm[73] = 136;
	ppm[74] = 133;
	ppm[75] = 131;
	ppm[76] = 129;
	ppm[77] = 126;
	ppm[78] = 124;
	ppm[79] = 122;
	ppm[80] = 120;
	ppm[81] = 118;
	ppm[82] = 116;
	ppm[83] = 114;
	ppm[84] = 112;
	ppm[85] = 110;
	ppm[86] = 108;
	ppm[87] = 107;
	ppm[88] = 105;
	ppm[89] = 103;
	ppm[90] = 102;
	ppm[91] = 100;
	ppm[92] = 99;
	ppm[93] = 97;
	ppm[94] = 96;
	ppm[95] = 94;
	ppm[96] = 93;
	ppm[97] = 91;
	ppm[98] = 90;
	ppm[99] = 89;
	ppm[100] = 88;
	ppm[101] = 86;
	ppm[102] = 85;
	ppm[103] = 84;
	ppm[104] = 83;
	ppm[105] = 82;
	ppm[106] = 81;
	ppm[107] = 80;
	ppm[108] = 79;
	ppm[109] = 78;
	ppm[110] = 77;
	ppm[111] = 76;
	ppm[112] = 75;
	ppm[113] = 74;
	ppm[114] = 73;
	ppm[115] = 72;
	ppm[116] = 71;
	ppm[117] = 70;
	ppm[118] = 69;
	ppm[119] = 68;
	ppm[120] = 68;
	ppm[121] = 67;
	ppm[122] = 66;
	ppm[123] = 65;
	ppm[124] = 64;
	ppm[125] = 64;
	ppm[126] = 63;
	ppm[127] = 62;
	ppm[128] = 62;
	ppm[129] = 61;
	ppm[130] = 60;
	ppm[131] = 60;
	ppm[132] = 59;
	ppm[133] = 58;
	ppm[134] = 58;
	ppm[135] = 57;
	ppm[136] = 56;
	ppm[137] = 56;
	ppm[138] = 55;
	ppm[139] = 55;
	ppm[140] = 54;
	ppm[141] = 54;
	ppm[142] = 53;
	ppm[143] = 52;
	ppm[144] = 52;
	ppm[145] = 51;
	ppm[146] = 51;
	ppm[147] = 50;
	ppm[148] = 50;
	ppm[149] = 49;
	ppm[150] = 49;
	ADCSRA = 0x87;
	ADMUX = 0x02; //Use ADC2
	ADCSRA |= (1<<ADSC);
	while((ADCSRA & (1<<ADIF))==0);
	adc = ADC; // solve for Vout
	if(adc != 0)
	{
		adc *= 5;
		rs = 5*1024;
		rs -= adc;
		rs *= RL;
		rs /= adc;
		rs *= 100;
		rs /= RO;
		rs-= 9;
	}
	if(rs < 0)
		return -1;
	else if(rs > 150)
		return 0;
	else
		return ppm[rs];
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
			if(in[p] == ',')
			{
				comma++;
				p++;
				switch(comma)
				{
					case 1:
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
						temp[i++] = '\r';
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
			sprintf(out, "\n\rTime: %.2d:%.2d:%.2d\n\rLatitude: %d %d.%d\n\rLongitude: %d %d.%d\n\rAltitude: ", hour, min, sec, latd, latm, lats, lond, lonm, lons);
		else if(ns == 1 && ew == 0)
			sprintf(out, "\n\rTime: %.2d:%.2d:%.2d\n\rLatitude: -%d %d.%d\n\rLongitude: %d %d.%d\n\rAltitude: ", hour, min, sec, latd, latm, lats, lond, lonm, lons);
		else if(ns == 0 && ew == 1)
			sprintf(out, "\n\rTime: %.2d:%.2d:%.2d\n\rLatitude: %d %d.%d\n\rLongitude: -%d %d.%d\n\rAltitude: ", hour, min, sec, latd, latm, lats, lond, lonm, lons);
		else if(ns == 1 && ew == 1)
			sprintf(out, "\n\rTime: %.2d:%.2d:%.2d\n\rLatitude: -%d %d.%d\n\rLongitude: -%d %d.%d\n\rAltitude: ", hour, min, sec, latd, latm, lats, lond, lonm, lons);
		strcat(out, temp);
	}
	else
	{
		sprintf(out, "\n\rTime: %.2d:%.2d:%.2d\n\r", hour, min, sec);
		strcat(out, "NO GPS DATA AVAILABLE\n\r");
	}

	return;
}

void parse_dylos(char* in, char* out)
{
	int small, large;
	char temp[10];
	sscanf(in, "%d,%d", &small, &large);	
	strcat(out, "Small: ");
	sprintf(temp, "%d\n\r", small);
	strcat(out, temp);
	strcat(out, "Large: ");
	sprintf(temp,"%d\n\r", large);
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