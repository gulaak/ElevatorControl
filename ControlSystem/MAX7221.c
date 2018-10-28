#include "MAX7221.h"
#include "buttonstate.h""
void MAX7221_INIT()
{
	uint8_t i;
	//msb first, mode 0, divider
	SPI_Init (0, 0, 0);
	//decoding modes
	MAX7221_SEND(MAX7221_DECODE, 0);
	//scan limit
	MAX7221_SEND(MAX7221_SCAN, 7);
	//intensity
	MAX7221_SEND(MAX7221_INTENSITY, 0xf);
	//turn on display
	MAX7221_SEND(MAX7221_ENABLE, 1);
	//set values
	for(i=0;i<8;i++)
	MAX7221_SetDigit((i+MAX7221_DIGIT0), 0x00);
}
uint8_t PROGMEM const Digitlut[8]  = {4, 3, 2, 1, 5, 6, 7, 8};	//order of eight displays, may require adjustment
uint8_t PROGMEM const SEGLUT[13] = {0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x3E, 0x67, 0x3D, 0x15, 0x37, 0x4F, 0x0E};
								//  0     1     2     3     4     5     U     P     D     N		H	  E		L
void MAX7221_SetDigit(uint8_t digit, uint8_t val)
{
	MAX7221_SEND(pgm_read_byte(&(Digitlut[digit - 1])), val);
}

void MAX7221_Print2digit (uint8_t startdigit, uint8_t BCDval, uint8_t DP )
{
	uint8_t decand = (DP>>(startdigit))&1;
	uint8_t CODEB = MAX7221_BCD_2_CODEB(BCDval >> 4);
	if(decand == 1)
	CODEB |= 0x80;
	MAX7221_SetDigit(startdigit, CODEB);
	decand = (DP >> (startdigit-MAX7221_DIGIT0)) & 1;
	CODEB = MAX7221_BCD_2_CODEB(BCDval & 0xF);
	if(decand == 1)
	CODEB |= 0x80;
	MAX7221_SetDigit(startdigit+1, CODEB);
}

void MAX7221_SEND(uint8_t ADDR, uint8_t DATA)
{
	SPI_ChipSelect(0);
	SPI_Send(ADDR & 0xf);
	SPI_Send(DATA);
	SPI_ChipSelect(1);
}

uint8_t MAX7221_BCD_2_CODEB(uint8_t BCD)
{
	uint8_t CODEB = 0;
	CODEB = pgm_read_byte(&(SEGLUT[BCD]));
	return CODEB;
}

void updatemax7221dir(uint8_t dir)
{
	if(!(buttonState&(1<<Butt_Emergency)))
	{
		if(dir==1)
		{
			MAX7221_SetDigit(MAX7221_DIGIT3,pgm_read_byte(&SEGLUT[8]));
			MAX7221_SetDigit(MAX7221_DIGIT2,pgm_read_byte(&SEGLUT[9]));
		}
		else
		{
			MAX7221_SetDigit(MAX7221_DIGIT3,pgm_read_byte(&SEGLUT[6]));
			MAX7221_SetDigit(MAX7221_DIGIT2,pgm_read_byte(&SEGLUT[7]));
		}
	}
}

void max7221_cleardir(void)
{
	MAX7221_SetDigit(MAX7221_DIGIT2, 0x00);
	MAX7221_SetDigit(MAX7221_DIGIT3, 0x00);
}