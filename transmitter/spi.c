/******************************************************************/
// Author: Tuan Truong
// SPI functions
//
/******************************************************************/


//the includes
#include<avr/io.h>
#include"spi.h"
#include<util/delay.h>


//sets up the SPI on port C
void Setup_SPIC(){
	//Setup DDR for SPIC (1 = output, 0 = input)
	PORTC.DIRSET = SS_PIN | SCK_PIN | MOSI_PIN;
	PORTC.DIRCLR = MISO_PIN;
    
	//Set up for 2x speed, enabled, master, at 64 prescale
	SPIC.CTRL = SPI_CLK2X_bm | SPI_ENABLE_bm | SPI_MASTER_bm |SPI_PRESCALER_DIV64_gc;
	
	return;
}

//sets up the SPI on port D
void Setup_SPID(){
	//Setup DDR for SPIC (1 = output, 0 = input)
	PORTD.DIRSET = SS_PIN | SCK_PIN | MOSI_PIN;
	PORTD.DIRCLR = MISO_PIN;
    
	//Set up for 2x speed, enabled, master, at 64 prescale
	SPID.CTRL = SPI_CLK2X_bm | SPI_ENABLE_bm | SPI_MASTER_bm |SPI_PRESCALER_DIV64_gc;
	
	return;
    
}

void LCD_send_byte(uint8_t data){
	PORTD.OUTCLR = LCD_SS_PIN;

	SPIC.DATA = 0x00;
	while(!(SPIC.STATUS & SPI_IF_bm)){};

	PORTD.OUTSET = LCD_SS_PIN;
}


void Setup_LCD(){
	//Setup DDR
	PORTD.DIRSET = LCD_SS_PIN | LCD_RST_PIN | LCD_SIG_PIN | LCD_LIGHT_PIN;

	PORTD.OUTSET = LCD_RST_PIN;
	PORTD.OUTCLR = LCD_SIG_PIN;

	_delay_ms(20);

	LCD_send_byte(0xA2);
	_delay_us(1);
	LCD_send_byte(0xA1);
	_delay_us(1);
	LCD_send_byte(0xC8);
	_delay_us(1);
	LCD_send_byte(0xA4);
	_delay_us(1);
	LCD_send_byte(0x40);
	_delay_us(1);
	LCD_send_byte(0x25);
	_delay_us(1);
	LCD_send_byte(0x81);
	_delay_us(1);
	LCD_send_byte(0x6F);
	_delay_us(1);
	LCD_send_byte(0x2F);
	_delay_us(1);
	LCD_send_byte(0xAF);
	_delay_us(1);

	PORTD.OUTSET LCD_SS_PIN;
}


void LCD_update(){

}

//read from SPIC which has the buttons and encoders on it via a shift register
uint8_t Read_Buttons(){
	uint8_t temp;

	//Pull SHIFT_LOAD low for >= 120 ns to load in button data
	PORTE.OUTCLR = SHIFT_LOAD_PIN;
	//_delay_us(1);
	PORTE.OUTSET = SHIFT_LOAD_PIN;

	//Pull SHIFT_LATCH low to send out serial data.
	PORTE.OUTCLR = SHIFT_LATCH_PIN;

	//send dummy spi 
	SPIC.DATA = 0x00;
	while(!(SPIC.STATUS & SPI_IF_bm)){};
	temp = SPIC.DATA;


	//After data read and saved set SHIFT_LOAD and SHIFT_LATCH
	PORTE.OUTSET = SHIFT_LATCH_PIN | SHIFT_LOAD_PIN;
	return(temp);
}


