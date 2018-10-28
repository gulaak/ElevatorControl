/*
 * Motordriver.h
 *
 * Created: 10/17/2017 7:14:28 PM
 *  Author: Gulaa
 */ 


#ifndef MOTORDRIVER_H_
#define MOTORDRIVER_H_
volatile uint8_t runtime;
volatile uint8_t dir;
volatile uint8_t floorrequest;
volatile uint8_t stopcondition;
volatile uint8_t currentfloor;
volatile uint8_t motorcounter;
volatile uint8_t pressed ;
volatile uint8_t timercounter;
volatile uint8_t magswipetimeout;
uint8_t motorflag;
void motorstart(void);
void timer2_init(void);
void timer2_stop(void);
void motorstop(void);
void timer1_init(uint8_t ps, uint8_t modesel);
void timer1_stop(void);
void pinchangesetup(void);
void motorhandler(uint8_t currentfloor, uint8_t floorrequested);
void updatemax7221floor(uint8_t floor);
void motorsetup(void);
void updatemax7221dir(uint8_t dir);

#endif /* MOTORDRIVER_H_ */