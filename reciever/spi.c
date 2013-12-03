/*
 * spi.c
 *
 * Created: 12/2/2013 2:05:47 PM
 *  Author: Tuan
 */ 
//includes
#include<avr/io.h>
#include "spi.h"

//Sets up SPI as master with a prescale division of 4
//The reason for the slow speed for placing slave further away, at a slower speed
//there is a smaller chance so lost data.
void Setup_SPI(){
	//set up DDR for SPI, 1 = output, 0 = input
	DDRB |= (1<< SS_PIN) | (1<< SCLK_PIN) | (1<< MOSI_PIN);
	DDRB &= ~(1<<MISO_PIN);
	//Set up SPI control register
	SPCR = (1<<SPE) | (1<<MSTR) | (3<<SPR0);

	return;
}


//Send data via SPI
void Send_SPI_byte(uint8_t data){
	//Send data
	SPDR = data;
	//wait for sending completion
	while(!(SPSR & (1<<SPIF)));
	
	return;
}


//Send data to strip
void Send_SPI_array( uint8_t *red, uint8_t *green, uint8_t *blue, uint8_t lenght){
    uint8_t counter;
    
    for (counter = 0; counter<lenght; counter++) {
        Send_SPI_byte(*(red+counter));
        Send_SPI_byte(*(green+counter));
        Send_SPI_byte(*(blue+counter));
    }
    return;
    
}