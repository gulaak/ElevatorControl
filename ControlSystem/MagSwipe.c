/*
 * MagSwipe.c
 *
 * Created: 10/17/2017 8:51:42 PM
 *  Author: Gulaa
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <string.h>
#include "finitestate.h"
#include "MagSwipe.h"
#include "Motordriver.h"
#include "buttonstate.h"

void readdata(int bit);
uint8_t PROGMEM const verify[82] = {230,111,64,115,54,7,62,3,3,85,109,120,10,118,101,119,96,50,122,67,95,1,33,108,110,124,6,55,48,115,112,24,57,65,111,64,96,118,47,62,3,23,88,57,120,12,28,96,119,96,48,59,7,31,1,67,76,44,124,6,22,43,59,112,21,93,45,111,64,86,117,59,62,2,93,89,61,120,12,94,109,102};
volatile uint8_t verified = 0;

volatile uint8_t dataavailable = 0;
volatile uint8_t number;

volatile uint16_t counter;

ISR (INT0_vect)
{
	readdata(i);
}

void readdata(int bit)
{

	if(!(PIND &(1<<4)))
	{
		GPIOR0 |= (1<<bit);
		--i;
		
	}
	else if((PIND&(1<<4)))
	{
		GPIOR0 &= ~(1<<bit);
		--i;
	}
	
	if(bit<=0)
	{
		//cardreader[j++]= GPIOR0;
		if(GPIOR0 != pgm_read_byte(&verify[j++]))
		{
				datainvalid++;
				j = 0;
				GPIOR0 = 0;
				magswipetimeout=0;
				
				
				
				Magcardstop();
				timercounter=0;
				cardflag++;
				i=7;
				write(0x01,0);	// clears lcd
				lcd_position(0,0);
				lcd_print("Invalid Please");
				lcd_position(0,1);
				lcd_print("Try Again");
				//timer2_init();
				if(datainvalid==3)
				{
					buttonState &= ~(1<<Butt_Floor3);	// if data cannot be validated return and clear floor request from queue
					i = 7;
					Magcardstop();
					//timer2_stop();
					TIMSK2 &= ~(1<<OCIE2A); // turns off T2OCA ISR
					datainvalid=0;
					write(0x01,0);	// clear lcd
					updatemax7221floor(floorNum);
					return;
				}
				return;	
		}
		
		i=6;
		GPIOR0 = 0x00;
		if(j==82)
		{
			write(0x01,0);	// clear lcd
			lcd_position(0,0); // bring cursor to position 0
			lcd_print("Verified"); // print "verified" on the LCD when correct card is swiped
			++datavalid;
			Magcardstop(); // turn off MagCard reader
			j = 0;
			i = 7;
			TIMSK2 &= ~(1<<OCIE2A); // turns off T2OCA ISR
			magswipetimeout=0;
			dataflag=0;
			buttonState |= (1 << Butt_Floor3);
			if(curr_state == IDLE)
				statetransition(PROCESS);
		}
	}		
	/*if(dataflag)
	{
		validatedata();
		timer2_init();
		while(timercounter<31);	// 500ms timer delay
		timercounter=0;
		timer2_stop();
	}*/
}

void Magcardinit(void)
{
	EICRA |= (1<<ISC01);
	EIMSK |= (1<<INT0);
	DDRD &= ~((1<<DDD2)|(1<<DDD3));
	PORTD |= (1<<PORTD2);
	i=7;
	timer2_init();
	TIMSK2 |= (1<<OCIE2A);
}
void Magcardstop(void)
{
	EIMSK &= ~(1<<INT0);
	EICRA &= ~(1<<ISC01);
}
/*void validatedata(void)
{
	
	uint8_t match=0,j;
	dataflag=0;
	for(j=0; j<82; j++)
	{
		if(cardreader[j]==pgm_read_byte(&(verify[j])))
		++match;
	}
	
	
 	if(match>=70)
	{
		
		match=0;
		write(0x01,0);	// clear lcd
		lcd_position(0,0);
		lcd_print("Verified");
		++datavalid;
		
	}
	else if(j==82 && match <=70)
	{
		match=0;
		write(0x01,0);	// clears lcd
		lcd_position(0,0);
		lcd_print("Invalid Please");
		lcd_position(0,1);
		lcd_print("Try Again");
		++datainvalid;
		
	}
	
	memset(cardreader,0,82);
	
}
*/