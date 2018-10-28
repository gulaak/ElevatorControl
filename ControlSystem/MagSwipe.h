/*
 * MagSwipe.h
 *
 * Created: 10/30/2017 9:01:23 PM
 *  Author: Gulaa
 */ 


#ifndef MAGSWIPE_H_
#define MAGSWIPE_H_

volatile int8_t i,j;
volatile uint8_t dataflag;
volatile uint8_t datavalid;
volatile uint8_t datainvalid;
volatile uint8_t cardflag;
void Magcardinit(void);
void Magcardstop(void);
void validatedata(void);
void resetreader(void);


#endif /* MAGSWIPE_H_ */