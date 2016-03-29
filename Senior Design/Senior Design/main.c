/*
 * Senior Design.c
 *
 * Created: 2/27/2016 7:15:24 PM
 * Author : jmsikorski
 */ 
#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>

void read_temp(char*);
void read_MQ2(char*);
void read_MQ5(char*);
void read_MQ7(char*);
void usart_init(int);
void usart_send(int, char*);	
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
	DDRF = 0;
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
	long rs = 0;
	int adc = 0;
	int RL = 9800;
	int RO = 6600;
	int ppm = 0;
	ADCSRA = 0x87;
	ADMUX = 0x00; //Use ADC0
	ADCSRA |= (1<<ADSC);
	while((ADCSRA & (1<<ADIF))==0);
	adc = ADC;
	adc *= 5;
	rs = 5*1024;
	rs -= adc;
	rs *= RL;
	rs /= adc;
	rs *= 100;
	rs /= RO;
  if(rs < 27)
  {
	strcat(buffer, "ERROR: MQ2 reading out of range\n");
	return;
  }
  else
  {
	  switch(rs)
	  {
		  case	27	: ppm =	10478	;
		  break;
		  case	28	: ppm =	9706	;
		  break;
		  case	29	: ppm =	9015	;
		  break;
		  case	30	: ppm =	8394	;
		  break;
		  case	31	: ppm =	7834	;
		  break;
		  case	32	: ppm =	7327	;
		  break;
		  case	33	: ppm =	6868	;
		  break;
		  case	34	: ppm =	6449	;
		  break;
		  case	35	: ppm =	6068	;
		  break;
		  case	36	: ppm =	5718	;
		  break;
		  case	37	: ppm =	5398	;
		  break;
		  case	38	: ppm =	5103	;
		  break;
		  case	39	: ppm =	4831	;
		  break;
		  case	40	: ppm =	4581	;
		  break;
		  case	41	: ppm =	4349	;
		  break;
		  case	42	: ppm =	4133	;
		  break;
		  case	43	: ppm =	3934	;
		  break;
		  case	44	: ppm =	3748	;
		  break;
		  case	45	: ppm =	3575	;
		  break;
		  case	46	: ppm =	3413	;
		  break;
		  case	47	: ppm =	3262	;
		  break;
		  case	48	: ppm =	3121	;
		  break;
		  case	49	: ppm =	2988	;
		  break;
		  case	50	: ppm =	2864	;
		  break;
		  case	51	: ppm =	2747	;
		  break;
		  case	52	: ppm =	2637	;
		  break;
		  case	53	: ppm =	2533	;
		  break;
		  case	54	: ppm =	2435	;
		  break;
		  case	55	: ppm =	2343	;
		  break;
		  case	56	: ppm =	2256	;
		  break;
		  case	57	: ppm =	2173	;
		  break;
		  case	58	: ppm =	2095	;
		  break;
		  case	59	: ppm =	2021	;
		  break;
		  case	60	: ppm =	1951	;
		  break;
		  case	61	: ppm =	1884	;
		  break;
		  case	62	: ppm =	1821	;
		  break;
		  case	63	: ppm =	1760	;
		  break;
		  case	64	: ppm =	1703	;
		  break;
		  case	65	: ppm =	1648	;
		  break;
		  case	66	: ppm =	1596	;
		  break;
		  case	67	: ppm =	1546	;
		  break;
		  case	68	: ppm =	1499	;
		  break;
		  case	69	: ppm =	1454	;
		  break;
		  case	70	: ppm =	1410	;
		  break;
		  case	71	: ppm =	1369	;
		  break;
		  case	72	: ppm =	1329	;
		  break;
		  case	73	: ppm =	1291	;
		  break;
		  case	74	: ppm =	1254	;
		  break;
		  case	75	: ppm =	1220	;
		  break;
		  case	76	: ppm =	1186	;
		  break;
		  case	77	: ppm =	1154	;
		  break;
		  case	78	: ppm =	1123	;
		  break;
		  case	79	: ppm =	1093	;
		  break;
		  case	80	: ppm =	1065	;
		  break;
		  case	81	: ppm =	1037	;
		  break;
		  case	82	: ppm =	1011	;
		  break;
		  case	83	: ppm =	985	;
		  break;
		  case	84	: ppm =	961	;
		  break;
		  case	85	: ppm =	937	;
		  break;
		  case	86	: ppm =	914	;
		  break;
		  case	87	: ppm =	892	;
		  break;
		  case	88	: ppm =	871	;
		  break;
		  case	89	: ppm =	851	;
		  break;
		  case	90	: ppm =	831	;
		  break;
		  case	91	: ppm =	812	;
		  break;
		  case	92	: ppm =	793	;
		  break;
		  case	93	: ppm =	775	;
		  break;
		  case	94	: ppm =	758	;
		  break;
		  case	95	: ppm =	741	;
		  break;
		  case	96	: ppm =	725	;
		  break;
		  case	97	: ppm =	710	;
		  break;
		  case	98	: ppm =	694	;
		  break;
		  case	99	: ppm =	680	;
		  break;
		  case	100	: ppm =	666	;
		  break;
		  case	101	: ppm =	652	;
		  break;
		  case	102	: ppm =	638	;
		  break;
		  case	103	: ppm =	625	;
		  break;
		  case	104	: ppm =	613	;
		  break;
		  case	105	: ppm =	601	;
		  break;
		  case	106	: ppm =	589	;
		  break;
		  case	107	: ppm =	577	;
		  break;
		  case	108	: ppm =	566	;
		  break;
		  case	109	: ppm =	555	;
		  break;
		  case	110	: ppm =	545	;
		  break;
		  case	111	: ppm =	534	;
		  break;
		  case	112	: ppm =	524	;
		  break;
		  case	113	: ppm =	515	;
		  break;
		  case	114	: ppm =	505	;
		  break;
		  case	115	: ppm =	496	;
		  break;
		  case	116	: ppm =	487	;
		  break;
		  case	117	: ppm =	478	;
		  break;
		  case	118	: ppm =	470	;
		  break;
		  case	119	: ppm =	461	;
		  break;
		  case	120	: ppm =	453	;
		  break;
		  case	121	: ppm =	446	;
		  break;
		  case	122	: ppm =	438	;
		  break;
		  case	123	: ppm =	430	;
		  break;
		  case	124	: ppm =	423	;
		  break;
		  case	125	: ppm =	416	;
		  break;
		  case	126	: ppm =	409	;
		  break;
		  case	127	: ppm =	402	;
		  break;
		  case	128	: ppm =	396	;
		  break;
		  case	129	: ppm =	389	;
		  break;
		  case	130	: ppm =	383	;
		  break;
		  case	131	: ppm =	377	;
		  break;
		  case	132	: ppm =	371	;
		  break;
		  case	133	: ppm =	365	;
		  break;
		  case	134	: ppm =	359	;
		  break;
		  case	135	: ppm =	354	;
		  break;
		  case	136	: ppm =	348	;
		  break;
		  case	137	: ppm =	343	;
		  break;
		  case	138	: ppm =	338	;
		  break;
		  case	139	: ppm =	333	;
		  break;
		  case	140	: ppm =	328	;
		  break;
		  case	141	: ppm =	323	;
		  break;
		  case	142	: ppm =	318	;
		  break;
		  case	143	: ppm =	313	;
		  break;
		  case	144	: ppm =	309	;
		  break;
		  case	145	: ppm =	304	;
		  break;
		  case	146	: ppm =	300	;
		  break;
		  case	147	: ppm =	296	;
		  break;
		  case	148	: ppm =	292	;
		  break;
		  case	149	: ppm =	287	;
		  break;
		  case	150	: ppm =	283	;
		  break;
		  case	151	: ppm =	279	;
		  break;
		  case	152	: ppm =	276	;
		  break;
		  case	153	: ppm =	272	;
		  break;
		  case	154	: ppm =	268	;
		  break;
		  case	155	: ppm =	265	;
		  break;
		  case	156	: ppm =	261	;
		  break;
		  case	157	: ppm =	257	;
		  break;
		  case	158	: ppm =	254	;
		  break;
		  case	159	: ppm =	251	;
		  break;
		  case	160	: ppm =	247	;
		  break;
		  case	161	: ppm =	244	;
		  break;
		  case	162	: ppm =	241	;
		  break;
		  case	163	: ppm =	238	;
		  break;
		  case	164	: ppm =	235	;
		  break;
		  case	165	: ppm =	232	;
		  break;
		  case	166	: ppm =	229	;
		  break;
		  case	167	: ppm =	226	;
		  break;
		  case	168	: ppm =	223	;
		  break;
		  case	169	: ppm =	220	;
		  break;
		  case	170	: ppm =	218	;
		  break;
		  case	171	: ppm =	215	;
		  break;
		  case	172	: ppm =	212	;
		  break;
		  case	173	: ppm =	210	;
		  break;
		  case	174	: ppm =	207	;
		  break;
		  case	175	: ppm =	205	;
		  break;
		  case	176	: ppm =	202	;
		  break;
		  case	177	: ppm =	200	;
		  break;
		  default		: ppm = 0	;
		}
    }
	strcat(buffer, "C3H8: ");
	sprintf(temp, "%d ppm\n", (int)ppm);
	strcat(buffer, temp);
	return;
}

void read_MQ5(char* buffer) // CH4 sensor
{
	char temp[10];
	long rs = 0;
	int adc = 0;
	int RL = 19900;
	int RO = 15750;
	int ppm = 0;
	ADCSRA = 0x87;
	ADMUX = 0x01; //Use ADC1
	ADCSRA |= (1<<ADSC);
	while((ADCSRA & (1<<ADIF))==0);
	adc = ADC; // solve for Vout
	adc *= 5;
	rs = 5*1024;
	rs -= adc;
	rs *= RL;
	rs /= adc;
	rs *= 1000;
	rs /= RO;
	if(rs < 210)
	{
		strcat(buffer, "ERROR: MQ5 reading out of range\n");
		return;
	}
	else
	{
		switch(rs)
		{
		case	210	: ppm =	9867	;
		break;
		case	215	: ppm =	9291	;
		break;
		case	220	: ppm =	8760	;
		break;
		case	225	: ppm =	8271	;
		break;
		case	230	: ppm =	7819	;
		break;
		case	235	: ppm =	7400	;
		break;
		case	240	: ppm =	7012	;
		break;
		case	245	: ppm =	6652	;
		break;
		case	250	: ppm =	6317	;
		break;
		case	255	: ppm =	6005	;
		break;
		case	260	: ppm =	5714	;
		break;
		case	265	: ppm =	5443	;
		break;
		case	270	: ppm =	5189	;
		break;
		case	275	: ppm =	4951	;
		break;
		case	280	: ppm =	4728	;
		break;
		case	285	: ppm =	4518	;
		break;
		case	290	: ppm =	4322	;
		break;
		case	295	: ppm =	4137	;
		break;
		case	300	: ppm =	3963	;
		break;
		case	305	: ppm =	3799	;
		break;
		case	310	: ppm =	3644	;
		break;
		case	315	: ppm =	3498	;
		break;
		case	320	: ppm =	3360	;
		break;
		case	325	: ppm =	3229	;
		break;
		case	330	: ppm =	3106	;
		break;
		case	335	: ppm =	2988	;
		break;
		case	340	: ppm =	2877	;
		break;
		case	345	: ppm =	2772	;
		break;
		case	350	: ppm =	2672	;
		break;
		case	355	: ppm =	2577	;
		break;
		case	360	: ppm =	2486	;
		break;
		case	365	: ppm =	2400	;
		break;
		case	370	: ppm =	2318	;
		break;
		case	375	: ppm =	2240	;
		break;
		case	380	: ppm =	2165	;
		break;
		case	385	: ppm =	2094	;
		break;
		case	390	: ppm =	2026	;
		break;
		case	395	: ppm =	1961	;
		break;
		case	400	: ppm =	1899	;
		break;
		case	405	: ppm =	1839	;
		break;
		case	410	: ppm =	1783	;
		break;
		case	415	: ppm =	1728	;
		break;
		case	420	: ppm =	1676	;
		break;
		case	425	: ppm =	1626	;
		break;
		case	430	: ppm =	1578	;
		break;
		case	435	: ppm =	1532	;
		break;
		case	440	: ppm =	1488	;
		break;
		case	445	: ppm =	1446	;
		break;
		case	450	: ppm =	1405	;
		break;
		case	455	: ppm =	1366	;
		break;
		case	460	: ppm =	1328	;
		break;
		case	465	: ppm =	1292	;
		break;
		case	470	: ppm =	1257	;
		break;
		case	475	: ppm =	1223	;
		break;
		case	480	: ppm =	1191	;
		break;
		case	485	: ppm =	1160	;
		break;
		case	490	: ppm =	1130	;
		break;
		case	495	: ppm =	1101	;
		break;
		case	500	: ppm =	1073	;
		break;
		case	505	: ppm =	1046	;
		break;
		case	510	: ppm =	1020	;
		break;
		case	515	: ppm =	995	;
		break;
		case	520	: ppm =	971	;
		break;
		case	525	: ppm =	947	;
		break;
		case	530	: ppm =	924	;
		break;
		case	535	: ppm =	903	;
		break;
		case	540	: ppm =	881	;
		break;
		case	545	: ppm =	861	;
		break;
		case	550	: ppm =	841	;
		break;
		case	555	: ppm =	822	;
		break;
		case	560	: ppm =	803	;
		break;
		case	565	: ppm =	785	;
		break;
		case	570	: ppm =	768	;
		break;
		case	575	: ppm =	751	;
		break;
		case	580	: ppm =	734	;
		break;
		case	585	: ppm =	718	;
		break;
		case	590	: ppm =	703	;
		break;
		case	595	: ppm =	688	;
		break;
		case	600	: ppm =	673	;
		break;
		case	605	: ppm =	659	;
		break;
		case	610	: ppm =	645	;
		break;
		case	615	: ppm =	632	;
		break;
		case	620	: ppm =	619	;
		break;
		case	625	: ppm =	606	;
		break;
		case	630	: ppm =	594	;
		break;
		case	635	: ppm =	582	;
		break;
		case	640	: ppm =	571	;
		break;
		case	645	: ppm =	559	;
		break;
		case	650	: ppm =	549	;
		break;
		case	655	: ppm =	538	;
		break;
		case	660	: ppm =	528	;
		break;
		case	665	: ppm =	517	;
		break;
		case	670	: ppm =	508	;
		break;
		case	675	: ppm =	498	;
		break;
		case	680	: ppm =	489	;
		break;
		case	685	: ppm =	480	;
		break;
		case	690	: ppm =	471	;
		break;
		case	695	: ppm =	462	;
		break;
		case	700	: ppm =	454	;
		break;
		case	705	: ppm =	446	;
		break;
		case	710	: ppm =	438	;
		break;
		case	715	: ppm =	430	;
		break;
		case	720	: ppm =	422	;
		break;
		case	725	: ppm =	415	;
		break;
		case	730	: ppm =	408	;
		break;
		case	735	: ppm =	401	;
		break;
		case	740	: ppm =	394	;
		break;
		case	745	: ppm =	387	;
		break;
		case	750	: ppm =	380	;
		break;
		case	755	: ppm =	374	;
		break;
		case	760	: ppm =	368	;
		break;
		case	765	: ppm =	362	;
		break;
		case	770	: ppm =	356	;
		break;
		case	775	: ppm =	350	;
		break;
		case	780	: ppm =	344	;
		break;
		case	785	: ppm =	339	;
		break;
		case	790	: ppm =	333	;
		break;
		case	795	: ppm =	328	;
		break;
		case	800	: ppm =	323	;
		break;
		case	805	: ppm =	317	;
		break;
		case	810	: ppm =	312	;
		break;
		case	815	: ppm =	308	;
		break;
		case	820	: ppm =	303	;
		break;
		case	825	: ppm =	298	;
		break;
		case	830	: ppm =	294	;
		break;
		case	835	: ppm =	289	;
		break;
		case	840	: ppm =	285	;
		break;
		case	845	: ppm =	280	;
		break;
		case	850	: ppm =	276	;
		break;
		case	855	: ppm =	272	;
		break;
		case	860	: ppm =	268	;
		break;
		case	865	: ppm =	264	;
		break;
		case	870	: ppm =	260	;
		break;
		case	875	: ppm =	256	;
		break;
		case	880	: ppm =	253	;
		break;
		case	885	: ppm =	249	;
		break;
		case	890	: ppm =	246	;
		break;
		case	895	: ppm =	242	;
		break;
		case	900	: ppm =	239	;
		break;
		case	905	: ppm =	235	;
		break;
		case	910	: ppm =	232	;
		break;
		case	915	: ppm =	229	;
		break;
		case	920	: ppm =	226	;
		break;
		case	925	: ppm =	222	;
		break;
		case	930	: ppm =	219	;
		break;
		case	935	: ppm =	216	;
		break;
		case	940	: ppm =	214	;
		break;
		case	945	: ppm =	211	;
		break;
		case	950	: ppm =	208	;
		break;
		case	955	: ppm =	205	;
		break;
		case	960	: ppm =	202	;
		break;
		default		: ppm = 0	;
		}
	}
	strcat(buffer, "CH4 : ");
	sprintf(temp, "%d ppm\n", (int)ppm);
	strcat(buffer, temp);
	return;
}

void read_MQ7(char* buffer) //C0 sensor
{
	char temp[10];
	long rs = 0;
	int adc = 0;
	int RL = 9900;
	int RO = 2000;
	int ppm = 0;
	ADCSRA = 0x87;
	ADMUX = 0x02; //Use ADC2
	ADCSRA |= (1<<ADSC);
	while((ADCSRA & (1<<ADIF))==0);
	adc = ADC; // solve for Vout
	adc *= 5;
	rs = 5*1024;
	rs -= adc;
	rs *= RL;
	rs /= adc;
	rs *= 100;
	rs /= RO;
	if(rs < 9)
	{
		strcat(buffer, "ERROR: MQ7 reading out of range\n");
		return;
	}
	else
	{
		switch(rs)
		{
		  case	9	: ppm =	4112	;
		  break;
		  case	10	: ppm =	3495	;
		  break;
		  case	11	: ppm =	3017	;
		  break;
		  case	12	: ppm =	2638	;
		  break;
		  case	13	: ppm =	2332	;
		  break;
		  case	14	: ppm =	2080	;
		  break;
		  case	15	: ppm =	1870	;
		  break;
		  case	16	: ppm =	1692	;
		  break;
		  case	17	: ppm =	1541	;
		  break;
		  case	18	: ppm =	1411	;
		  break;
		  case	19	: ppm =	1298	;
		  break;
		  case	20	: ppm =	1199	;
		  break;
		  case	21	: ppm =	1112	;
		  break;
		  case	22	: ppm =	1035	;
		  break;
		  case	23	: ppm =	967	;
		  break;
		  case	24	: ppm =	905	;
		  break;
		  case	25	: ppm =	850	;
		  break;
		  case	26	: ppm =	800	;
		  break;
		  case	27	: ppm =	755	;
		  break;
		  case	28	: ppm =	714	;
		  break;
		  case	29	: ppm =	676	;
		  break;
		  case	30	: ppm =	641	;
		  break;
		  case	31	: ppm =	610	;
		  break;
		  case	32	: ppm =	581	;
		  break;
		  case	33	: ppm =	554	;
		  break;
		  case	34	: ppm =	529	;
		  break;
		  case	35	: ppm =	506	;
		  break;
		  case	36	: ppm =	484	;
		  break;
		  case	37	: ppm =	464	;
		  break;
		  case	38	: ppm =	445	;
		  break;
		  case	39	: ppm =	428	;
		  break;
		  case	40	: ppm =	412	;
		  break;
		  case	41	: ppm =	396	;
		  break;
		  case	42	: ppm =	382	;
		  break;
		  case	43	: ppm =	368	;
		  break;
		  case	44	: ppm =	355	;
		  break;
		  case	45	: ppm =	343	;
		  break;
		  case	46	: ppm =	332	;
		  break;
		  case	47	: ppm =	321	;
		  break;
		  case	48	: ppm =	311	;
		  break;
		  case	49	: ppm =	301	;
		  break;
		  case	50	: ppm =	292	;
		  break;
		  case	51	: ppm =	283	;
		  break;
		  case	52	: ppm =	275	;
		  break;
		  case	53	: ppm =	267	;
		  break;
		  case	54	: ppm =	259	;
		  break;
		  case	55	: ppm =	252	;
		  break;
		  case	56	: ppm =	245	;
		  break;
		  case	57	: ppm =	238	;
		  break;
		  case	58	: ppm =	232	;
		  break;
		  case	59	: ppm =	226	;
		  break;
		  case	60	: ppm =	220	;
		  break;
		  case	61	: ppm =	215	;
		  break;
		  case	62	: ppm =	209	;
		  break;
		  case	63	: ppm =	204	;
		  break;
		  case	64	: ppm =	199	;
		  break;
		  case	65	: ppm =	195	;
		  break;
		  case	66	: ppm =	190	;
		  break;
		  case	67	: ppm =	186	;
		  break;
		  case	68	: ppm =	181	;
		  break;
		  case	69	: ppm =	177	;
		  break;
		  case	70	: ppm =	174	;
		  break;
		  case	71	: ppm =	170	;
		  break;
		  case	72	: ppm =	166	;
		  break;
		  case	73	: ppm =	163	;
		  break;
		  case	74	: ppm =	159	;
		  break;
		  case	75	: ppm =	156	;
		  break;
		  case	76	: ppm =	153	;
		  break;
		  case	77	: ppm =	150	;
		  break;
		  case	78	: ppm =	147	;
		  break;
		  case	79	: ppm =	144	;
		  break;
		  case	80	: ppm =	141	;
		  break;
		  case	81	: ppm =	139	;
		  break;
		  case	82	: ppm =	136	;
		  break;
		  case	83	: ppm =	133	;
		  break;
		  case	84	: ppm =	131	;
		  break;
		  case	85	: ppm =	129	;
		  break;
		  case	86	: ppm =	126	;
		  break;
		  case	87	: ppm =	124	;
		  break;
		  case	88	: ppm =	122	;
		  break;
		  case	89	: ppm =	120	;
		  break;
		  case	90	: ppm =	118	;
		  break;
		  case	91	: ppm =	116	;
		  break;
		  case	92	: ppm =	114	;
		  break;
		  case	93	: ppm =	112	;
		  break;
		  case	94	: ppm =	110	;
		  break;
		  case	95	: ppm =	108	;
		  break;
		  case	96	: ppm =	107	;
		  break;
		  case	97	: ppm =	105	;
		  break;
		  case	98	: ppm =	103	;
		  break;
		  case	99	: ppm =	102	;
		  break;
		  case	100	: ppm =	100	;
		  break;
		  case	101	: ppm =	99	;
		  break;
		  case	102	: ppm =	97	;
		  break;
		  case	103	: ppm =	96	;
		  break;
		  case	104	: ppm =	94	;
		  break;
		  case	105	: ppm =	93	;
		  break;
		  case	106	: ppm =	91	;
		  break;
		  case	107	: ppm =	90	;
		  break;
		  case	108	: ppm =	89	;
		  break;
		  case	109	: ppm =	88	;
		  break;
		  case	110	: ppm =	86	;
		  break;
		  case	111	: ppm =	85	;
		  break;
		  case	112	: ppm =	84	;
		  break;
		  case	113	: ppm =	83	;
		  break;
		  case	114	: ppm =	82	;
		  break;
		  case	115	: ppm =	81	;
		  break;
		  case	116	: ppm =	80	;
		  break;
		  case	117	: ppm =	79	;
		  break;
		  case	118	: ppm =	78	;
		  break;
		  case	119	: ppm =	77	;
		  break;
		  case	120	: ppm =	76	;
		  break;
		  case	121	: ppm =	75	;
		  break;
		  case	122	: ppm =	74	;
		  break;
		  case	123	: ppm =	73	;
		  break;
		  case	124	: ppm =	72	;
		  break;
		  case	125	: ppm =	71	;
		  break;
		  case	126	: ppm =	70	;
		  break;
		  case	127	: ppm =	69	;
		  break;
		  case	128	: ppm =	68	;
		  break;
		  case	129	: ppm =	68	;
		  break;
		  case	130	: ppm =	67	;
		  break;
		  case	131	: ppm =	66	;
		  break;
		  case	132	: ppm =	65	;
		  break;
		  case	133	: ppm =	64	;
		  break;
		  case	134	: ppm =	64	;
		  break;
		  case	135	: ppm =	63	;
		  break;
		  case	136	: ppm =	62	;
		  break;
		  case	137	: ppm =	62	;
		  break;
		  case	138	: ppm =	61	;
		  break;
		  case	139	: ppm =	60	;
		  break;
		  case	140	: ppm =	60	;
		  break;
		  case	141	: ppm =	59	;
		  break;
		  case	142	: ppm =	58	;
		  break;
		  case	143	: ppm =	58	;
		  break;
		  case	144	: ppm =	57	;
		  break;
		  case	145	: ppm =	56	;
		  break;
		  case	146	: ppm =	56	;
		  break;
		  case	147	: ppm =	55	;
		  break;
		  case	148	: ppm =	55	;
		  break;
		  case	149	: ppm =	54	;
		  break;
		  case	150	: ppm =	54	;
		  break;
		  case	151	: ppm =	53	;
		  break;
		  case	152	: ppm =	52	;
		  break;
		  case	153	: ppm =	52	;
		  break;
		  case	154	: ppm =	51	;
		  break;
		  case	155	: ppm =	51	;
		  break;
		  case	156	: ppm =	50	;
		  break;
		  case	157	: ppm =	50	;
		  break;
		  case	158	: ppm =	49	;
		  break;
		  case	159	: ppm =	49	;
		  break;
		  default		: ppm = 0	;
		}
	}
	strcat(buffer, "CO  : ");
	sprintf(temp, "%d ppm\n", (int)ppm);
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
			if(in[p] == ',')
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