 #include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "TWI.h"
#include "buttonstate.h"
#include <util/delay.h>
#include "finitestate.h"
#include "MAX7221.h"
#include "MagSwipe.h"
#define F_CPU 16000000UL
#define P4 0b01111111 // 0x7F
#define P5 0b10111111 // 0xBF
#define P6 0b11011111 // 0xDF
#define P7 0b11101111 // 0xEF
#define MASK 0b11111111 // 0xFF

#define PCF_ADDR (0x021<<1)
//15		14	13	12	11	10	9		8		7		6		5		4		3		2		1			0
//emergency	fl5	fl4	fl3	fl2	fl1	fl5dwn	fl4dwn	fl3dwn	fl2dwn	fl4up	fl3up	fl2up	fl1up	door close	door open
uint16_t const keypad[4][4] = {	{0x0400, 0x0200,0x0100, 0x8000},	//bottom row
								{0x4000, 0x2000, 0x1000, 0x0800},	//third row
								{0x0004, 0x0002, 0x0001, 0x0080 },	//second row
								{0x0040, 0x0020, 0x0010, 0x0008}};	//top row
								
									
								

uint8_t PCF8574_SINGLEREAD(uint8_t ADDR) // read from LCD
{
	uint8_t STATUS = 0,DATA = 0;
	STATUS = TWI_START();				
	//if(STATUS == TWI_START_VAL)
	STATUS = TWI_SEND((PCF_ADDR | TWI_READ_BIT));
	//if(STATUS == TWI_WRITE_ACK)
	STATUS = TWI_READ(ACK,&DATA);
	//if(STATUS == TWI_READ_ACK)
	TWI_STOP();
//	DATA &= (0b1111);
	STATUS &= (0b1111);	
	return STATUS;
}

void PCF8574_SINGLEWRITE(uint8_t DATA) // write to LCD
{
	uint8_t STATUS = 0;
	STATUS = TWI_START();
	if(STATUS == TWI_START_VAL)
	STATUS = TWI_SEND((PCF_ADDR | TWI_WRITE_BIT));
	if(STATUS == TWI_WRITE_ACK)
	TWI_SEND(DATA);
	if(STATUS == TWI_WDATA_ACK)
	TWI_STOP();
}


uint16_t readButtons(void)
{
	uint8_t result;
 
	PCF8574_SINGLEWRITE(MASK & P4);
	result = PCF8574_SINGLEREAD(result);
	if(result != 0x0F)
	{
		if(!(result & (1<<3)))
			buttonState |= keypad[0][0];
		if(!(result & (1<<2)))
			buttonState |= keypad[0][1];
		if(!(result & (1<<1)))
			buttonState |= keypad[0][2];
		if(!(result & (1<<0)))
		{
			buttonState |= keypad[0][3];
			if(curr_state==EMERGENCY)
				buttonState &= ~(1<<Butt_Emergency);
		}
	}
	
	PCF8574_SINGLEWRITE(MASK & P5);
	result= PCF8574_SINGLEREAD(result);
	if(result != 0x0F)
	{
		if(!(result & (1<<3)))
			buttonState |= keypad[1][0];
		if(!(result & (1<<2)))
			buttonState |= keypad[1][1];
		if(!(result & (1<<1)))
			buttonState |= keypad[1][2];
		if(!(result & (1<<0)))
			buttonState |= keypad[1][3];
	}
	
	PCF8574_SINGLEWRITE(MASK & P6);
	result = PCF8574_SINGLEREAD(result);
	if(result != 0x0F)
	{
		if(!(result & (1<<3)))
			buttonState |= keypad[2][0];
		if(!(result & (1<<2)))
			buttonState |= keypad[2][1];
		if(!(result & (1<<1)))
			buttonState |= keypad[2][2];
		if(!(result & (1<<0)))
			buttonState |= keypad[2][3];
	}
	
	PCF8574_SINGLEWRITE(MASK & P7);
	result = PCF8574_SINGLEREAD(result);
	if(result != 0x0F)
	{
		if(!(result & (1<<3)))
			buttonState |= keypad[3][0];
		if(!(result & (1<<2)))
			buttonState |= keypad[3][1];
		if(!(result & (1<<1)))
			buttonState |= keypad[3][2];
		if(!(result & (1<<0)))
			buttonState |= keypad[3][3];
	}
	
	PCF8574_SINGLEWRITE(0x0F);
	PCF8574_SINGLEREAD(result);
	PCIFR |= (1<<PCIF0);
	buttonled();
	
	if(((buttonState & (1<<Butt_Floor3))) && datavalid != 1)
	{
		datarequest();
		buttonState &= ~(1 << Butt_Floor3);
		TIMSK2 |= (1<<OCIE2A);
	}
	
	if(buttonState & (1<<Butt_Emergency))
	{
		MAX7221_SetDigit(MAX7221_DIGIT4, 0x08);
		MAX7221_SetDigit(MAX7221_DIGIT3, pgm_read_byte(&SEGLUT[MAX_H]));
		MAX7221_SetDigit(MAX7221_DIGIT2, pgm_read_byte(&SEGLUT[MAX_E]));
		MAX7221_SetDigit(MAX7221_DIGIT1, pgm_read_byte(&SEGLUT[MAX_L]));
		MAX7221_SetDigit(MAX7221_DIGIT0, pgm_read_byte(&SEGLUT[MAX_P]));
	}

	if(buttonState & (1<< Butt_DoorOpen) || buttonState & (1<< Butt_DoorClose))
	{
		if(curr_state==IDLE)
		{
			statetransition(TRAN);
		}
		if(curr_state==MOVING || curr_state==PROCESS || (buttonState & (1<<Butt_Emergency)))
		{
			buttonState &= ~(1<<Butt_DoorOpen);
			buttonState &= ~(1<<Butt_DoorClose);
			buttonled();
			//return;
		}
		else if(buttonState & (1<<Butt_DoorClose))
		{
			if(!(PIND & (1 << PIND3)))
			{
				buttonState &= ~(1<<Butt_DoorClose);
				solenoid_start();
			}
		}
		else if(buttonState & (1<<Butt_DoorOpen))
		{
			solenoid_stop();
			buttonState &= ~(1<<Butt_DoorOpen);
			//return;
		}
	}
	
	if(curr_state== IDLE && (!(buttonState & (1<<Butt_DoorOpen)) && !(buttonState & (1<<Butt_DoorClose))) && buttonState!= 0)
			statetransition(PROCESS);
	
	
		
}
								

ISR(PCINT0_vect)
{
	if((PINB & (1<<0)))
		return;
	
	readButtons();	
}

void buttonled(void)
{
	MAX7221_SetDigit(MAX7221_DIGIT7,(buttonState & 0xFF));
	MAX7221_SetDigit(MAX7221_DIGIT6,(buttonState>>8 & 0xFF));
}

