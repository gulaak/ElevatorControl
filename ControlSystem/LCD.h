/*
 * LCD.h
 *
 * Created: 10/17/2017 5:02:29 PM
 *  Author: Gulaa
 */ 

#include <avr/io.h>

#ifndef LCD_H_
#define LCD_H_
#define LCD_ADDR (0x027<<1)
#define En 0b00000100  // Enable bit
#define Rw 0b00000010  // Read/Write bit
#define Rs 0b00000001 // Register select bit
void write(uint8_t data, uint8_t rsmode);
void initializelcd(void);
void lcd_position(unsigned char x, unsigned char y);
void lcd_print (char * str);
void lcd_arrow (uint8_t direction);
void pulse(unsigned char data);
void LCD_SEND(unsigned char DATA);

#endif /* LCD_H_ */