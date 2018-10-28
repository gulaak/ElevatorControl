/*
 * GccApplication7.c
 *
 * Created: 9/24/2017 3:21:12 PM
 * Author : Gulaa
 */ 
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "Motordriver.h"
#include "MAX7221.h"
#include "sensors.h"
#include "buttonstate.h"
#include "finitestate.h"
#include "MagSwipe.h"
#include "LCD.h"


void initalizeglobalvar(void);
void floortransition(void);
void datarequest(void);
void solenoid_start(void);
void solenoid_stop(void);
void keypadset(void);

int main(void)
{
   	motorsetup();
	initalizeglobalvar();
	ADC_init(0b00);
	initializelcd();
	keypadset();
	solenoid_start();
	MAX7221_INIT();
	DDRD |= (1 << PORTD5);	//buzzer
	PORTD &= ~(1 << PORTD5);
	sei();
	curr_state = IDLE;
	//MAX7221_SetDigit(MAX7221_DIGIT0,pgm_read_byte(&SEGLUT[6]))
	
	updatemax7221floor(floorNum); // starts with current floor
    while (1) 
    {
		switch(curr_state)
		{
			case IDLE:
			break;
			case PROCESS:
			
			
			
			buttondelay();
				
			getNextFloor();
			if(nextFloor==floorNum)
			{
				if(buttonState == 0 && floorNum == 1)
				{
					clearButtons();
					if(buttonState &(1<<Butt_Emergency))
					{
							statetransition(EMERGENCY);
							break;
					}
					statetransition(IDLE);
				}
				else
				{	
					upDown = nextDir;
					updatemax7221dir(upDown);
					lcd_arrow(!upDown);
					statetransition(TRAN);
					clearButtons();
				}
			}
			else
				statetransition(MOVING);	//moves to moving state
				break;
			case MOVING:
				motorhandler(floorNum,nextFloor);
				
			//	MAX7221_SetDigit(MAX7221_DIGIT6,(buttonState & 0xFFFF));
			//	MAX7221_SetDigit(MAX7221_DIGIT7,(buttonState>>8 & 0xFFFF));
				if(buttonState == 0 && floorNum == 1)
				{
						lcdarrow_clear();
						max7221_cleardir();
						statetransition(IDLE);
				}
	
				else if((buttonState != 0)||(floorNum != 1))
					statetransition(PROCESS);
							
				if(curr_state!=IDLE)
					statetransition(TRAN);
					clearButtons();
				break;
			case TRAN:
				
				ADC_init(0b000); // ADC needs to sample for pressure sensor
				solenoid_stop(); // open doors
				pirsetup();
				floortransition();
				pirclear();
				if(buttonState &(1<<Butt_Emergency))
				{
					statetransition(EMERGENCY);
					break;
				}
				solenoid_start(); // close doors
							
				if((buttonState != 0)|(floorNum != 1))
					statetransition(PROCESS);
				
				else
				{
					MAX7221_SetDigit(MAX7221_DIGIT2,0x00);
					MAX7221_SetDigit(MAX7221_DIGIT3,0x00);
					lcdarrow_clear();
					statetransition(IDLE);
				}
				break;
				
			case EMERGENCY:
				if((buttonState &(1<<Butt_Emergency)) == 0)
				{
					/*while((buttonState &(1<<Butt_Emergency)) != 0)
					{
						;//if((buttonState &(1<<Butt_Emergency)) == 0)
							//break;
					}*/
					MAX7221_SetDigit(MAX7221_DIGIT4,0x00);
					MAX7221_SetDigit(MAX7221_DIGIT3,0x00);
					MAX7221_SetDigit(MAX7221_DIGIT2,0x00);
					MAX7221_SetDigit(MAX7221_DIGIT1,0x00);
					updatemax7221floor(floorNum);
					updatemax7221dir(upDown);
					statetransition(TRAN);
				}
				break;
			default:
				statetransition(IDLE);
				break;	
		}			
	}	
}
void floortransition()
{

	timer1_init(0b101,0b0000);	// timer set for 
	runtime=0;
	while(1)
	{
		if(runtime >= 2 && !(PIND & (1<<PIND3)))	// 12 seconds
		{
			
			timer1_stop();
			TCNT1=0;
			break;
		}
		
		timer2_init();
		timercounter=0;
		while(timercounter<20);	// 500ms timer delay
		timercounter=0;
		if(overload<=500)
		{
			MAX7221_SetDigit(MAX7221_DIGIT4, 0x08);
			write(0x01,0); // clears lcd
			lcd_position(0,0);
			lcd_print("Weight Capacity");
			lcd_position(5,1);
			lcd_print("Exceeded");
			
			
			while(1)			// system will wait 
			{
				if(overload >=500)
				{
			      MAX7221_SetDigit(MAX7221_DIGIT4, 0x00);
				  write(0x01,0);	// clears lcd
				  updatemax7221floor(floorNum);
				  updatemax7221dir(upDown);
				  lcd_arrow(!upDown);
				  runtime=0;
				  break;
				}
			}
			
		}
		
	}
}

void initalizeglobalvar(void)
{
	floorNum=1;
	nextFloor=1;
	buttonState=0;
	nextDir=0;
	upDown=0;
	adccount=0;
	runtime=0;
	dir=1;
	floorrequest=0;
	cardflag = 0;
	stopcondition=0;
	currentfloor=1;
	motorcounter;
	motorflag = 0;
	buttonState=0;
	pressed=0 ;
    floorNum=1;
	magswipetimeout=0;
	upDown=0 ; // 0 = Up 1 = Down
	i=7,j=0,dataflag=0,datainvalid=0;
	timercounter=0;
}

void datarequest(void)
{
	datavalid=0;
	write(0x01,0);	// swipe id
	lcd_position(0,0); // beginning of top row
	lcd_print("Please Swipe");
	lcd_position(0,1); // beginning of bottom row
	lcd_print("ID Card");
	Magcardinit();
}

void solenoid_start(void) // close door
{
	DDRD |= (1<<PORTD6);
	PORTD &= ~(1<<PORTD6);
}

void solenoid_stop(void) // open door
{
	PORTD |= (1<<PORTD6);
}

void keypadset(void)
{
	DDRB &= ~(1<<0);
	PORTB |= (1<<0);	// enables pullup
	PCF8574_SINGLEWRITE(0b00001111);
	PCICR |= (1<<PCIE0);
	PCMSK0 |= (1<<PCINT0);
}

void keypadclear(void)
{
	PORTB &= ~(1<<0);
	PCICR &= ~(1<<PCIE0);
	PCMSK0 &= ~(1<<PCINT0);
}
void pirsetup(void)
{
	DDRD &= ~(1<<3);	// INT1 pin as input
	PORTD |= (1<<3);	// setup pullup 
	EICRA |= (0b01<<ISC10);
	EIMSK |= (1<<INT1);
	runtime = 0;
}
void pirclear(void)
{
	EICRA &= ~(0b11<<ISC10);
	EIMSK &= ~(1<<INT1);
	
}

ISR (INT1_vect)
{
	solenoid_stop();
	runtime=0;
}

void buttondelay(void)
{
	timer1_init(0b101,0b0000); // transition prescaler, normal operation
	runtime = 0;
	
	while(runtime < 1); // waits for interrupt to set runtime to 1
	
	timer1_stop();
	return;	
}






