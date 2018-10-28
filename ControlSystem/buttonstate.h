/*
 * ButtonState.h
 *
 * Created: 10/23/2017 8:23:27 PM
 *  Author: clayg_000
 */ 
#include <avr/io.h>

#ifndef BUTTONSTATE_H_
#define BUTTONSTATE_H_




#define Butt_DoorOpen		6
#define Butt_DoorClose		5
#define Butt_Floor1Up		4
#define Butt_Floor2Up		3
#define Butt_Floor3Up		2
#define Butt_Floor4Up		1
#define Butt_Floor2Dwn		0
#define Butt_Floor3Dwn		7
#define Butt_Floor4Dwn		14
#define Butt_Floor5Dwn		13
#define Butt_Floor1			12
#define Butt_Floor2			11
#define Butt_Floor3			10
#define Butt_Floor4			9
#define Butt_Floor5			8
#define Butt_Emergency		15

uint16_t buttonState;
//15		14	13	12	11	10	9		8		7		6		5		4		3		2		1			0
//emgncy	fl5	fl4	fl3	fl2	fl1	fl5dwn	fl4dwn	fl3dwn	fl2dwn	fl4up	fl3up	fl2up	fl1up	dr close	dr open
uint8_t upDown ; //0 = up, 1= down
uint8_t nextFloor;
uint8_t nextDir ;
uint8_t floorNum;
#endif /* BUTTONSTATE_H_ */