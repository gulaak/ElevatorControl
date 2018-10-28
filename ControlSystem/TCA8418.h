#include "TWI.h"

#ifndef F_CPU
	#define F_CPU					16000000UL
#endif

#define TCA8418_PRE					1
#define TCA8418_FREQ				400000
#define TCA8418_ADDR				(0x34 << 1)

#define TCA8418_INT_STAT			0x2
#define TCA8418_KEY_LCK_EC			0x3
#define TCA8418_KEY_EVENT_A			0x4

#define TCA8418_K_INT				0x0

void TCA8418_INIT();
uint8_t TCA8418_SINGLEREAD(uint8_t ADDR);
void TCA8418_SINGLEWRITE(uint8_t ADDR, uint8_t DATA);
uint8_t TCA8418_VERIFY_PRESS();
uint8_t TCA8418_GET_SINGLE_PRESS();
uint8_t TCA8418_GET_NUM_EVENTS();
void TCA8418_CLR_INT();
void buttonled(void);
