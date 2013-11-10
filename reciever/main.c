/****************************************************************/
// Author: Tuan Truong
// Date created: 10/27/13
// This is the source code for the reciever for the ECE473 project
// The Reciever is supposed to:
// - wait for incoming data from XBEE via USART
// - parce data 
// - send data out via SPI to 6 RGB LEDs

// Hardware description
// - SPI SS 	PB0
// - SPI SCLK 	PB1
// - SPI MOSI	PB2
// - SPI MISO	PB3
// - USART RXD1	PD2
// - USART TXD1	PD3
// - LED	PD6

/****************************************************************/




//includes
#include<avr/io.h>
#include<avr/interrupt.h>

// macros
#define LEDPORT PORTD
#define LED	6
#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define CPU_16MHz 0x00
#define CPU_4Mhz 0x02

//global variables
uint8_t dataring[50];
uint8_t ring counter= 0;

//Sets up SPI as master with a prescale division of 4
//The reason for the slow speed for placing slave further away, at a slower speed
//there is a smaller chance so lost data.
void Setup_SPI(){
	//Set up SPI control register
	SPCR = (1<<SPE) | (1<<MSTR) | (3<<SPR0);

	return;
}

//Send data via SPI
void Send_SPI(uint8_t data){
	//Send data
	SPDR = data;
	//wait for sending completion 
	while(!(SPSR & (1<<SPIF)));
	
	return;
}


void Setup_USART(){
	
	//By default USART is asynchronous, parity is disable, and 1 stop bit
	UCSR1A = (1<<U2X1);
	UCSR1B = (1<<RXEN1) | (1<<TXEN1);
	//Setting USART data frame to 8bit
	UCSR1C = (3<<UCSZ10);

	//set baud rate to 38.4K
	UDRR1 = 12;
	return;
}



//USART receiving interrupt
//When data is received it will be stored in a buffer waiting to be accessed
ISR(USART1_RX_vect){
	//Store data

}

//Bad interrupt catcher
ISR(BADISR_vect){
	while(1){
		LEDPORT ^= (1<<LED);
		_delay_ms(200);
	}
}

//functions that access data buffer
void PushData(uint8_t data){

}

uint8_t PopData(){

}

uint8_t ReadData(uint8_t location){
	return dataring[location];
}


int main(){
	//Set clock to 4 Mhz
	CPU_PRESCALE(CPU_4MHz);
	Setup_SPI();
	Setup_USART();
	





	while(1){







	}

}
