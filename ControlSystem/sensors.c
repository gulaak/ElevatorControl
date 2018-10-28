#include "sensors.h"
#include "LCD.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "MAX7221.h"
#include "finitestate.h"
#include "buttonstate.h"
void ADC_init(uint8_t mux)
{
	ADCSRA |= (1<<ADEN)|(0b111<<ADPS0)|(1<<ADIE)|(1<<ADATE);
	ADMUX = (0b01<<REFS0)|((mux&0b1111)<<MUX0);
	ADCSRB |= (0b011<<ADTS0); // enables trigger source timer 0 Output compare match
	timer0_init();
	
	
}
void ADC_stop(void)
{
	
	ADCSRA = 0;//&= ~(1<<ADATE);	// stop adc sampling
}


ISR (ADC_vect)
{
	if(curr_state== MOVING)
	{
		readadc(1);	// reads distance
	}
	if(curr_state== TRAN)
	{
		readadc(2);	// reads pressure
	}
}
void timer0_init(void)
{
	TCCR0A |= (0b10<<WGM00);
	TCCR0B |= (0b101<<CS00);
	OCR0A = 0xFE;
	TIMSK0 = (1<<OCIE0A);
	TCNT0 = 0;
}

void timer0_stop(void)
{
	TCCR0B =0x00;
}
void readadc(uint8_t select)
{
	
	
	switch(select)
	{
		case 1:
		ADC_init(0b0000);
		distance = ADC;
		break;
		case 2:
		ADC_init(0b0001);
		overload= ADC;
		break;
		//case 3:
		//	ADC_init(0b0010);
		//	motion = ADC;
		//break;
		case 3:
		selector=1;
		break;
		
	}
}

ISR(TIMER0_COMPA_vect)
{
	if(adccount==32)
		return;
	++adccount;
}
void updatemax7221floor(uint8_t floor)
{
	if(!(buttonState&(1<<Butt_Emergency)))
		MAX7221_SetDigit(MAX7221_DIGIT0, pgm_read_byte(&SEGLUT[floor]));
		
	char string[40];
	if((EIMSK & (1<<INT0)))
		return;
	lcd_position(0,0);
	sprintf(string,"Current Floor:%d", floor);
	lcd_print(string);
}