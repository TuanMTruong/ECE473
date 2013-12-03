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


