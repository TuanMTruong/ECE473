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
#include "twi.h"

/****************************************************************/
// Macros
/****************************************************************/

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n)) //set system clock 
#define CPU_16MHz 	0x00
#define CPU_4MHz 	0x02

#define LED_PIN 	6	//pin of on board LED
#define LED_PORT 	PORTD  	//PORT of on board LED

#define OPEN_COM 	0xFF	//tell system ready catch data
#define CLOSE_COM 	0xBA	//tell system mode of operation
#define ADDR_MODE 	0xCB	//set operational mode to addressable
#define COLOR_MODE 	0x0C	//set operational mode to full scale color
#define RAINBOW_MODE 	0xAC	//set operational mode to rainbow scale
#define RAINBOW2_MODE 	0xCA	//set operational mode to rainbow 2 mode
#define TIME_MODE 	0xEE	//set operational to show time on LED
#define TIME_MODE_2 	0xAA	//originally intended to speed up time

#define LED_NUM 	32	//number of LED per strip

#define HS_TESTING 	0	//Display full hue and saturation spectrum
#define COLOR_TESTING 	0	//Full operational mode (does everything)
#define TIME_TESTING 	0	//Display time more on led 
#define RTC_TEST 	1	//send rtc data over usart for debug of twi

#define T_SEC 		8	
#define T_MIN 		7
#define T_HOUR 		6
#define T_COUNT 	9

/****************************************************************/
// Global variables
/****************************************************************/

uint8_t rtc_array[10];
uint8_t time_array[35];
uint8_t flag = 0;


/****************************************************************/
//Set up Data Direction Registers for LEDs, enables, and latches
//1 = output, 0 = input
/****************************************************************/
void Setup_DDR(void){
	DDRD = (1<<LED_PIN);    //set on baord LED to output

}

/****************************************************************/
//used to fill an a array with a single byte
/****************************************************************/
void Fill_array(uint8_t *array, uint8_t lenght, uint8_t data){
	uint8_t counter;    //create a temo counter

	for (counter =0; counter<lenght; counter++) {
		*(array+counter) = data;    //loop through array and fill with data
	}
	return;
}

/****************************************************************/
//set up timmer for BCD prototype clock
/****************************************************************/
void Setup_Timer(void){
	TCCR1A = 0;
	TCCR1B = (1<<WGM12) | (4<<CS10);
	TIMSK1 = 1<<OCIE1A;
	TCNT1 = 0;
	OCR1A = 60000;
	return;

}


/****************************************************************/
//Take time data and convert to BCD for LED time mode (not rtc)
/****************************************************************/

void fill_time(uint8_t hour, uint8_t min){
	uint8_t i =0;

	uint8_t hour_1 = (hour / 10)<<2;
	uint8_t hour_0 = (hour % 10)<<4;
	uint8_t min_1 = (min /10);
	uint8_t min_0 = (min % 10)<<2;


	time_array[2] = hour_1;
	time_array[3] = hour_0;
	time_array[4] = min_1;
	time_array[5] = min_0;


	return;
}

/****************************************************************/
//RTC: read and write to RTC 
/****************************************************************/
void rtc_write(uint8_t addr, uint8_t data){
    uint8_t send_array[2];
    send_array[0] = addr;
    send_array[1] = data;
    twi_write(RTC_ADDR, send_array, 2);
}

void rtc_read(){
    uint8_t send_array[2];
    send_array[0] = 0x00;
    twi_write(RTC_ADDR, send_array, 1);
    twi_read(RTC_ADDR, rtc_array, 8);
    
}

/****************************************************************/
//USART ISR
/****************************************************************/
ISR(USART1_RX_vect){
	//Store data
	uint8_t temp = UDR1;

	if(temp == OPEN_COM){
		set_buff_location(0);
	}

	//push, push, push, just keep pushing
	PushData(temp);
}

//testing
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



/****************************************************************/
//TIMER ISR
/****************************************************************/
ISR(TIMER1_COMPA_vect){
	LED_PORT ^= 1<<LED_PIN;

	time_array[16] ^= 0xff;
	time_array[17] ^= 0xff;


	time_array[T_SEC]++;
	if (time_array[T_SEC] > 59) {
		time_array[T_SEC] =0;
		time_array[T_MIN]++;
		if (time_array[T_MIN] > 59) {
			time_array[T_MIN] =0;
			time_array[T_HOUR]++;
			if(time_array[T_HOUR]>12){
				time_array[T_HOUR] = 1;
			}

		}
	}


	TCNT1=0;

}


/****************************************************************/
//check if data in buffer is in correct format
/****************************************************************/
uint8_t verify_buff(void){

	if (ReadData(0) == OPEN_COM ){
		if (ReadData(1) == COLOR_MODE || ReadData(1) == RAINBOW_MODE|| ReadData(1) == TIME_MODE || ReadData(1) ==TIME_MODE_2){
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


/****************************************************************/
//display to LED depending on mode
/****************************************************************/
void display_mode(uint8_t *data, uint8_t *disp_buff){
	uint8_t i =0;

	if (*(disp_buff+1)== COLOR_MODE){
		hs_convert(*(disp_buff+2), 0, data);
		_delay_ms(2);
		for(i=0; i<LED_NUM; i++){
			Send_SPI_array(data, (data+1), (data+2),1);
		}
	}
	else if (*(disp_buff+1) == ADDR_MODE){
		_delay_ms(1);
		for(i=0; i<LED_NUM; i++){
			hs_convert(*(disp_buff+2+i), 0, data);
			Send_SPI_array(data, (data+1), (data+2),1);
		}
	}
	else if (ReadData(1) == RAINBOW_MODE){
		_delay_ms(1);
		for(i=0; i<LED_NUM; i++){
			hs_convert((i+1)*7, 0, data);
			Send_SPI_array(data, (data+1), (data+2),1);
		}
	}
	else if (ReadData(1) == TIME_MODE) {

		if (flag == 0){
			time_array[6] = 11;
			time_array[7] = 19;
			time_array[T_SEC] = 0;
			flag = 200;
		}

		*data = 0xff;
		*(data+1) = 0;
		fill_time(time_array[6],time_array[7]);
		_delay_ms(1);
		for(i=9; i>=4; i--){
			if(time_array[2] & (1<<(i-2))){
				Send_SPI_array(data, (data+1), (data+1),1);
			}
			else{
				Send_SPI_array((data+1), (data+1), (data+1),1);
			}

		}
		Send_SPI_byte(0xFF);
		Send_SPI_byte(0xFF);
		Send_SPI_byte(0xFF);


		Send_SPI_byte(0x00);
		Send_SPI_byte(0x00);
		Send_SPI_byte(0x00);

		for(i=9; i>=6; i--){
			if(time_array[3] & (1<<(i-2))){
				Send_SPI_array((data+1), (data+0), (data+1),1);
			}
			else{
				Send_SPI_array((data+1), (data+1), (data+1),1);
			}
		}
		Send_SPI_byte(0xFF);
		Send_SPI_byte(0xFF);
		Send_SPI_byte(0xFF);


		Send_SPI_byte(0x00);
		Send_SPI_byte(0x00);
		Send_SPI_byte(0x00);


		Send_SPI_byte(time_array[16]);
		Send_SPI_byte(0x00);
		Send_SPI_byte(0x00);
		Send_SPI_byte(time_array[16]);
		Send_SPI_byte(0x00);
		Send_SPI_byte(0x00);

		for(i=9; i>=3; i--){
			if(time_array[4] & (1<<(i-3))){
				Send_SPI_array((data+1), (data+1), (data+0),1);
			}
			else{
				Send_SPI_array((data+1), (data+1), (data+1),1);
			}
		}

		Send_SPI_byte(0xFF);
		Send_SPI_byte(0xFF);
		Send_SPI_byte(0xFF);


		Send_SPI_byte(0x00);
		Send_SPI_byte(0x00);
		Send_SPI_byte(0x00);


		for(i=9; i>=5; i--){
			if(time_array[5] & (1<<(i-3))){
				Send_SPI_array((data+0), (data+0), (data+1),1);
			}
			else{
				Send_SPI_array((data+1), (data+1), (data+1),1);
			}
		}
		Send_SPI_byte(0xFF);
		Send_SPI_byte(0xFF);
		Send_SPI_byte(0xFF);


		Send_SPI_byte(0x00);
		Send_SPI_byte(0x00);
		Send_SPI_byte(0x00);

	}


}


/****************************************************************/
//main party
/****************************************************************/
int main(void){
	CPU_PRESCALE(CPU_16MHz);    //Set clock to 16 Mhz
	Setup_DDR();                //Set up DDR
	Setup_SPI();                //enable SPI
	Setup_USART();              //enable USART
	Setup_Timer();              //enable Timer
    Setup_twi();                //enable TWI

	uint8_t hs_rgb[3];          //used to store hue and saturation data
	uint8_t disp_buff[50];      //buffer that data gets copied to for displaying
	uint8_t i = 0;
	_delay_ms(200);

	LED_PORT |= (1<<LED_PIN);   //give me a light to show you are alive

    set_buff_location(0);       //reset data buffer to index 0
	PushData(OPEN_COM);         //send open command
	PushData(ADDR_MODE);        //set to addressable mode
	for(i =0; i<LED_NUM; i++){
		PushData((i+1) * 7);    //fill array with multi color
	}
	PushData(CLOSE_COM);        //close com


	//let the interrupts interrupt
	sei();

    time_array[6] = 12;
	time_array[7] = 11;
	
	
	while(1){

#if COLOR_TESTING
		//Check if data buffer recieved from xbee has a start, mode, and end byte
		if(verify_buff()){
			//if so pop a light for me
			LED_PORT ^= 1<<LED_PIN;
			//WIP: need to copy data from recieving buffer to a disaply buffer so data doesn't change
			//when recieving wireless data while in the middle of displaying data
			for (i=0; i<10; i++) {
				disp_buff[i] = ReadData(i);
			}

			//speaking of displaying data.. display date
			//give 3 element array to store rgb temp data
			display_mode(hs_rgb, disp_buff);

			


		}

#endif

#if RTC_TEST
		//twi_write(0xD0, rtc_array, 1);
		//twi_read(0xD0, (rtc_array+1), 1);
		//send_byte_usart(rtc_array[1]);
		//send_byte_usart(twi_array[2]);
        rtc_read();
		//send_byte_usart('\n');
        send_string_usart(rtc_array, 8);
		_delay_ms(200);

#endif

	}

}
