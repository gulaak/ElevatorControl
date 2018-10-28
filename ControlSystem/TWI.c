#include "TWI.h"
#include <util/delay.h>
#define F_CPU 16000000UL
void TWI_INIT(uint8_t CPU, uint8_t freq,uint8_t pre)
{
	
	TWBR = 72;//(CPU/(2*freq)-8);
	TWSR = (pre & TWPS1) | (pre & TWPS0) ;
	TWCR = (1 << TWEN);
	DDRC &=~((1<<4)|(1<<5));
	PORTC |= (1<<5)|(1<<4);	// internal pull ups needed in conjunction with external
}

uint8_t TWI_START()
{
	TWCR=0x00;
	TWCR = (1 << TWINT) | (1 << TWSTA)|(1<<TWEN);
	while(!(TWCR & (1 << TWINT)));	
	return (TWSR & TWI_SBITS);
}

void TWI_STOP()
{
	TWCR = (1 << TWEN) | (1 << TWSTO) | (1<<TWINT);
	
}

uint8_t TWI_SEND(uint8_t DATA)
{
	TWDR = DATA;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT)));
	return (TWSR & TWI_SBITS);
}

uint8_t TWI_READ(uint8_t AN,uint8_t *DATA)
{
	TWCR = (1 << TWINT) | (1 << TWEN);// | ((AN == ACK)?ACK:NACK);
	while(!(TWCR & (1 << TWINT)));
//	*DATA = TWDR;
	//return (TWSR & TWI_SBITS);
	return TWDR;
}