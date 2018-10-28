//#ifndef SPI
//#define SPI
#include <avr/io.h>
//#define F_CPU 16000000UL

//#ifdef atmega328p						//specific to atmega328p board

/*
PortB5 = SCK
PortB4 = MISO
PortB3 = MOSI
PortB2 = SS
*/

#ifndef SCK
//# warning "Define SCK (default PORTB)"
#define SCK PORTB5
#endif

#ifndef MISO
//# warning "Define MISO if needed"
#define MISO PORTB4
#endif

#ifndef MOSI
//# warning "Define MOSI if needed"
#define MOSI PORTB3
#endif

#ifndef SS
//# warning "Define SS if needed"
#define SS PORTB2
#endif

#define SPI_DDR_REG DDRB
#define SPI_PORT_REG PORTB

void SPI_Init(uint8_t dord, uint8_t mod, uint8_t div);
void SPI_Send(uint8_t Data);
uint8_t SPI_Receive();
void SPI_ChipSelect(uint8_t VALUE);