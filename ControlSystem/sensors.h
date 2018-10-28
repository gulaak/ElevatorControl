/*
 * sensors.h
 *
 * Created: 10/17/2017 8:22:22 PM
 *  Author: Gulaa
 */ 

#include <avr/io.h>
#ifndef SENSORS_H_
#define SENSORS_H_
volatile uint8_t selector;
volatile uint16_t distance;
volatile uint16_t overload;
volatile uint16_t motion;
volatile uint8_t dir;
volatile uint8_t adccount;
void readadc(uint8_t select);
void ADC_init(uint8_t mux);
void timer0_init(void);
void updatemax7221(uint8_t floor);
void timer0_stop(void);
void ADC_stop(void);
void MAX7221_SEND(uint8_t ADDR, uint8_t DATA);
volatile uint8_t testvariable;





#endif /* SENSORS_H_ */