/******************************************************************/
// Author: Tuan Truong
// USART functions
//
/******************************************************************/

//the includes
#include<avr/io.h>
#include"usart.h"
//#include<util/delay.h>
//global variables
uint8_t USART_BUFFER_CURRENT[26];
uint8_t USART_BUFFER_PREC[26];


/******************************************************************/
// Set up USART on PORTC
// 8Bit, 1SB, NP
// 57600 bps
/******************************************************************/
void Setup_USARTC(){
	//Setup DDR (1 = output, 0 = input)
	PORTC.DIRSET = TX_PIN;
	PORTC.DIRCLR = RX_PIN;

	USARTC0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;  //Enable USART
	USARTC0.CTRLC = USART_CHSIZE_8BIT_gc;           //asynchronous, no parity, 1 stop bit, 8bit
	USART_Baudrate_Set(USARTC0, 1079, -5);          //Set baudrate to 57600 bps
	return;

}

/******************************************************************/
// Send one byte over USART
/******************************************************************/
void usart_send_byte(uint8_t data){
	while(!(USARTC0.STATUS &USART_DREIF_bm)){}  //wait for transmition read
	USARTC0.DATA = data;                        //send data
	return;
}

/******************************************************************/
// send and array of byte over USART, set determine length
/******************************************************************/
void usart_send_string(uint8_t *data, uint8_t length){
	int i =0;                       //counter
	for(i=0; i<length; i++){        //loop till length
		usart_send_byte(*(data+i)); //send byte
	}
	return;
}
