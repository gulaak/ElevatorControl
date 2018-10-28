#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include "Motordriver.h"
#include "MagSwipe.h"
#include <avr/interrupt.h>
#include "LCD.h"
#include "MAX7221.h"
#include "finitestate.h"
#include "sensors.h"
#include "buttonstate.h"

void motorstart(void)
{
	
	//timer2_init();
	timer1_init(/*0b101*/0b011, 0b0101);	//setup for pwm
	DDRB |= (1<<1);	// pwm pin
	stopcondition=0;
}


void motorhandler(uint8_t currentfloor, uint8_t floorrequested)
{
	
	
	 if(upDown==0)	// motor will go up
	 {
		 updatemax7221dir(upDown);
		 lcd_arrow(!upDown);
		 PORTC |= (1<<2);
		 PORTC &= ~(1<<3); // motor configured to go up
	 }
	 else
	 {
		 updatemax7221dir(upDown);
		 lcd_arrow(!upDown);
		 PORTC &= ~(1<<2);	// motor configured to go down
		 PORTC |= (1<<3);
	 }
	
	
//	if(floorrequested==currentfloor)
//	{
//		upDown= nextDir;
//		motorflag = 1;
//		return;
		
//	}
	motorstart();
	while(1)
	{
		if(distance>=500) //next floor
		{
			//_delay_ms(100);
			ADC_stop();	// stops sampling to ensure it will not trigger multiple times.
			TCNT0 = 0;	//resets clock
			adccount = 0;
			distance=0;	// make sure this will not trigger
			if(upDown==0 && floorNum!=5)
			{
				updatemax7221floor(++floorNum);				/* Monitors Distance sensor */
				
				
			}
			else if(upDown == 1 && floorNum != 1)
			{
				
				updatemax7221floor(--floorNum);
				
			}
		}
		else if(adccount>=32)
		{
			ADC_init(0b00);	// allows sampling again after 4 seconds.
			TCNT0 = 0;
			adccount=0;
		}
		else if(floorrequested == floorNum)
		{
			upDown = nextDir;
			updatemax7221dir(upDown);
			lcd_arrow(!upDown);
			motorstop();
			clearButtons();
			break;
		}
	
	}
}
	

void motorsetup(void)
{
	DDRC |= (1<<2)|(1<<3);	// H-Bridge Logic Control
	DDRB |= (1<<1);			// PWM pin for motor speed control
}

void timer2_init(void)
{
	TCCR2A |= (0b00<<WGM20); //normal mode
	TCCR2B |= (0b111<<CS20); // 1024 prescaler
	TIMSK2 |= (1<<TOIE2);	// enable interrupt

	//TCCR2A |= (0b11<<WGM20)|(0b10<<COM2B0);
	//TCCR2B |= (0b00<< WGM22)|(0b101<<CS20);
	//TIMSK2 = (1<<TOIE2);
	//OCR2B = 0;
	TCNT2=0;
	OCR2A= 0xFE;
	//(((((dutycycle)/100.0)*256)-1));
	
}
void motorstop()
{
	stopcondition=1;
	while(OCR1A!=0);	// wait till pulse width is 0
	timer1_stop();
	//timer2_stop();		// shut off timer that generates waveform
	TCNT1=0;
	PORTD &= ~(1<<1); //turn PWM off
	
}
ISR (TIMER2_COMPA_vect)
{
	++dataflag;
	
	if(dataflag==255)
	{
		++magswipetimeout;
		dataflag=0;
	}
		
	if(magswipetimeout == 7)
	{
		magswipetimeout=0;
		TIMSK2 &= ~(1<<OCIE2A);
		Magcardstop();
		write(0x01,0);	// clear lcd
		updatemax7221floor(floorNum);
	}
		
}

void timer2_stop(void)
{
	TCCR2B = 0x00;
	TCCR2B = 0x00;
	TCCR2A= 0x00;
	TIMSK2= 0x00;
	
	OCR2B =0;
}
ISR(TIMER2_OVF_vect)
{
	
	timercounter++;
	if(cardflag == 1 && timercounter >= 20)
	{
			Magcardinit();
			cardflag = 0;
			//timer2_stop();
	}
	//if(stopcondition!=1 && OCR2B != 255)
	//OCR2B +=1;
	//if(stopcondition ==1 && OCR2B != 0)
	//OCR2B -=1;
	//else
		//return;
}

void timer1_init(uint8_t ps,uint8_t modesel)
{
	TCCR1B |= (((modesel&0b1100)>>2)<< WGM12);	// 0b0000 normal operation 0b1111 pwm operation
	TCCR1A  |= (((modesel&0b0011))<<WGM10);
	TCCR1B |= ((ps&0b111)<<CS10);			// 0b001 no prescale	0b101 transition prescale 
	if(modesel != 0)
	{
		TIMSK1 |= (1<<TOIE1);
		TCCR1A |= (0b10<<COM1A0);	//setup for pwm
	}
	else
	{
		OCR1A = 65000;
		TIMSK1 |= (1<<OCIE1A);
	}
	
	TCNT1=0;
	
}
void timer1_stop()
{
	TCNT1=0;
	TCCR1B = 0b000;
	TCCR1A = 0b000;
	OCR1A = 0x00;
}

ISR (TIMER1_OVF_vect)
{
	if(upDown == 0)	//up
	{
	if(stopcondition!=1 && OCR1A != 232)
	OCR1A +=1;
	if(stopcondition ==1 && OCR1A != 0)
	OCR1A -=1;
	}
	if(upDown == 1)	//down
	{
		if(stopcondition!=1 && OCR1A != 160)
			OCR1A +=1;
		if(stopcondition == 1 && OCR1A != 0)
			OCR1A -=1;
		
	}
	return;
}

ISR (TIMER1_COMPA_vect)
{
	++runtime;
}

ISR (PCINT2_vect)
{
	_delay_ms(30);
	if( (PIND & ((1 << PIND1)|(1 << PIND6)|(1<<PIND7))) ==0 )
	{
		do					//de-bounces the button
		{
			while( (PIND & ((1 << PIND1)|(1 << PIND6)|(1<<PIND7))) != 0 )
			{
				_delay_ms(20);
			}
		} while ( (PIND & ((1 << PIND1)|(1 << PIND6)|(1<<PIND7))) != 0 );

	}
	else
	{
		do
		{
			while( (PIND & ((1 << PIND1)|(1 << PIND6)|(1<<PIND7))) == 0)
			{
				_delay_ms(20);
			}
		} while ( (PIND & ((1 << PIND1)|(1 << PIND6)|(1<<PIND7))) == 0);	//button de-bounced
	}
	pressed = 1;
	if((PIND & (1 << PIND1)) == 0)
	{
		buttonState |= (1<<Butt_Floor3);
		if(curr_state == IDLE)
			statetransition(PROCESS);
	}
	if((PIND & (1 << PIND6)) == 0)
	{
		buttonState |= (1<<Butt_Floor2);
		if(curr_state == IDLE)
			statetransition(PROCESS);
	}
	
	if((PIND & (1 << PIND7)) == 0)
	{
		buttonState |= (1<<Butt_Floor1);
		if(curr_state == IDLE)
			statetransition(PROCESS);
	
	}
	MAX7221_SetDigit(MAX7221_DIGIT4,(buttonState & 0xFFFF));
	MAX7221_SetDigit(MAX7221_DIGIT5,(buttonState>>8 & 0xFFFF));
	

}


void pinchangesetup(void)
{
	DDRD &= ~((1<<PORTD1)|(1 << PORTD6)|(1 << PORTD7)); //sets PORTD1,D4,D7 to button inputs
	PORTD |= ((1<<PORTD1)|(1 << PORTD6)|(1 << PORTD7));	//sets pull-up resistors for the button input
	

	PCICR |= (1 << PCIE2); //enable pin change interrupts
	PCMSK2 |= (1 << PCINT17)|(1 << PCINT22)|(1 << PCINT23);
	PCIFR |= (1<<PCIF0);	//clears the flag to prevent entering the PCINT ISR
}