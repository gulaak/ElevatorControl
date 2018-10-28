
#include <util/delay.h>
#define F_CPU 16000000UL
#include "LCD.h"
#include "TWI.h"



void initializelcd(void)
{
  TWI_INIT(F_CPU, 100000,0);
  write(0x33,0);
  _delay_us(5);
  write(0x32,0);
  _delay_us(5);
  write(0x28,0);
  _delay_us(5);
  write(0x0e,0);
  _delay_us(5);
  write(0x01,0);
  _delay_us(5);
  write(0x06,0);
  _delay_us(5);
  write(0x0C,0);

  
}



void lcd_position(unsigned char x, unsigned char y)
{
	unsigned char firstaddress[] = {0x80, 0xC0, 0x94, 0xD4};
	
	write(firstaddress[y] + x, 0);
	_delay_us(10);
}

void lcd_print( char * str)
{
	int i= 0;
	while (str[i] != 0)
	{
		
		write(str[i], 1);
		++i;
		_delay_us(10);
	}
	return;
}

void lcd_arrow(uint8_t direction)
{
	write(0x40,0);	//cg ram
	uint8_t i=0,j=0;
	uint8_t arraybase[]= {0xE,0xE,0xE,0xE,0xE,0xE,0xE,0xE};
	uint8_t arrayheadup[]={0x00,0x00,0x00,0x00,0x00,0x04,0xE,0x1F};
	uint8_t arrayheaddown[]={0x1F,0x0E,0x04,0x00,0x00,0x00,0x00,0x00};
		
	if(direction==1)
	{
		
		while(i!=8)
		{
			write(arraybase[i],1);
			++i;
		}
		lcd_position(15,1);
		write(0x00,1);	// prints character for cg ram
		write(0x48,0);	// cg ram
		while(j!=8)
		{
			write(arrayheadup[j],1);
			++j;
		}
	
		lcd_position(15,0);
		write(0x01,1);	// second character for cg ram
	}
	
	else
	{
		while(i!=8)
		{
			write(arraybase[i],1);
			++i;
		}
		lcd_position(15,0);
		write(0x00,1);
		write(0x48,0);
		while(j!=8)
		{
			write(arrayheaddown[j],1);
			++j;
		}
	
		lcd_position(15,1);
		write(0x01,1);
		
		
	}
	
	
}

void pulse(unsigned char data)
{
	LCD_SEND(En | data);
	_delay_us(1);
	LCD_SEND(data & ~En);
	_delay_us(50);
}


void bitdata(unsigned char data)
{
	
	LCD_SEND(data);
	pulse(data);

	
}

void write(unsigned char data, uint8_t rsmode) // writes to either data or command register
{
	uint8_t uppernibble= (data & 0xF0); // masks nibbles
	uint8_t lowernibble = ((data<<4) & 0xF0);
	if(rsmode==1)
	{
		
		bitdata(Rs | uppernibble);
		bitdata(Rs | lowernibble);
	}
	else
	{
		
		bitdata(uppernibble & ~Rs);
		bitdata(lowernibble & ~Rs);
	}
}



void LCD_SEND(unsigned char DATA)
{
	uint8_t STATUS = 0;
	STATUS = TWI_START();
	if(STATUS == TWI_START_VAL)
	STATUS = TWI_SEND(LCD_ADDR | TWI_WRITE_BIT);
	if(STATUS == TWI_WRITE_ACK)
	STATUS = TWI_SEND(DATA | 0b00001000); // keeps backlight on
	if(STATUS == TWI_WDATA_ACK)
	TWI_STOP();
	

}

void lcdarrow_clear(void)
{
	lcd_position(15,0);
	lcd_print(" ");
	lcd_position(15,1);
	lcd_print(" ");
}