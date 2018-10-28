#include <avr/io.h>

#define TWI_SBITS			0xF8
#define TWI_START_VAL		0x8
#define TWI_REPSTART_VAL	0x10
#define TWI_WRITE_ACK		0x18
#define TWI_WRITE_NACK		0x20
#define TWI_WDATA_ACK		0x28
#define TWI_WDATA_NACK		0x30
#define TWI_ARB_LOST		0x38
#define TWI_READ_ACK		0x40
#define TWI_READ_NACK		0x48
#define TWI_RDATA_ACK		0x50
#define TWI_RDATA_NACK		0x58
#define TWI_WRITE_BIT		0
#define TWI_READ_BIT		1
#define ACK					1
#define NACK				0

void TWI_INIT(uint8_t CPU,uint8_t freq, uint8_t pre);
uint8_t TWI_START();
void TWI_STOP();
uint8_t TWI_SEND(uint8_t DATA);
uint8_t TWI_READ(uint8_t AN,uint8_t *DATA);