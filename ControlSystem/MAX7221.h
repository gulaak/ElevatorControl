#include "SPI.h"

#include <avr/pgmspace.h>

#define MAX7221_NOP			0
#define MAX7221_DIGIT0		1
#define MAX7221_DIGIT1		2
#define MAX7221_DIGIT2		3
#define MAX7221_DIGIT3		4
#define MAX7221_DIGIT4		5
#define MAX7221_DIGIT5		6
#define MAX7221_DIGIT6		7
#define MAX7221_DIGIT7		8
#define MAX7221_DECODE		9
#define MAX7221_INTENSITY	10
#define MAX7221_SCAN		11
#define MAX7221_ENABLE		12
#define MAX7221_TEST		15
#define MAX7221_DP0			0
#define MAX7221_DP1			1
#define MAX7221_DP2			2
#define MAX7221_DP3			3
#define MAX7221_DP4			4
#define MAX7221_DP5			5
#define MAX7221_DP6			6
#define MAX7221_DP7			7
#define MAX_H				10
#define MAX_E				11
#define MAX_L				12
#define MAX_P				7

void MAX7221_INIT();
void MAX7221_SEND(uint8_t ADDR, uint8_t DATA);
void MAX7221_SetDigit(uint8_t digit, uint8_t val);
void MAX7221_Print2digit (uint8_t startdigit, uint8_t BCDval,uint8_t DP );
void updatemax7221dir(uint8_t floor);
uint8_t MAX7221_BCD_2_CODEB(uint8_t BCD);
extern uint8_t const digitlut[8] PROGMEM;
extern uint8_t const SEGLUT[13] PROGMEM;