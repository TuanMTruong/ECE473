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


//define CPU clock
#define F_CPU 16000000UL

//includes
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include "spi.h"
#include "usart.h"
#include "hs_converter.h"

// macros
#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define CPU_16MHz 0x00
#define CPU_4MHz 0x02

#define LED_PIN 6
#define LED_PORT PORTD






//Set up Data Direction Registers for LEDs, enables, and latches
void Setup_DDR(){
	//1 = output, 0 = input
	//on board LED
	DDRD = (1<<LED_PIN);

}

void Fill_array(uint8_t *array, uint8_t lenght, uint8_t data){
    uint8_t counter;
    
    for (counter =0; counter<lenght; counter++) {
        *(array+counter) = data;
    }
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
		LED_PORT ^= (1<<LED_PIN);
		_delay_ms(200);
	}
}





int main(){
	//Set clock to 4 Mhz
	CPU_PRESCALE(CPU_16MHz);
	Setup_DDR();
	Setup_SPI();
	Setup_USART();
    
    uint8_t hs_rgb[3];
	
    uint8_t red_array[50];
    uint8_t blue_array[50];
    uint8_t green_array[50];
    
    uint8_t temp=0;
    
    _delay_ms(200);
    
    
    Fill_array(red_array, 32, 0x00);
    Fill_array(blue_array, 32, 0x00);
    Fill_array(green_array, 32, 0x00);
    
	while(1){
        _delay_ms(5);
        
        
        
        Send_SPI_array(red_array, green_array, blue_array, 32);
    
        
        LED_PORT ^= (1<<LED_PIN);
        hs_convert(temp, 0, hs_rgb);
        Fill_array(red_array, 32, *hs_rgb);
        Fill_array(green_array, 32, *(hs_rgb+1));
        Fill_array(blue_array, 32, *(hs_rgb+2));
        temp++;



	}

}
