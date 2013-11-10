/****************************************************************/
// Author: Tuan Truong
// Date created: 10/27/13
// This is the source code for the reciever for the ECE473 project
// The Reciever is supposed to:
// - wait for incoming data from XBEE via USART
// - parce data 
// - send data out via SPI to 6 RGB LEDs

// Hardware description
// -




//includes
#include<avr/io.h>

// macros

//Sets up SPI as master with a prescale division of 4
//The reason for the slow speed for placing slave further away, at a slower speed
//there is a smaller chance so lost data.
void Setup_SPI(){
	
	return;
}

//Send data via SPI
uint8_t Send_SPI(uint8_t data){
	//Send data

	//wait for sending completetion
}


void Setup_USART(){
	//set intrrupt level (must enable PMIC first)
	//USART_XBEE.CTRLA = 

	//By default USART is asynchronous, parity is disable, and 1 stop bit
	//Setting USART data frame to 8bit
}



int main(){
	Setup_SPI();
	//Setup_USART();
	





	while(1){







	}

}
