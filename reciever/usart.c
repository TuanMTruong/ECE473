/*
 * usart.c
 *
 * Created: 12/2/2013 2:04:21 PM
 *  Author: Tuan
 */ 
//includes
#include<avr/io.h>
#include "usart.h"


//global variables
uint8_t dataring[buffersize];
uint8_t ringcounter= 0;


void Setup_USART(){
	//set up DDR for USART, 1 = output, 0 = input
	DDRD |= (1<<TX_PIN);
	DDRD &= ~(1<<RX_PIN);
	//By default USART is asynchronous, parity is disable, and 1 stop bit
	//UCSR1A = (1<<U2X1);
	UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1);
	//Setting USART data frame to 8bit
	UCSR1C = (3<<UCSZ10);

	//set baud rate to 57.6K
	UBRR1 = 16;
	return;
}

//functions that access data buffer
void PushData(uint8_t data){
	if(ringcounter >= buffersize){
		ringcounter = 0;
	}
	dataring[ringcounter] = data;
	ringcounter++;
}

uint8_t PopData(){
	return dataring[ringcounter];
}

uint8_t ReadData(uint8_t location){
	return dataring[location];
}

uint8_t get_buff_location(){
	return ringcounter;
}

void set_buff_location(uint8_t location){
	ringcounter = location;
	return;
}

