/*
 * Senior Design.c
 *
 * Created: 2/27/2016 7:15:24 PM
 * Author : jmsikorski
 */ 
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "ff.h"

void sd_write(char*, FIL*, char*);
int read_temp();
int read_MQ2();
int read_MQ5();
int read_MQ7();
int read_MQ136();
void usart_init(int);
void usart_send(int, char*);	
void parse_GPS(char*, int*);
void parse_dylos(char*, int*);
void strclr(char*);
void init_GPS();
FATFS FatFs;
FIL Fil;

char volatile rec_dylos[10];
char volatile rec_dylos_flag = 0;
char volatile rec_GPS[100];
char volatile rec_GPS_flag = 0;
char volatile ov_flag = 0;
char fil_nm[15] = {0};

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
	char GPS[100];
	char dylos[10];
	char GPS_line = 0;
	char dylos_line = 0;
	char buffer[150];
	int GPS_data[15];
	int dylos_data[2];
	int C3H8, CH4, CO, H2S, F;
	char new_GPS_data = 0;
	char new_dylos_data = 0;

	int ppm2[151];
	ppm2[0] =	10478;
	ppm2[1] =	9706;
	ppm2[2] =	9015;
	ppm2[3] =	8394;
	ppm2[4] =	7834;
	ppm2[5] =	7327;
	ppm2[6] =	6868;
	ppm2[7] =	6449;
	ppm2[8] =	6068;
	ppm2[9] =	5718;
	ppm2[10] =	5398;
	ppm2[11] =	5103;
	ppm2[12] =	4831;
	ppm2[13] =	4581;
	ppm2[14] =	4349;
	ppm2[15] =	4133;
	ppm2[16] =	3934;
	ppm2[17] =	3748;
	ppm2[18] =	3575;
	ppm2[19] =	3413;
	ppm2[20] =	3262;
	ppm2[21] =	3121;
	ppm2[22] =	2988;
	ppm2[23] =	2864;
	ppm2[24] =	2747;
	ppm2[25] =	2637;
	ppm2[26] =	2533;
	ppm2[27] =	2435;
	ppm2[28] =	2343;
	ppm2[29] =	2256;
	ppm2[30] =	2173;
	ppm2[31] =	2095;
	ppm2[32] =	2021;
	ppm2[33] =	1951;
	ppm2[34] =	1884;
	ppm2[35] =	1821;
	ppm2[36] =	1760;
	ppm2[37] =	1703;
	ppm2[38] =	1648;
	ppm2[39] =	1596;
	ppm2[40] =	1546;
	ppm2[41] =	1499;
	ppm2[42] =	1454;
	ppm2[43] =	1410;
	ppm2[44] =	1369;
	ppm2[45] =	1329;
	ppm2[46] =	1291;
	ppm2[47] =	1254;
	ppm2[48] =	1220;
	ppm2[49] =	1186;
	ppm2[50] =	1154;
	ppm2[51] =	1123;
	ppm2[52] =	1093;
	ppm2[53] =	1065;
	ppm2[54] =	1037;
	ppm2[55] =	1011;
	ppm2[56] =	985;
	ppm2[57] =	961;
	ppm2[58] =	937;
	ppm2[59] =	914;
	ppm2[60] =	892;
	ppm2[61] =	871;
	ppm2[62] =	851;
	ppm2[63] =	831;
	ppm2[64] =	812;
	ppm2[65] =	793;
	ppm2[66] =	775;
	ppm2[67] =	758;
	ppm2[68] =	741;
	ppm2[69] =	725;
	ppm2[70] =	710;
	ppm2[71] =	694;
	ppm2[72] =	680;
	ppm2[73] =	667;
	ppm2[74] =	652;
	ppm2[75] =	638;
	ppm2[76] =	625;
	ppm2[77] =	613;
	ppm2[78] =	601;
	ppm2[79] =	589;
	ppm2[80] =	577;
	ppm2[81] =	566;
	ppm2[82] =	555;
	ppm2[83] =	545;
	ppm2[84] =	534;
	ppm2[85] =	524;
	ppm2[86] =	515;
	ppm2[87] =	505;
	ppm2[88] =	496;
	ppm2[89] =	487;
	ppm2[90] =	478;
	ppm2[91] =	470;
	ppm2[92] =	461;
	ppm2[93] =	453;
	ppm2[94] =	446;
	ppm2[95] =	438;
	ppm2[96] =	430;
	ppm2[97] =	423;
	ppm2[98] =	416;
	ppm2[99] =	409;
	ppm2[100] =	402;
	ppm2[101] =	396;
	ppm2[102] =	389;
	ppm2[103] =	383;
	ppm2[104] =	377;
	ppm2[105] =	371;
	ppm2[106] =	365;
	ppm2[107] =	359;
	ppm2[108] =	354;
	ppm2[109] =	348;
	ppm2[110] =	343;
	ppm2[111] =	338;
	ppm2[112] =	333;
	ppm2[113] =	328;
	ppm2[114] =	323;
	ppm2[115] =	318;
	ppm2[116] =	313;
	ppm2[117] =	309;
	ppm2[118] =	304;
	ppm2[119] =	300;
	ppm2[120] =	296;
	ppm2[121] =	292;
	ppm2[122] =	287;
	ppm2[123] =	283;
	ppm2[124] =	279;
	ppm2[125] =	276;
	ppm2[126] =	272;
	ppm2[127] =	268;
	ppm2[128] =	265;
	ppm2[129] =	261;
	ppm2[130] =	257;
	ppm2[131] =	254;
	ppm2[132] =	251;
	ppm2[133] =	247;
	ppm2[134] =	244;
	ppm2[135] =	241;
	ppm2[136] =	238;
	ppm2[137] =	235;
	ppm2[138] =	232;
	ppm2[139] =	229;
	ppm2[140] =	226;
	ppm2[141] =	223;
	ppm2[142] =	220;
	ppm2[143] =	218;
	ppm2[144] =	215;
	ppm2[145] =	212;
	ppm2[146] =	210;
	ppm2[147] =	207;
	ppm2[148] =	205;
	ppm2[149] =	202;
	ppm2[150] =	200;

	int ppm5[151];
	ppm5[0] =	9867;
	ppm5[1] =	9291;
	ppm5[2] =	8760;
	ppm5[3] =	8271;
	ppm5[4] =	7819;
	ppm5[5] =	7400;
	ppm5[6] =	7012;
	ppm5[7] =	6652;
	ppm5[8] =	6317;
	ppm5[9] =	6005;
	ppm5[10] =	5714;
	ppm5[11] =	5443;
	ppm5[12] =	5189;
	ppm5[13] =	4951;
	ppm5[14] =	4728;
	ppm5[15] =	4518;
	ppm5[16] =	4322;
	ppm5[17] =	4137;
	ppm5[18] =	3963;
	ppm5[19] =	3799;
	ppm5[20] =	3644;
	ppm5[21] =	3498;
	ppm5[22] =	3360;
	ppm5[23] =	3229;
	ppm5[24] =	3106;
	ppm5[25] =	2988;
	ppm5[26] =	2772;
	ppm5[27] =	2672;
	ppm5[28] =	2577;
	ppm5[29] =	2486;
	ppm5[30] =	2400;
	ppm5[31] =	2318;
	ppm5[32] =	2240;
	ppm5[33] =	2165;
	ppm5[34] =	2094;
	ppm5[35] =	2026;
	ppm5[36] =	1961;
	ppm5[37] =	1899;
	ppm5[38] =	1839;
	ppm5[39] =	1783;
	ppm5[40] =	1728;
	ppm5[41] =	1676;
	ppm5[42] =	1626;
	ppm5[43] =	1578;
	ppm5[44] =	1532;
	ppm5[45] =	1488;
	ppm5[46] =	1446;
	ppm5[47] =	1405;
	ppm5[48] =	1366;
	ppm5[49] =	1328;
	ppm5[50] =	1292;
	ppm5[51] =	1257;
	ppm5[52] =	1223;
	ppm5[53] =	1191;
	ppm5[54] =	1160;
	ppm5[55] =	1130;
	ppm5[56] =	1101;
	ppm5[57] =	1073;
	ppm5[58] =	1046;
	ppm5[59] =	1020;
	ppm5[60] =	995;
	ppm5[61] =	971;
	ppm5[62] =	947;
	ppm5[63] =	924;
	ppm5[64] =	903;
	ppm5[65] =	881;
	ppm5[66] =	861;
	ppm5[67] =	841;
	ppm5[68] =	822;
	ppm5[69] =	803;
	ppm5[70] =	785;
	ppm5[71] =	768;
	ppm5[72] =	751;
	ppm5[73] =	734;
	ppm5[74] =	718;
	ppm5[75] =	703;
	ppm5[76] =	688;
	ppm5[77] =	673;
	ppm5[78] =	659;
	ppm5[79] =	645;
	ppm5[80] =	632;
	ppm5[81] =	619;
	ppm5[82] =	606;
	ppm5[83] =	594;
	ppm5[84] =	582;
	ppm5[85] =	571;
	ppm5[86] =	559;
	ppm5[87] =	549;
	ppm5[88] =	538;
	ppm5[89] =	528;
	ppm5[90] =	517;
	ppm5[91] =	508;
	ppm5[92] =	498;
	ppm5[93] =	489;
	ppm5[94] =	480;
	ppm5[95] =	471;
	ppm5[96] =	462;
	ppm5[97] =	454;
	ppm5[98] =	446;
	ppm5[99] =	438;
	ppm5[100] =	430;
	ppm5[101] =	422;
	ppm5[102] =	415;
	ppm5[103] =	408;
	ppm5[104] =	401;
	ppm5[105] =	394;
	ppm5[106] =	387;
	ppm5[107] =	380;
	ppm5[108] =	374;
	ppm5[109] =	368;
	ppm5[110] =	362;
	ppm5[111] =	356;
	ppm5[112] =	350;
	ppm5[113] =	344;
	ppm5[114] =	339;
	ppm5[115] =	333;
	ppm5[116] =	328;
	ppm5[117] =	323;
	ppm5[118] =	317;
	ppm5[119] =	312;
	ppm5[120] =	308;
	ppm5[121] =	303;
	ppm5[122] =	298;
	ppm5[123] =	294;
	ppm5[124] =	289;
	ppm5[125] =	285;
	ppm5[126] =	280;
	ppm5[127] =	276;
	ppm5[128] =	272;
	ppm5[129] =	268;
	ppm5[130] =	264;
	ppm5[131] =	260;
	ppm5[132] =	256;
	ppm5[133] =	253;
	ppm5[134] =	249;
	ppm5[135] =	246;
	ppm5[136] =	242;
	ppm5[137] =	239;
	ppm5[138] =	235;
	ppm5[139] =	232;
	ppm5[140] =	229;
	ppm5[141] =	226;
	ppm5[142] =	222;
	ppm5[143] =	219;
	ppm5[144] =	216;
	ppm5[145] =	214;
	ppm5[146] =	211;
	ppm5[147] =	208;
	ppm5[148] =	205;
	ppm5[149] =	202;
	ppm5[150] =	200;	

	int ppm7[151];
	ppm7[0] = 4112;
	ppm7[1] = 3495;
	ppm7[2] = 3017;
	ppm7[3] = 2638;
	ppm7[4] = 2332;
	ppm7[5] = 2080;
	ppm7[6] = 1870;
	ppm7[7] = 1692;
	ppm7[8] = 1541;
	ppm7[9] = 1411;
	ppm7[10] = 1298;
	ppm7[11] = 1199;
	ppm7[12] = 1112;
	ppm7[13] = 1035;
	ppm7[14] = 967;
	ppm7[15] = 905;
	ppm7[16] = 850;
	ppm7[17] = 800;
	ppm7[18] = 755;
	ppm7[19] = 714;
	ppm7[20] = 676;
	ppm7[21] = 641;
	ppm7[22] = 610;
	ppm7[23] = 581;
	ppm7[24] = 554;
	ppm7[25] = 529;
	ppm7[26] = 506;
	ppm7[27] = 484;
	ppm7[28] = 464;
	ppm7[29] = 445;
	ppm7[30] = 428;
	ppm7[31] = 412;
	ppm7[32] = 396;
	ppm7[33] = 382;
	ppm7[34] = 368;
	ppm7[35] = 355;
	ppm7[36] = 343;
	ppm7[37] = 332;
	ppm7[38] = 321;
	ppm7[39] = 311;
	ppm7[40] = 301;
	ppm7[41] = 292;
	ppm7[42] = 283;
	ppm7[43] = 275;
	ppm7[44] = 267;
	ppm7[45] = 259;
	ppm7[46] = 252;
	ppm7[47] = 245;
	ppm7[48] = 238;
	ppm7[49] = 232;
	ppm7[50] = 226;
	ppm7[51] = 220;
	ppm7[52] = 215;
	ppm7[53] = 209;
	ppm7[54] = 204;
	ppm7[55] = 199;
	ppm7[56] = 195;
	ppm7[57] = 190;
	ppm7[58] = 186;
	ppm7[59] = 181;
	ppm7[60] = 177;
	ppm7[61] = 174;
	ppm7[62] = 170;
	ppm7[63] = 166;
	ppm7[64] = 163;
	ppm7[65] = 159;
	ppm7[66] = 156;
	ppm7[67] = 153;
	ppm7[68] = 150;
	ppm7[69] = 147;
	ppm7[70] = 144;
	ppm7[71] = 141;
	ppm7[72] = 139;
	ppm7[73] = 136;
	ppm7[74] = 133;
	ppm7[75] = 131;
	ppm7[76] = 129;
	ppm7[77] = 126;
	ppm7[78] = 124;
	ppm7[79] = 122;
	ppm7[80] = 120;
	ppm7[81] = 118;
	ppm7[82] = 116;
	ppm7[83] = 114;
	ppm7[84] = 112;
	ppm7[85] = 110;
	ppm7[86] = 108;
	ppm7[87] = 107;
	ppm7[88] = 105;
	ppm7[89] = 103;
	ppm7[90] = 102;
	ppm7[91] = 100;
	ppm7[92] = 99;
	ppm7[93] = 97;
	ppm7[94] = 96;
	ppm7[95] = 94;
	ppm7[96] = 93;
	ppm7[97] = 91;
	ppm7[98] = 90;
	ppm7[99] = 89;
	ppm7[100] = 88;
	ppm7[101] = 86;
	ppm7[102] = 85;
	ppm7[103] = 84;
	ppm7[104] = 83;
	ppm7[105] = 82;
	ppm7[106] = 81;
	ppm7[107] = 80;
	ppm7[108] = 79;
	ppm7[109] = 78;
	ppm7[110] = 77;
	ppm7[111] = 76;
	ppm7[112] = 75;
	ppm7[113] = 74;
	ppm7[114] = 73;
	ppm7[115] = 72;
	ppm7[116] = 71;
	ppm7[117] = 70;
	ppm7[118] = 69;
	ppm7[119] = 68;
	ppm7[120] = 68;
	ppm7[121] = 67;
	ppm7[122] = 66;
	ppm7[123] = 65;
	ppm7[124] = 64;
	ppm7[125] = 64;
	ppm7[126] = 63;
	ppm7[127] = 62;
	ppm7[128] = 62;
	ppm7[129] = 61;
	ppm7[130] = 60;
	ppm7[131] = 60;
	ppm7[132] = 59;
	ppm7[133] = 58;
	ppm7[134] = 58;
	ppm7[135] = 57;
	ppm7[136] = 56;
	ppm7[137] = 56;
	ppm7[138] = 55;
	ppm7[139] = 55;
	ppm7[140] = 54;
	ppm7[141] = 54;
	ppm7[142] = 53;
	ppm7[143] = 52;
	ppm7[144] = 52;
	ppm7[145] = 51;
	ppm7[146] = 51;
	ppm7[147] = 50;
	ppm7[148] = 50;
	ppm7[149] = 49;
	ppm7[150] = 49;
	
	int ppm136[161];
	ppm136[0] = 199;
	ppm136[1] = 194;
	ppm136[2] = 188;
	ppm136[3] = 183;
	ppm136[4] = 178;
	ppm136[5] = 173;
	ppm136[6] = 168;
	ppm136[7] = 163;
	ppm136[8] = 159;
	ppm136[9] = 154;
	ppm136[10] = 150;
	ppm136[11] = 146;
	ppm136[12] = 142;
	ppm136[13] = 139;
	ppm136[14] = 135;
	ppm136[15] = 131;
	ppm136[16] = 128;
	ppm136[17] = 125;
	ppm136[18] = 122;
	ppm136[19] = 119;
	ppm136[20] = 116;
	ppm136[21] = 113;
	ppm136[22] = 110;
	ppm136[23] = 107;
	ppm136[24] = 105;
	ppm136[25] = 102;
	ppm136[26] = 100;
	ppm136[27] = 97;
	ppm136[28] = 95;
	ppm136[29] = 93;
	ppm136[30] = 91;
	ppm136[31] = 89;
	ppm136[32] = 86;
	ppm136[33] = 84;
	ppm136[34] = 83;
	ppm136[35] = 81;
	ppm136[36] = 79;
	ppm136[37] = 77;
	ppm136[38] = 75;
	ppm136[39] = 74;
	ppm136[40] = 72;
	ppm136[41] = 71;
	ppm136[42] = 69;
	ppm136[43] = 68;
	ppm136[44] = 66;
	ppm136[45] = 65;
	ppm136[46] = 63;
	ppm136[47] = 62;
	ppm136[48] = 61;
	ppm136[49] = 59;
	ppm136[50] = 58;
	ppm136[51] = 57;
	ppm136[52] = 56;
	ppm136[53] = 55;
	ppm136[54] = 54;
	ppm136[55] = 53;
	ppm136[56] = 52;
	ppm136[57] = 50;
	ppm136[58] = 49;
	ppm136[59] = 49;
	ppm136[60] = 48;
	ppm136[61] = 47;
	ppm136[62] = 46;
	ppm136[63] = 45;
	ppm136[64] = 44;
	ppm136[65] = 43;
	ppm136[66] = 42;
	ppm136[67] = 42;
	ppm136[68] = 41;
	ppm136[69] = 40;
	ppm136[70] = 39;
	ppm136[71] = 39;
	ppm136[72] = 38;
	ppm136[73] = 37;
	ppm136[74] = 37;
	ppm136[75] = 36;
	ppm136[76] = 35;
	ppm136[77] = 35;
	ppm136[78] = 34;
	ppm136[79] = 33;
	ppm136[80] = 33;
	ppm136[81] = 32;
	ppm136[82] = 32;
	ppm136[83] = 31;
	ppm136[84] = 31;
	ppm136[85] = 30;
	ppm136[86] = 30;
	ppm136[87] = 29;
	ppm136[88] = 29;
	ppm136[89] = 28;
	ppm136[90] = 28;
	ppm136[91] = 27;
	ppm136[92] = 27;
	ppm136[93] = 26;
	ppm136[94] = 26;
	ppm136[95] = 25;
	ppm136[96] = 25;
	ppm136[97] = 25;
	ppm136[98] = 24;
	ppm136[99] = 24;
	ppm136[100] = 23;
	ppm136[101] = 23;
	ppm136[102] = 23;
	ppm136[103] = 22;
	ppm136[104] = 22;
	ppm136[105] = 22;
	ppm136[106] = 21;
	ppm136[107] = 21;
	ppm136[108] = 21;
	ppm136[109] = 20;
	ppm136[110] = 20;
	ppm136[111] = 20;
	ppm136[112] = 19;
	ppm136[113] = 19;
	ppm136[114] = 19;
	ppm136[115] = 19;
	ppm136[116] = 18;
	ppm136[117] = 18;
	ppm136[118] = 18;
	ppm136[119] = 17;
	ppm136[120] = 17;
	ppm136[121] = 17;
	ppm136[122] = 17;
	ppm136[123] = 16;
	ppm136[124] = 16;
	ppm136[125] = 16;
	ppm136[126] = 16;
	ppm136[127] = 16;
	ppm136[128] = 15;
	ppm136[129] = 15;
	ppm136[130] = 15;
	ppm136[131] = 15;
	ppm136[132] = 14;
	ppm136[133] = 14;
	ppm136[134] = 14;
	ppm136[135] = 14;
	ppm136[136] = 14;
	ppm136[137] = 14;
	ppm136[138] = 13;
	ppm136[139] = 13;
	ppm136[140] = 13;
	ppm136[141] = 13;
	ppm136[142] = 13;
	ppm136[143] = 12;
	ppm136[144] = 12;
	ppm136[145] = 12;
	ppm136[146] = 12;
	ppm136[147] = 12;
	ppm136[148] = 12;
	ppm136[149] = 12;
	ppm136[150] = 11;
	ppm136[151] = 11;
	ppm136[152] = 11;
	ppm136[153] = 11;
	ppm136[154] = 11;
	ppm136[155] = 11;
	ppm136[156] = 11;
	ppm136[157] = 10;
	ppm136[158] = 10;
	ppm136[159] = 10;
	ppm136[160] = 10;

	TCNT1 = 57723;
	TCCR1B = (1<<CS12) | (1<<CS10);
	TIMSK1 = (1<<TOIE1);
	usart_init(0);
	usart_init(1);
	usart_init(2);
	usart_init(3);
	init_GPS();
	f_mount(&FatFs, "", 0);		/* Give a work area to the default drive */
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
			parse_dylos(dylos, dylos_data);
			dylos_line = 0;
		}
		
		if(GPS_line == 1)
		{
			parse_GPS(GPS, GPS_data);
			GPS_line = 0;
		}
	
		if(ov_flag == 1)
		{
			ov_flag = 0;
			if(new_GPS_data == 1)
			{
				if(GPS_data[9] == 1)
					sprintf(buffer, "Date: %.2d/%.2d/%.2d\nTime: %.2d:%.2d:%.2d\nLatitude: %d %d.%d\nLongitude: %d %d.%d\nAltitude %d.%d\n", GPS_data[12], GPS_data[13], GPS_data[14], GPS_data[0], GPS_data[1], GPS_data[2], GPS_data[3], GPS_data[4], GPS_data[5], GPS_data[6], GPS_data[7], GPS_data[8], GPS_data[10], GPS_data[11]);
				else
					sprintf(buffer, "Time: %.2d:%.2d:%.2d\nNO GPS DATA AVAILABLE\n", GPS_data[0], GPS_data[1], GPS_data[2]);
				usart_send(3, buffer);
				sd_write(buffer, &Fil, fil_nm);
				new_GPS_data = 0;
			}
			if(new_dylos_data == 1)
			{
				sprintf(buffer, "Small: %d\n Large: %d\n", dylos_data[0], dylos_data[1]);
				usart_send(3, buffer);
				sd_write(buffer, &Fil, fil_nm);
				new_dylos_data = 0;
			}
			F = read_temp();
			sprintf(buffer, "Temperature %dF\n", F);
			usart_send(3, buffer);
			sd_write(buffer, &Fil, fil_nm);			
			C3H8 = read_MQ2();
			if(C3H8 < -1)
			{
				usart_send(3, "C3H8: ERROR - MQ2 reading out of range\n");
				sd_write("C3H8: ERROR - MQ2 reading out of range\n", &Fil, fil_nm);
			}
			else if(C3H8 == -1)
			{
				sprintf(buffer, "C3H8: %d ppm \n",0);
				usart_send(3, buffer);
				sd_write(buffer, &Fil, fil_nm);
			}
			else
			{
				sprintf(buffer, "C3H8: %d ppm \n",ppm2[C3H8]);
				usart_send(3, buffer);
				sd_write(buffer, &Fil, fil_nm);
			}
			CH4 = read_MQ5();
			if(CH4 < -1)
			{
				usart_send(3, "CH4 : ERROR - MQ5 reading out of range\n");
				sd_write("CH4 : ERROR - MQ5 reading out of range\n", &Fil, fil_nm);
			}
			else if(CH4 == -1)
			{
				sprintf(buffer, "CH4 : %d ppm \n",0);
				usart_send(3, buffer);
				sd_write(buffer, &Fil, fil_nm);
			}
			else
			{
				sprintf(buffer, "CH4 : %d ppm \n",ppm5[CH4]);
				usart_send(3, buffer);
				sd_write(buffer, &Fil, fil_nm);
			}
			CO = read_MQ7();
			if(CO < -1)
			{
				usart_send(3, "CO  : ERROR - MQ7 reading out of range\n");
				sd_write("CO  : ERROR - MQ7 reading out of range\n", &Fil, fil_nm);
			}
			else if(CO == -1)
			{
				sprintf(buffer, "CO  : %d ppm \n",0);
				usart_send(3, buffer);
				sd_write(buffer, &Fil, fil_nm);
			}
			else
			{
				sprintf(buffer, "CO  : %d ppm \n",ppm7[CO]);
				usart_send(3, buffer);
				sd_write(buffer, &Fil, fil_nm);
			}			
			H2S = read_MQ136();
			if(H2S < -1)
			{
				usart_send(3, "H2S : ERROR - MQ136 reading out of range\n\n");
				sd_write("H2S : ERROR - MQ136 reading out of range\n\n", &Fil, fil_nm);
			}
			else if(H2S == -1)
			{
				sprintf(buffer, "H2S : %d ppm \n\n",0);
				usart_send(3, buffer);
				sd_write(buffer, &Fil, fil_nm);
			}
			else
			{
				sprintf(buffer, "H2S : %d ppm \n\n",ppm136[H2S]);
				usart_send(3, buffer);
				sd_write(buffer, &Fil, fil_nm);
			}
		}
	}
	return 0;
}

void sd_write(char* buffer, FIL* Fil, char* fil_nm)
{
	if(fil_nm[0] != 0)
	{
		FRESULT fr;
		UINT bw;
		fr = f_open(Fil, fil_nm, FA_WRITE | FA_OPEN_ALWAYS);	/* Create a file */
		if(fr)
			usart_send(3, "\nERROR: Unable to write to SD card\n");
		f_lseek(Fil, f_size(Fil));
		f_write(Fil, buffer, strlen(buffer), &bw);
		f_close(Fil);								/* Close the file */
	}
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
	double rs = -1;
	int adc = 0;
	int RL = 9800;
	int RO = 6600;
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
		return -2;
	else if(rs > 150)
		return -1;
	else
		return rs;
}

int read_MQ5() // CH4 sensor
{
	double rs = -1;
	int adc = 0;
	int RL = 19900;
	int RO = 15750;

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
		return -2;
	else if(rs > 150)
		return -1;
	else
		return rs;
}

int read_MQ7() //C0 sensor
{
	double rs = -1;
	int adc = 0;
	int RL = 9900;
	int RO = 2000;
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
		return -2;
	else if(rs > 150)
		return -1;
	else
		return rs;
}

int read_MQ136() //H2S sensor
{
	double rs = -1;
	int adc = 0;
	int RL = 19600;
	int RO = 4150;
	ADCSRA = 0x87;
	ADMUX = 0x03; //Use ADC3
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
		rs *= 200;
		rs /= RO;
		rs -= 120;
	}
	if(rs < 0)
	return -2;
	else if(rs > 160)
	return -1;
	else
	return rs;
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

void parse_GPS(char* in, int* GPS_data)
{
	int fix = 0;
	int min, sec, latd, lats, latm, lond, lonm, lons, alt1, alt2;
	long hour;
	char ns, ew;
	int temp;
	if(in[3] == 'G')
	{
		sscanf(in, "$GPGGA,%ld.%d,%d.%d,%c,%d.%d,%c,%d,%d,%d.%d,%d.%d",&hour, &temp, &latd, &lats, &ns, &lond, &lons, &ew, &fix, &temp, &temp, &temp,&alt1, &alt2);
		latm = latd % 100;
		latd /= 100;
		lonm = lond % 100;
		lond /= 100;
		sec = hour % 100;
		hour /= 100;
		min = hour % 100;
		hour /= 100;

		if(hour > 6)
		hour = hour-7;
		else
		hour = hour + 17;
		if(fix == 1)
		{
			if(ns == 'S')
				latd = -latd;
			if(ew == 'W')
				lond = -lond;
		}
		GPS_data[0] = hour;
		GPS_data[1] = min;
		GPS_data[2] = sec;
		GPS_data[3]	= latd;
		GPS_data[4] = latm;
		GPS_data[5] = lats;
		GPS_data[6] = lond;
		GPS_data[7] = lonm;
		GPS_data[8] = lons;
		GPS_data[9] = fix;
		GPS_data[10] = alt1;
		GPS_data[11] = alt2;
	}
	else if(in[3] == 'R')
	{
		sscanf(in, "$GPRMC,%ld", &hour);
		sec = hour % 100;
		hour /= 100;
		min = hour % 100;
		hour /= 100;

		if(hour > 6)
		hour = hour-7;
		else
		hour = hour + 17;
		GPS_data[0] = hour;
		GPS_data[1] = min;
		GPS_data[2] = sec;	

		int i = 0;
		int comma = 9;
		char temp[3];
		FRESULT fr;
		while(comma > 7)
		{
			while(in[i] != ',')
				i++;
			comma--;
			i++;
		}
		if(in[i] == 'A')
		{
			GPS_data[9] = 1;
			while(comma > 0)
			{
				while(in[i] != ',')
					i++;
				comma--;
				i++;
			}
			temp[0] = in[i++];
			temp[1] = in[i++];
			temp[2] = 0;
			sscanf(temp, "%d", &GPS_data[13]);
			temp[0] = in[i++];
			temp[1] = in[i++];
			temp[2] = 0;
			sscanf(temp, "%d", &GPS_data[12]);
			temp[0] = in[i++];
			temp[1] = in[i++];
			temp[2] = 0;
			sscanf(temp, "%d", &GPS_data[14]);
			usart_send(1,"$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
			i = 0;
			do 
			{
				sprintf(fil_nm, "d%.2d%.2d%d.txt",GPS_data[12],GPS_data[13],i);
				fr = f_open(&Fil, fil_nm, FA_WRITE | FA_CREATE_NEW);
				i++;
			} while (fr);
		}
	}
	return;
}

void parse_dylos(char* in, int* dylos_data)
{
	int small, large;
	sscanf(in, "%d,%d", &small, &large);	
	dylos_data[0] = small;
	dylos_data[1] = large;
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
	usart_send(1,"$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
	return;
}