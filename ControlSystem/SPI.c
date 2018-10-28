#include "SPI.h"


void SPI_Init(uint8_t dord, uint8_t mod, uint8_t div)
{

	SPI_DDR_REG |= (1<<SCK)|(1<<MOSI)|(1<<SS);				 // SCK, MOSI and SS as outputs
	SPI_DDR_REG &= ~(1<<MISO);								 // MISO as input
	
	SPCR = (1<<MSTR)|((div&3)<<SPR0)|(1<<SPE)|((mod&3) << CPHA) | ((dord&1) << DORD);  // Set as Master/ divided clock by 128/ Enable SPI
	SPSR = (1 << SPI2X);
	SPI_PORT_REG |= (1<<SS);
	
}

void SPI_Send(uint8_t Data)
{							// SS pin set?
	SPDR = Data;											// Set data to SPDR (SPI Data Reg)
	while(!(SPSR & (1<<SPIF)));							// verify send complete
}

uint8_t SPI_Receive()
{
	uint8_t Data;
	while(!(SPSR & (1<<SPIF)));						// wait until all data is received
	Data = SPDR;										// set SPDR to char Data
	return Data;
}

void SPI_ChipSelect(uint8_t VALUE)
{
	if (VALUE==1) SPI_PORT_REG |= (1 << SS);
	else
	SPI_PORT_REG &=~(1 << SS);
}
