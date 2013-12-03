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

#define OPEN_COM 0xAB
#define CLOSE_COM 0xBA
#define ADDR_MODE 0xCB
#define COLOR_MODE 0xBC

#define LED_NUM 32

//debug mode
#define HS_TESTING 0
#define SINGLE_COLOR_TESTING 1



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
	uint8_t temp = UDR1;
	if (temp == COLOR_MODE || temp == ADDR_MODE){
		if (ReadData(get_buff_location()-1) == OPEN_COM){
			set_buff_location(0);
			PushData(OPEN_COM);
		}
	}
	
	PushData(temp);
}


ISR(USART1_UDRE_vect){
	LED_PORT ^= (1<<LED_PIN);
	UDR1 = 'a';
	_delay_ms(1);
}

//Bad interrupt catcher
ISR(BADISR_vect){
	while(1){
		LED_PORT ^= (1<<LED_PIN);
		_delay_ms(100);
	}
}


uint8_t verify_buff(){

	if (ReadData(0) == OPEN_COM ){
		if (ReadData(1) == COLOR_MODE){
			if (ReadData(3) == CLOSE_COM){
				return 1;
			}
		}
		else if (ReadData(1) == ADDR_MODE){
			if (ReadData(LED_NUM+2) == CLOSE_COM){
				return 1;
			}
		}
	}
	return 0;

}

void display_mode(uint8_t *data){
	uint8_t i =0;
	
	if (ReadData(1)== COLOR_MODE){
		hs_convert(ReadData(2), 0, data);
		_delay_ms(5);
		for(i=0; i<LED_NUM; i++){
			Send_SPI_array(data, (data+1), (data+2),1);
		}
	}
	else if (ReadData(1) == ADDR_MODE){
		_delay_ms(5);
		for(i=0; i<LED_NUM; i++){
			hs_convert(ReadData(i+2), 0, data);
			Send_SPI_array(data, (data+1), (data+2),1);

		}
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
	uint8_t i = 0;
	_delay_ms(200);

	LED_PORT |= (1<<LED_PIN);

	Fill_array(red_array, 32, 0x00);
	Fill_array(blue_array, 32, 0x00);
	Fill_array(green_array, 32, 0x00);

	set_buff_location(0);
	PushData(OPEN_COM);
	PushData(ADDR_MODE);
	for(i =0; i<LED_NUM; i++){
		PushData((i+1) * 7);
	}
	PushData(CLOSE_COM);

	sei();
	_delay_ms(200);

	while(1){
#if HS_TESTING
		_delay_ms(100);



		Send_SPI_array(red_array, green_array, blue_array, 32);


		hs_convert(temp, 0, hs_rgb);
		Fill_array(red_array, 32, *hs_rgb);
		Fill_array(green_array, 32, *(hs_rgb+1));
		Fill_array(blue_array, 32, *(hs_rgb+2));
		temp++;
#endif

#if SINGLE_COLOR_TESTING
		if(verify_buff()){
		//	LED_PORT ^= 1<<LED_PIN;
			display_mode(hs_rgb);
		}
		_delay_ms(500);
//		UDR1 = 'a';
	
#endif

	}

}
