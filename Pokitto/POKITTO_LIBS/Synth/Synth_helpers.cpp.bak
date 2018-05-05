/**************************************************************************/
/*!
    @file     Synth_envfuncs.cpp
    @author   Jonne Valola

    @section LICENSE

    Pokitto development stage library
    Software License Agreement

    Copyright (c) 2015, Jonne Valola ("Author")
    All rights reserved.

    This library is intended solely for the purpose of Pokitto development.

    Redistribution and use in source and binary forms, with or without
    modification requires written permission from Author.
*/
/**************************************************************************/

#include "Pokitto.h"
#include "Synth.h"
#include "Pokitto_settings.h"

#define PROGMEM
    #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
    #ifndef WIN32
    #ifndef pgm_read_word
    #define pgm_read_word(addr) (*(const unsigned short *)(addr))
    #endif
    #else
    #define pgm_read_word(addr) (*(const unsigned int *)(addr))
    #endif // WIN32

// http://www.arklyffe.com/main/2010/08/29/xorshift-pseudorandom-number-generator/
static uint8_t y8 = 1;
static uint16_t y16 = 1;

// returns values from 1 to 255 inclusive, period is 255
uint8_t xorshift8(void) {
    //y8 ^= (y8 << 7);
    y8 ^= (y8 * 128);
    y8 ^= (y8 >> 5);
    //return y8 ^= (y8 << 3);
    return y8 ^= (y8 * 8);
}

// returns values from 1 to 65535 inclusive, period is 65535
uint16_t xorshift16(void) {
    //y16 ^= (y16 << 13);
    y16 ^= (y16 * 8192);
    y16 ^= (y16 >> 9);
    //return y16 ^= (y16 << 7);
    return y16 ^= (y16 * 128);
}


int8_t arptable[MAX_ARPMODE][5] = {
{0,0,0,0,0}, // Off
{0,4,7,4,0}, // Major tonic, 3rd, 5th
{0,3,7,3,0}, // Minor tonic, 3rd, 5th
{0,1,0,-1,0}, // vibrato test
{0,12,0,12,0}, // octave test
{0,11,14,17,21}, // funky test
};


/** VITAL TABLES **/

#if (POK_AUD_FREQ==8000)

uint32_t cincs[] ={
16573205	,	  // 0 - NOTE_B0
17555679	,	  // 1 - NOTE_C1
18602577	,	  // 2 - NOTE_CS1
19708531	,	  // 3 - NOTE_D1
20878910	,	  // 4 - NOTE_DS1
22119082	,	  // 5 - NOTE_E1
23434415	,	  // 6 - NOTE_F1
24830280	,	  // 7 - NOTE_FS1
26306675	,	  // 8 - NOTE_G1
27868969	,	  // 9 - NOTE_GS1
29527900	,	  // 10 - NOTE_A1
31283468	,	  // 11 - NOTE_AS1
33146410	,	  // 12 - NOTE_B1
35116726	,	  // 13 - NOTE_C2
37205154	,	  // 14 - NOTE_CS2
39417062	,	  // 15 - NOTE_D2
41757820	,	  // 16 - NOTE_DS2
44243532	,	  // 17 - NOTE_E2
46874199	,	  // 18 - NOTE_F2
49660559	,	  // 19 - NOTE_FS2
52613349	,	  // 20 - NOTE_G2
55743307	,	  // 21 - NOTE_GS2
59055800	,	  // 22 - NOTE_A2
62566936	,	  // 23 - NOTE_AS2
66287451	,	  // 24 - NOTE_B2
70228084	,	  // 25 - NOTE_C3
74404940	,	  // 26 - NOTE_CS3
78828756	,	  // 27 - NOTE_D3
83515639	,	  // 28 - NOTE_DS3
88481695	,	  // 29 - NOTE_E3
93743030	,	  // 30 - NOTE_F3
99321119	,	  // 31 - NOTE_FS3
105226699	,	  // 32 - NOTE_G3
111481245	,	  // 33 - NOTE_GS3
118111601	,	  // 34 - NOTE_A3
125133872	,	  // 35 - NOTE_AS3
132574903	,	  // 36 - NOTE_B3
140461537	,	  // 37 - NOTE_C4
148809879	,	  // 38 - NOTE_CS4
157657512	,	  // 39 - NOTE_D4
167036647	,	  // 40 - NOTE_DS4
176968759	,	  // 41 - NOTE_E4
187491429	,	  // 42 - NOTE_F4
198636869	,	  // 43 - NOTE_FS4
210453397	,	  // 44 - NOTE_G4
222962490	,	  // 45 - NOTE_GS4
236223201	,	  // 46 - NOTE_A4 // 440Hz
250267744	,	  // 47 - NOTE_AS4
265149806	,	  // 48 - NOTE_B4
280917705	,	  // 49 - NOTE_C5
297625127	,	  // 50 - NOTE_CS5
315320393	,	  // 51 - NOTE_D5
334067925	,	  // 52 - NOTE_DS5
353932149	,	  // 53 - NOTE_E5
374982857	,	  // 54 - NOTE_F5
397279106	,	  // 55 - NOTE_FS5
420901426	,	  // 56 - NOTE_G5
445930348	,	  // 57 - NOTE_GS5
472446402	,	  // 58 - NOTE_A5
500540857	,	  // 59 - NOTE_AS5
530304981	,	  // 60 - NOTE_B5
561835409	,	  // 61 - NOTE_C6
595244886	,	  // 62 - NOTE_CS6
630640785	,	  // 63 - NOTE_D6
668141219	,	  // 64 - NOTE_DS6
707869666	,	  // 65 - NOTE_E6
749960346	,	  // 66 - NOTE_F6
794558212	,	  // 67 - NOTE_FS6
841802852	,	  // 68 - NOTE_G6
891860696	,	  // 69 - NOTE_GS6
944892805	,	  // 70 - NOTE_A6
1001081715	,	  // 71 - NOTE_AS6
1060604593	,	  // 72 - NOTE_B6
1123670819	,	  // 73 - NOTE_C7
1190489772	,	  // 74 - NOTE_CS7
1261281571	,	  // 75 - NOTE_D7
1336282437	,	  // 76 - NOTE_DS7
1415739332	,	  // 77 - NOTE_E7
1499926060	,	  // 78 - NOTE_F7
1589116424	,	  // 79 - NOTE_FS7
1683605705	,	  // 80 - NOTE_G7
1783721392	,	  // 81 - NOTE_GS7
1889785610	,	  // 82 - NOTE_A7
2002158060	,	  // 83 - NOTE_AS7
2121214554	,	  // 84 - NOTE_B7
2247347006	,	  // 85 - NOTE_C8
2380979544	,	  // 86 - NOTE_CS8
2522557773	,	  // 87 - NOTE_D8
2672559505	,	  // 88 - NOTE_DS8
};

#elif (POK_AUD_FREQ==11025)

uint32_t cincs[] ={
12025908	,	  // 0 - NOTE_B0
12738815	,	  // 1 - NOTE_C1
13498469	,	  // 2 - NOTE_CS1
14300975	,	  // 3 - NOTE_D1
15150229	,	  // 4 - NOTE_DS1
16050127	,	  // 5 - NOTE_E1
17004564	,	  // 6 - NOTE_F1
18017436	,	  // 7 - NOTE_FS1
19088744	,	  // 8 - NOTE_G1
20222381	,	  // 9 - NOTE_GS1
21426141	,	  // 10 - NOTE_A1
22700022	,	  // 11 - NOTE_AS1
24051817	,	  // 12 - NOTE_B1
25481525	,	  // 13 - NOTE_C2
26996937	,	  // 14 - NOTE_CS2
28601950	,	  // 15 - NOTE_D2
30300459	,	  // 16 - NOTE_DS2
32104150	,	  // 17 - NOTE_E2
34013024	,	  // 18 - NOTE_F2
36034873	,	  // 19 - NOTE_FS2
38177487	,	  // 20 - NOTE_G2
40448658	,	  // 21 - NOTE_GS2
42852281	,	  // 22 - NOTE_A2
45400044	,	  // 23 - NOTE_AS2
48099738	,	  // 24 - NOTE_B2
50959154	,	  // 25 - NOTE_C3
53989979	,	  // 26 - NOTE_CS3
57200004	,	  // 27 - NOTE_D3
60600917	,	  // 28 - NOTE_DS3
64204405	,	  // 29 - NOTE_E3
68022153	,	  // 30 - NOTE_F3
72069746	,	  // 31 - NOTE_FS3
76354974	,	  // 32 - NOTE_G3
80893420	,	  // 33 - NOTE_GS3
85704563	,	  // 34 - NOTE_A3
90800089	,	  // 35 - NOTE_AS3
96199476	,	  // 36 - NOTE_B3
101922203	,	  // 37 - NOTE_C4
107979958	,	  // 38 - NOTE_CS4
114400009	,	  // 39 - NOTE_D4
121205730	,	  // 40 - NOTE_DS4
128412705	,	  // 41 - NOTE_E4
136048202	,	  // 42 - NOTE_F4
144135596	,	  // 43 - NOTE_FS4
152709948	,	  // 44 - NOTE_G4
161786841	,	  // 45 - NOTE_GS4
171409126	,	  // 46 - NOTE_A4 // 440Hz
181600177	,	  // 47 - NOTE_AS4
192398952	,	  // 48 - NOTE_B4
203840511	,	  // 49 - NOTE_C5
215963811	,	  // 50 - NOTE_CS5
228803913	,	  // 51 - NOTE_D5
242407565	,	  // 52 - NOTE_DS5
256821514	,	  // 53 - NOTE_E5
272096404	,	  // 54 - NOTE_F5
288275088	,	  // 55 - NOTE_FS5
305416001	,	  // 56 - NOTE_G5
323577577	,	  // 57 - NOTE_GS5
342818251	,	  // 58 - NOTE_A5
363204250	,	  // 59 - NOTE_AS5
384801800	,	  // 60 - NOTE_B5
407681023	,	  // 61 - NOTE_C6
431923727	,	  // 62 - NOTE_CS6
457607826	,	  // 63 - NOTE_D6
484819025	,	  // 64 - NOTE_DS6
513646923	,	  // 65 - NOTE_E6
544188913	,	  // 66 - NOTE_F6
576550177	,	  // 67 - NOTE_FS6
610832002	,	  // 68 - NOTE_G6
647155154	,	  // 69 - NOTE_GS6
685636502	,	  // 70 - NOTE_A6
726408500	,	  // 71 - NOTE_AS6
769599704	,	  // 72 - NOTE_B6
815362045	,	  // 73 - NOTE_C7
863847454	,	  // 74 - NOTE_CS7
915215652	,	  // 75 - NOTE_D7
969638050	,	  // 76 - NOTE_DS7
1027293846	,	  // 77 - NOTE_E7
1088381721	,	  // 78 - NOTE_F7
1153100353	,	  // 79 - NOTE_FS7
1221664003	,	  // 80 - NOTE_G7
1294310307	,	  // 81 - NOTE_GS7
1371273005	,	  // 82 - NOTE_A7
1452813105	,	  // 83 - NOTE_AS7
1539203304	,	  // 84 - NOTE_B7
1630727986	,	  // 85 - NOTE_C8
1727694908	,	  // 86 - NOTE_CS8
1830427409	,	  // 87 - NOTE_D8
1939272203	,	  // 88 - NOTE_DS8
};

#elif (POK_AUD_FREQ==16000)

uint32_t cincs[] ={
8286603	,	  // 0 - NOTE_B0
8777839	,	  // 1 - NOTE_C1
9301289	,	  // 2 - NOTE_CS1
9854266	,	  // 3 - NOTE_D1
10439455	,	  // 4 - NOTE_DS1
11059541	,	  // 5 - NOTE_E1
11717208	,	  // 6 - NOTE_F1
12415140	,	  // 7 - NOTE_FS1
13153337	,	  // 8 - NOTE_G1
13934485	,	  // 9 - NOTE_GS1
14763950	,	  // 10 - NOTE_A1
15641734	,	  // 11 - NOTE_AS1
16573205	,	  // 12 - NOTE_B1
17558363	,	  // 13 - NOTE_C2
18602577	,	  // 14 - NOTE_CS2
19708531	,	  // 15 - NOTE_D2
20878910	,	  // 16 - NOTE_DS2
22121766	,	  // 17 - NOTE_E2
23437100	,	  // 18 - NOTE_F2
24830280	,	  // 19 - NOTE_FS2
26306675	,	  // 20 - NOTE_G2
27871653	,	  // 21 - NOTE_GS2
29527900	,	  // 22 - NOTE_A2
31283468	,	  // 23 - NOTE_AS2
33143726	,	  // 24 - NOTE_B2
35114042	,	  // 25 - NOTE_C3
37202470	,	  // 26 - NOTE_CS3
39414378	,	  // 27 - NOTE_D3
41757820	,	  // 28 - NOTE_DS3
44240847	,	  // 29 - NOTE_E3
46871515	,	  // 30 - NOTE_F3
49660559	,	  // 31 - NOTE_FS3
52613349	,	  // 32 - NOTE_G3
55740622	,	  // 33 - NOTE_GS3
59055800	,	  // 34 - NOTE_A3
62566936	,	  // 35 - NOTE_AS3
66287451	,	  // 36 - NOTE_B3
70230768	,	  // 37 - NOTE_C4
74404940	,	  // 38 - NOTE_CS4
78828756	,	  // 39 - NOTE_D4
83518323	,	  // 40 - NOTE_DS4
88484379	,	  // 41 - NOTE_E4
93745714	,	  // 42 - NOTE_F4
99318434	,	  // 43 - NOTE_FS4
105226699	,	  // 44 - NOTE_G4
111481245	,	  // 45 - NOTE_GS4
118111601	,	  // 46 - NOTE_A4 // 440Hz
125133872	,	  // 47 - NOTE_AS4
132574903	,	  // 48 - NOTE_B4
140458852	,	  // 49 - NOTE_C5
148812564	,	  // 50 - NOTE_CS5
157660196	,	  // 51 - NOTE_D5
167033962	,	  // 52 - NOTE_DS5
176966074	,	  // 53 - NOTE_E5
187491429	,	  // 54 - NOTE_F5
198639553	,	  // 55 - NOTE_FS5
210450713	,	  // 56 - NOTE_G5
222965174	,	  // 57 - NOTE_GS5
236223201	,	  // 58 - NOTE_A5
250270429	,	  // 59 - NOTE_AS5
265152490	,	  // 60 - NOTE_B5
280917705	,	  // 61 - NOTE_C6
297622443	,	  // 62 - NOTE_CS6
315320393	,	  // 63 - NOTE_D6
334070609	,	  // 64 - NOTE_DS6
353934833	,	  // 65 - NOTE_E6
374980173	,	  // 66 - NOTE_F6
397279106	,	  // 67 - NOTE_FS6
420901426	,	  // 68 - NOTE_G6
445930348	,	  // 69 - NOTE_GS6
472446402	,	  // 70 - NOTE_A6
500540857	,	  // 71 - NOTE_AS6
530302296	,	  // 72 - NOTE_B6
561835409	,	  // 73 - NOTE_C7
595244886	,	  // 74 - NOTE_CS7
630640785	,	  // 75 - NOTE_D7
668141219	,	  // 76 - NOTE_DS7
707869666	,	  // 77 - NOTE_E7
749963030	,	  // 78 - NOTE_F7
794558212	,	  // 79 - NOTE_FS7
841802852	,	  // 80 - NOTE_G7
891860696	,	  // 81 - NOTE_GS7
944892805	,	  // 82 - NOTE_A7
1001079030	,	  // 83 - NOTE_AS7
1060607277	,	  // 84 - NOTE_B7
1123673503	,	  // 85 - NOTE_C8
1190489772	,	  // 86 - NOTE_CS8
1261278886	,	  // 87 - NOTE_D8
1336279753	,	  // 88 - NOTE_DS8
};

#elif (POK_AUD_FREQ==22050)

uint32_t cincs[] ={
6012954	,	  // 0 - NOTE_B0
6369407	,	  // 1 - NOTE_C1
6749234	,	  // 2 - NOTE_CS1
7150488	,	  // 3 - NOTE_D1
7575115	,	  // 4 - NOTE_DS1
8025064	,	  // 5 - NOTE_E1
8502282	,	  // 6 - NOTE_F1
9008718	,	  // 7 - NOTE_FS1
9544372	,	  // 8 - NOTE_G1
10111191	,	  // 9 - NOTE_GS1
10713070	,	  // 10 - NOTE_A1
11350011	,	  // 11 - NOTE_AS1
12025908	,	  // 12 - NOTE_B1
12740762	,	  // 13 - NOTE_C2
13498469	,	  // 14 - NOTE_CS2
14300975	,	  // 15 - NOTE_D2
15150229	,	  // 16 - NOTE_DS2
16052075	,	  // 17 - NOTE_E2
17006512	,	  // 18 - NOTE_F2
18017436	,	  // 19 - NOTE_FS2
19088744	,	  // 20 - NOTE_G2
20224329	,	  // 21 - NOTE_GS2
21426141	,	  // 22 - NOTE_A2
22700022	,	  // 23 - NOTE_AS2
24049869	,	  // 24 - NOTE_B2
25479577	,	  // 25 - NOTE_C3
26994989	,	  // 26 - NOTE_CS3
28600002	,	  // 27 - NOTE_D3
30300459	,	  // 28 - NOTE_DS3
32102202	,	  // 29 - NOTE_E3
34011077	,	  // 30 - NOTE_F3
36034873	,	  // 31 - NOTE_FS3
38177487	,	  // 32 - NOTE_G3
40446710	,	  // 33 - NOTE_GS3
42852281	,	  // 34 - NOTE_A3
45400044	,	  // 35 - NOTE_AS3
48099738	,	  // 36 - NOTE_B3
50961102	,	  // 37 - NOTE_C4
53989979	,	  // 38 - NOTE_CS4
57200004	,	  // 39 - NOTE_D4
60602865	,	  // 40 - NOTE_DS4
64206352	,	  // 41 - NOTE_E4
68024101	,	  // 42 - NOTE_F4
72067798	,	  // 43 - NOTE_FS4
76354974	,	  // 44 - NOTE_G4
80893420	,	  // 45 - NOTE_GS4
85704563	,	  // 46 - NOTE_A4 // 440Hz
90800089	,	  // 47 - NOTE_AS4
96199476	,	  // 48 - NOTE_B4
101920256	,	  // 49 - NOTE_C5
107981906	,	  // 50 - NOTE_CS5
114401957	,	  // 51 - NOTE_D5
121203782	,	  // 52 - NOTE_DS5
128410757	,	  // 53 - NOTE_E5
136048202	,	  // 54 - NOTE_F5
144137544	,	  // 55 - NOTE_FS5
152708000	,	  // 56 - NOTE_G5
161788788	,	  // 57 - NOTE_GS5
171409126	,	  // 58 - NOTE_A5
181602125	,	  // 59 - NOTE_AS5
192400900	,	  // 60 - NOTE_B5
203840511	,	  // 61 - NOTE_C6
215961863	,	  // 62 - NOTE_CS6
228803913	,	  // 63 - NOTE_D6
242409512	,	  // 64 - NOTE_DS6
256823462	,	  // 65 - NOTE_E6
272094456	,	  // 66 - NOTE_F6
288275088	,	  // 67 - NOTE_FS6
305416001	,	  // 68 - NOTE_G6
323577577	,	  // 69 - NOTE_GS6
342818251	,	  // 70 - NOTE_A6
363204250	,	  // 71 - NOTE_AS6
384799852	,	  // 72 - NOTE_B6
407681023	,	  // 73 - NOTE_C7
431923727	,	  // 74 - NOTE_CS7
457607826	,	  // 75 - NOTE_D7
484819025	,	  // 76 - NOTE_DS7
513646923	,	  // 77 - NOTE_E7
544190861	,	  // 78 - NOTE_F7
576550177	,	  // 79 - NOTE_FS7
610832002	,	  // 80 - NOTE_G7
647155154	,	  // 81 - NOTE_GS7
685636502	,	  // 82 - NOTE_A7
726406553	,	  // 83 - NOTE_AS7
769601652	,	  // 84 - NOTE_B7
815363993	,	  // 85 - NOTE_C8
863847454	,	  // 86 - NOTE_CS8
915213704	,	  // 87 - NOTE_D8
969636102	,	  // 88 - NOTE_DS8
};


#elif (POK_AUD_FREQ==32000)

uint32_t cincs[] ={
4143301	,	  // 0 - NOTE_B0
4388920	,	  // 1 - NOTE_C1
4650644	,	  // 2 - NOTE_CS1
4927133	,	  // 3 - NOTE_D1
5219727	,	  // 4 - NOTE_DS1
5529770	,	  // 5 - NOTE_E1
5858604	,	  // 6 - NOTE_F1
6207570	,	  // 7 - NOTE_FS1
6576669	,	  // 8 - NOTE_G1
6967242	,	  // 9 - NOTE_GS1
7381975	,	  // 10 - NOTE_A1
7820867	,	  // 11 - NOTE_AS1
8286603	,	  // 12 - NOTE_B1
8779182	,	  // 13 - NOTE_C2
9301289	,	  // 14 - NOTE_CS2
9854266	,	  // 15 - NOTE_D2
10439455	,	  // 16 - NOTE_DS2
11060883	,	  // 17 - NOTE_E2
11718550	,	  // 18 - NOTE_F2
12415140	,	  // 19 - NOTE_FS2
13153337	,	  // 20 - NOTE_G2
13935827	,	  // 21 - NOTE_GS2
14763950	,	  // 22 - NOTE_A2
15641734	,	  // 23 - NOTE_AS2
16571863	,	  // 24 - NOTE_B2
17557021	,	  // 25 - NOTE_C3
18601235	,	  // 26 - NOTE_CS3
19707189	,	  // 27 - NOTE_D3
20878910	,	  // 28 - NOTE_DS3
22120424	,	  // 29 - NOTE_E3
23435757	,	  // 30 - NOTE_F3
24830280	,	  // 31 - NOTE_FS3
26306675	,	  // 32 - NOTE_G3
27870311	,	  // 33 - NOTE_GS3
29527900	,	  // 34 - NOTE_A3
31283468	,	  // 35 - NOTE_AS3
33143726	,	  // 36 - NOTE_B3
35115384	,	  // 37 - NOTE_C4
37202470	,	  // 38 - NOTE_CS4
39414378	,	  // 39 - NOTE_D4
41759162	,	  // 40 - NOTE_DS4
44242190	,	  // 41 - NOTE_E4
46872857	,	  // 42 - NOTE_F4
49659217	,	  // 43 - NOTE_FS4
52613349	,	  // 44 - NOTE_G4
55740622	,	  // 45 - NOTE_GS4
59055800	,	  // 46 - NOTE_A4 // 440Hz
62566936	,	  // 47 - NOTE_AS4
66287451	,	  // 48 - NOTE_B4
70229426	,	  // 49 - NOTE_C5
74406282	,	  // 50 - NOTE_CS5
78830098	,	  // 51 - NOTE_D5
83516981	,	  // 52 - NOTE_DS5
88483037	,	  // 53 - NOTE_E5
93745714	,	  // 54 - NOTE_F5
99319777	,	  // 55 - NOTE_FS5
105225357	,	  // 56 - NOTE_G5
111482587	,	  // 57 - NOTE_GS5
118111601	,	  // 58 - NOTE_A5
125135214	,	  // 59 - NOTE_AS5
132576245	,	  // 60 - NOTE_B5
140458852	,	  // 61 - NOTE_C6
148811222	,	  // 62 - NOTE_CS6
157660196	,	  // 63 - NOTE_D6
167035305	,	  // 64 - NOTE_DS6
176967417	,	  // 65 - NOTE_E6
187490086	,	  // 66 - NOTE_F6
198639553	,	  // 67 - NOTE_FS6
210450713	,	  // 68 - NOTE_G6
222965174	,	  // 69 - NOTE_GS6
236223201	,	  // 70 - NOTE_A6
250270429	,	  // 71 - NOTE_AS6
265151148	,	  // 72 - NOTE_B6
280917705	,	  // 73 - NOTE_C7
297622443	,	  // 74 - NOTE_CS7
315320393	,	  // 75 - NOTE_D7
334070609	,	  // 76 - NOTE_DS7
353934833	,	  // 77 - NOTE_E7
374981515	,	  // 78 - NOTE_F7
397279106	,	  // 79 - NOTE_FS7
420901426	,	  // 80 - NOTE_G7
445930348	,	  // 81 - NOTE_GS7
472446402	,	  // 82 - NOTE_A7
500539515	,	  // 83 - NOTE_AS7
530303638	,	  // 84 - NOTE_B7
561836751	,	  // 85 - NOTE_C8
595244886	,	  // 86 - NOTE_CS8
630639443	,	  // 87 - NOTE_D8
668139876	,	  // 88 - NOTE_DS8
};


#elif (POK_AUD_FREQ==44100)

uint32_t cincs[] ={
3006477	,	  // 0 - NOTE_B0
3184704	,	  // 1 - NOTE_C1
3374617	,	  // 2 - NOTE_CS1
3575244	,	  // 3 - NOTE_D1
3787557	,	  // 4 - NOTE_DS1
4012532	,	  // 5 - NOTE_E1
4251141	,	  // 6 - NOTE_F1
4504359	,	  // 7 - NOTE_FS1
4772186	,	  // 8 - NOTE_G1
5055595	,	  // 9 - NOTE_GS1
5356535	,	  // 10 - NOTE_A1
5675006	,	  // 11 - NOTE_AS1
6012954	,	  // 12 - NOTE_B1
6370381	,	  // 13 - NOTE_C2
6749234	,	  // 14 - NOTE_CS2
7150488	,	  // 15 - NOTE_D2
7575115	,	  // 16 - NOTE_DS2
8026038	,	  // 17 - NOTE_E2
8503256	,	  // 18 - NOTE_F2
9008718	,	  // 19 - NOTE_FS2
9544372	,	  // 20 - NOTE_G2
10112164	,	  // 21 - NOTE_GS2
10713070	,	  // 22 - NOTE_A2
11350011	,	  // 23 - NOTE_AS2
12024935	,	  // 24 - NOTE_B2
12739788	,	  // 25 - NOTE_C3
13497495	,	  // 26 - NOTE_CS3
14300001	,	  // 27 - NOTE_D3
15150229	,	  // 28 - NOTE_DS3
16051101	,	  // 29 - NOTE_E3
17005538	,	  // 30 - NOTE_F3
18017436	,	  // 31 - NOTE_FS3
19088744	,	  // 32 - NOTE_G3
20223355	,	  // 33 - NOTE_GS3
21426141	,	  // 34 - NOTE_A3
22700022	,	  // 35 - NOTE_AS3
24049869	,	  // 36 - NOTE_B3
25480551	,	  // 37 - NOTE_C4
26994989	,	  // 38 - NOTE_CS4
28600002	,	  // 39 - NOTE_D4
30301433	,	  // 40 - NOTE_DS4
32103176	,	  // 41 - NOTE_E4
34012051	,	  // 42 - NOTE_F4
36033899	,	  // 43 - NOTE_FS4
38177487	,	  // 44 - NOTE_G4
40446710	,	  // 45 - NOTE_GS4
42852281	,	  // 46 - NOTE_A4 // 440Hz
45400044	,	  // 47 - NOTE_AS4
48099738	,	  // 48 - NOTE_B4
50960128	,	  // 49 - NOTE_C5
53990953	,	  // 50 - NOTE_CS5
57200978	,	  // 51 - NOTE_D5
60601891	,	  // 52 - NOTE_DS5
64205378	,	  // 53 - NOTE_E5
68024101	,	  // 54 - NOTE_F5
72068772	,	  // 55 - NOTE_FS5
76354000	,	  // 56 - NOTE_G5
80894394	,	  // 57 - NOTE_GS5
85704563	,	  // 58 - NOTE_A5
90801063	,	  // 59 - NOTE_AS5
96200450	,	  // 60 - NOTE_B5
101920256	,	  // 61 - NOTE_C6
107980932	,	  // 62 - NOTE_CS6
114401957	,	  // 63 - NOTE_D6
121204756	,	  // 64 - NOTE_DS6
128411731	,	  // 65 - NOTE_E6
136047228	,	  // 66 - NOTE_F6
144137544	,	  // 67 - NOTE_FS6
152708000	,	  // 68 - NOTE_G6
161788788	,	  // 69 - NOTE_GS6
171409126	,	  // 70 - NOTE_A6
181602125	,	  // 71 - NOTE_AS6
192399926	,	  // 72 - NOTE_B6
203840511	,	  // 73 - NOTE_C7
215961863	,	  // 74 - NOTE_CS7
228803913	,	  // 75 - NOTE_D7
242409512	,	  // 76 - NOTE_DS7
256823462	,	  // 77 - NOTE_E7
272095430	,	  // 78 - NOTE_F7
288275088	,	  // 79 - NOTE_FS7
305416001	,	  // 80 - NOTE_G7
323577577	,	  // 81 - NOTE_GS7
342818251	,	  // 82 - NOTE_A7
363203276	,	  // 83 - NOTE_AS7
384800826	,	  // 84 - NOTE_B7
407681997	,	  // 85 - NOTE_C8
431923727	,	  // 86 - NOTE_CS8
457606852	,	  // 87 - NOTE_D8
484818051	,	  // 88 - NOTE_DS8
};



#endif

/** NOTE TO TEXT TABLES **/

const char note_0[] PROGMEM = "B-0";  // 0 - NOTE_B0
const char note_1[] PROGMEM = "C-1";  // 1 - NOTE_C1
const char note_2[] PROGMEM = "C#1";  // 2 - NOTE_CS1
const char note_3[] PROGMEM = "D-1";  // 3 - NOTE_D1
const char note_4[] PROGMEM = "D#1";  // 4 - NOTE_DS1
const char note_5[] PROGMEM = "E-1";  // 5 - NOTE_E1
const char note_6[] PROGMEM = "F-1";  // 6 - NOTE_F1
const char note_7[] PROGMEM = "F#1";  // 7 - NOTE_FS1
const char note_8[] PROGMEM = "G-1";  // 8 - NOTE_G1
const char note_9[] PROGMEM = "G#1";  // 9 - NOTE_GS1
const char note_10[] PROGMEM = "A-1";  // 10 - NOTE_A1
const char note_11[] PROGMEM = "A#1";  // 11 - NOTE_AS1
const char note_12[] PROGMEM = "B-1";  // 12 - NOTE_B1
const char note_13[] PROGMEM = "C-2";  // 2 - NOTE_C2
const char note_14[] PROGMEM = "C#2";  // 2 - NOTE_CS2
const char note_15[] PROGMEM = "D-2";  // 3 - NOTE_D2
const char note_16[] PROGMEM = "D#2";  // 4 - NOTE_DS2
const char note_17[] PROGMEM = "E-2";  // 5 - NOTE_E2
const char note_18[] PROGMEM = "F-2";  // 6 - NOTE_F2
const char note_19[] PROGMEM = "F#2";  // 7 - NOTE_FS2
const char note_20[] PROGMEM = "G-2";  // 8 - NOTE_G2
const char note_21[] PROGMEM = "G#2";  // 9 - NOTE_GS2
const char note_22[] PROGMEM = "A-2";  // 20 - NOTE_A2
const char note_23[] PROGMEM = "A#2";  // 22 - NOTE_AS2
const char note_24[] PROGMEM = "B-2";  // 22 - NOTE_B2
const char note_25[] PROGMEM = "C-3";  // 3 - NOTE_C3
const char note_26[] PROGMEM = "C#3";  // 3 - NOTE_CS3
const char note_27[] PROGMEM = "D-3";  // 3 - NOTE_D3
const char note_28[] PROGMEM = "D#3";  // 4 - NOTE_DS3
const char note_29[] PROGMEM = "E-3";  // 5 - NOTE_E3
const char note_30[] PROGMEM = "F-3";  // 6 - NOTE_F3
const char note_31[] PROGMEM = "F#3";  // 7 - NOTE_FS3
const char note_32[] PROGMEM = "G-3";  // 8 - NOTE_G3
const char note_33[] PROGMEM = "G#3";  // 9 - NOTE_GS3
const char note_34[] PROGMEM = "A-3";  // 30 - NOTE_A3
const char note_35[] PROGMEM = "A#3";  // 33 - NOTE_AS3
const char note_36[] PROGMEM = "B-3";  // 33 - NOTE_B3
const char note_37[] PROGMEM = "C-4";  // 4 - NOTE_C4
const char note_38[] PROGMEM = "C#4";  // 4 - NOTE_CS4
const char note_39[] PROGMEM = "D-4";  // 3 - NOTE_D4
const char note_40[] PROGMEM = "D#4";  // 4 - NOTE_DS4
const char note_41[] PROGMEM = "E-4";  // 5 - NOTE_E4
const char note_42[] PROGMEM = "F-4";  // 6 - NOTE_F4
const char note_43[] PROGMEM = "F#4";  // 7 - NOTE_FS4
const char note_44[] PROGMEM = "G-4";  // 8 - NOTE_G4
const char note_45[] PROGMEM = "G#4";  // 9 - NOTE_GS4
const char note_46[] PROGMEM = "A-4";  // 40 - NOTE_A4
const char note_47[] PROGMEM = "A#4";  // 44 - NOTE_AS4
const char note_48[] PROGMEM = "B-4";  // 44 - NOTE_B4
const char note_49[] PROGMEM = "C-5";  // 5 - NOTE_C5
const char note_50[] PROGMEM = "C#5";  // 5 - NOTE_CS5
const char note_51[] PROGMEM = "D-5";  // 3 - NOTE_D5
const char note_52[] PROGMEM = "D#5";  // 4 - NOTE_DS5
const char note_53[] PROGMEM = "E-5";  // 5 - NOTE_E5
const char note_54[] PROGMEM = "F-5";  // 6 - NOTE_F5
const char note_55[] PROGMEM = "F#5";  // 7 - NOTE_FS5
const char note_56[] PROGMEM = "G-5";  // 8 - NOTE_G5
const char note_57[] PROGMEM = "G#5";  // 9 - NOTE_GS5
const char note_58[] PROGMEM = "A-5";  // 50 - NOTE_A5
const char note_59[] PROGMEM = "A#5";  // 55 - NOTE_AS5
const char note_60[] PROGMEM = "B-5";  // 55 - NOTE_B5
const char note_61[] PROGMEM = "C-6";  // 6 - NOTE_C6
const char note_62[] PROGMEM = "C#6";  // 6 - NOTE_CS6
const char note_63[] PROGMEM = "D-6";  // 3 - NOTE_D6
const char note_64[] PROGMEM = "D#6";  // 4 - NOTE_DS6
const char note_65[] PROGMEM = "E-6";  // 5 - NOTE_E6
const char note_66[] PROGMEM = "F-6";  // 6 - NOTE_F6
const char note_67[] PROGMEM = "F#6";  // 7 - NOTE_FS6
const char note_68[] PROGMEM = "G-6";  // 8 - NOTE_G6
const char note_69[] PROGMEM = "G#6";  // 9 - NOTE_GS6
const char note_70[] PROGMEM = "A-6";  // 60 - NOTE_A6
const char note_71[] PROGMEM = "A#6";  // 66 - NOTE_AS6
const char note_72[] PROGMEM = "B-6";  // 66 - NOTE_B6
const char note_73[] PROGMEM = "C-7";  // 7 - NOTE_C7
const char note_74[] PROGMEM = "C#7";  // 7 - NOTE_CS7
const char note_75[] PROGMEM = "D-7";  // 3 - NOTE_D7
const char note_76[] PROGMEM = "D#7";  // 4 - NOTE_DS7
const char note_77[] PROGMEM = "E-7";  // 5 - NOTE_E7
const char note_78[] PROGMEM = "F-7";  // 6 - NOTE_F7
const char note_79[] PROGMEM = "F#7";  // 7 - NOTE_FS7
const char note_80[] PROGMEM = "G-7";  // 8 - NOTE_G7
const char note_81[] PROGMEM = "G#7";  // 9 - NOTE_GS7
const char note_82[] PROGMEM = "A-7";  // 70 - NOTE_A7
const char note_83[] PROGMEM = "A#7";  // 77 - NOTE_AS7
const char note_84[] PROGMEM = "B-7";  // 77 - NOTE_B7
const char note_85[] PROGMEM = "C-8";  // 8 - NOTE_C8
const char note_86[] PROGMEM = "C#8";  // 8 - NOTE_CS8
const char note_87[] PROGMEM = "D-8";  // 3 - NOTE_D8
const char note_88[] PROGMEM = "D#8";  // 4 - NOTE_DS8

const char* note_table[] PROGMEM = {
 note_0,
 note_1,
 note_2,
 note_3,
 note_4,
 note_5,
 note_6,
 note_7,
 note_8,
 note_9,
 note_10,
 note_11,
 note_12,
 note_13,
 note_14,
 note_15,
 note_16,
 note_17,
 note_18,
 note_19,
 note_20,
 note_21,
 note_22,
 note_23,
 note_24,
 note_25,
 note_26,
 note_27,
 note_28,
 note_29,
 note_30,
 note_31,
 note_32,
 note_33,
 note_34,
 note_35,
 note_36,
 note_37,
 note_38,
 note_39,
 note_40,
 note_41,
 note_42,
 note_43,
 note_44,
 note_45,
 note_46,
 note_47,
 note_48,
 note_49,
 note_50,
 note_51,
 note_52,
 note_53,
 note_54,
 note_55,
 note_56,
 note_57,
 note_58,
 note_59,
 note_60,
 note_61,
 note_62,
 note_63,
 note_64,
 note_65,
 note_66,
 note_67,
 note_68,
 note_69,
 note_70,
 note_71,
 note_72,
 note_73,
 note_74,
 note_75,
 note_76,
 note_77,
 note_78,
 note_79,
 note_80,
 note_81,
 note_82,
 note_83,
 note_84,
 note_85,
 note_86,
 note_87,
 note_88,
};

void getNoteString(char * buffer, uint8_t i) {
    if (i>88) strcpy(buffer,"@@@");
    else strcpy(buffer, note_table[i]);
}

