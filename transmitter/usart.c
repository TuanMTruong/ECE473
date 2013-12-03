/******************************************************************/
// Author: Tuan Truong
// USART functions
//
/******************************************************************/

//the includes
#include<avr/io.h>
#include"usart.h"

//global variables
uint8_t USART_BUFFER_CURRENT[26];
uint8_t USART_BUFFER_PREC[26];


//Sets up the usart on port c
void Setup_USARTC(){
	//Setup DDR (1 = output, 0 = input)
	PORTC.DIRSET = TX_PIN;
	PORTC.DIRCLR = RX_PIN;

	//Set up USART interrupts

	//Enable USART
	USARTC0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;

	//asynchronous, no parity, 1 stop bit, 8bit
	USARTC0.CTRLC = USART_CHSIZE_8BIT_gc;

	//Set baudrate to 57600 bps
	//9600 = 3317, -4
	USART_Baudrate_Set(USARTC0, 1079, -5);
	return;

}

void usart_send_byte(uint8_t data){
	while(!(USARTC0.STATUS &USART_DREIF_bm)){}
	USARTC0.DATA = data;
	return;
}

void usart_send_string(uint8_t *data, uint8_t length){
	int i =0;
	for(i=0; i<length; i++){
		usart_send_byte(*(data+i));
	}
	return;
}
