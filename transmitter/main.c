/******************************************************************/
// Author: Tuan Truong
// Date created: 10/29/13
// This is the source code for the transmitter for the ECE473 project
// The transmitter is supposed to:
// - write the time to an LCD using I2C
// - Send wireless data via USART through XBEE
// - Use SPI to read in 2 encoders & 4 push buttons
// - Sense temperature and display on LCD
// - Use PWM to control gain of mic amplifier
// - send audio data via DAC to frequency divider and read output via SPI

// Hardware description
// - Push buttons & Encoder on SPIC
//	- SPIC_SS 	PC4
//	- SPIC_MOSI 	PC5
//	- SPIC_MISO	PC6
//	- SPIC_SCK	PC7
//	- SHIFT_LOAD	PE2
//	- SHIFT_LATCH	PE3
// - Frequency divider on SPID
//	- SPID_SS	PD4
//	- SPID_MOSI	PD5
//	- SPID_MISO	PD6
//	- SPID_SCK	PD7
//	- FQD_RST	PB0
//	- FQD_AUDIO	PB2
// - LCD on TWIC
//	- TWIC_SDA	PC0
//	- TWIC_SCL	PC1
//	- LCD_SS	PD0
//	- LCD_RST	PD1
//	- LCD_SIG	PD2
//	- LCD_LIGHT	PD3
// - XBEE on USARTC0
//	- USART_RXCO	PC2
//	- USART_TXC0	PC3
// - Audio in
// 	- AUD_IN 	PA1
//	- AUD_VOL	PE0
// - Temp Sensor
//	- TEMP		PA0
//
//
//
// Plan of Attack:
// pretty much just wing it...
/******************************************************************/


//defining clock speed
#define F_CPU 32000000UL

//the includes
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include"spi.h"
#include"usart.h"
#include"timer.h"
#include"twi.h"


//macros
#define AUD_IN_PIN	PIN1_bm
#define AUD_VOL_PIN	PIN0_bm
#define TEMP_PIN	PIN0_bm

//used to start and end communication packet
#define OPEN_COM 0xFF
#define CLOSE_COM 0xBA
#define MODE 1
#define OPEN 0

//different color mode
#define ADDR_MODE 0xCB
#define COLOR_MODE 0x0C
#define RAINBOW_MODE 0xAC
#define TIME_MODE 0xEE
#define TIME_MODE_2 0xAA

//physical buttons pin (the color caps are removable so this is could be wrong)
#define RED_BUTTON 5
#define GREEN_BUTTON 7
#define BLUE_BUTTON 4
#define YELLOW_BUTTON 6

//maximum LED for LED strip
#define LED_NUM 32


//debug & test modes
#define SPECTRUM_TEST 0
#define RTC_TEST 1
#define BUTTONS_TEST 0


//global variables
uint8_t SendData_buffer[50];

//encoder = [prev_E1, curr_E1, state_E1, prev_E2, curr_E2, state_E2]
uint8_t Encoders[10];
#define prev_E1 0
#define curr_E1 1
#define state_E1 2
#define prev_E2 3
#define curr_E2 4
#define state_E2 5
#define CW 0x0F
#define CCW 0xF0

//save data from button press
//buttons = [prev_BN, curr_BN]
uint8_t buttons[2];
#define prev_BN 0
#define curr_BN 1

//set up Data Direction Register non-serial protocols
void Setup_DDR(void){
	//1 = output, 0 = input
	PORTE.DIRSET = SHIFT_LATCH_PIN | SHIFT_LOAD_PIN | AUD_VOL_PIN;
	PORTB.DIRSET = FQD_RST_PIN | FQD_AUDIO_PIN;
	PORTD.DIRSET = LCD_SS_PIN | LCD_RST_PIN | LCD_SIG_PIN | LCD_LIGHT_PIN;

	//set audio and temp sensor to input
	PORTA.DIRCLR = AUD_IN_PIN | TEMP_PIN;

	return;
}



uint8_t set_mode(uint8_t mode){
	SendData_buffer[MODE] = mode;
    SendData_buffer[OPEN] = OPEN_COM;
    if (mode != ADDR_MODE){
        SendData_buffer[3] = CLOSE_COM;
    }
    
	return(SendData_buffer[MODE]);
}


uint8_t check_mode(void){
	return(SendData_buffer[MODE]);
}





uint8_t check_buttons(uint8_t inputs){
	buttons[curr_BN] = inputs & 0xF0;
	if(buttons[curr_BN]){
	
		if(buttons[curr_BN] & (1<<RED_BUTTON)){
			set_mode(COLOR_MODE);
			SendData_buffer[2] = 30;
		}
		else if(buttons[curr_BN] & (1<<GREEN_BUTTON)){
			set_mode(COLOR_MODE);
			SendData_buffer[2] = 108; 
		}
		else if(buttons[curr_BN] & (1<<BLUE_BUTTON)){
			set_mode(COLOR_MODE);
			SendData_buffer[2] = 180;
		}
		else if(buttons[curr_BN] & (1<<YELLOW_BUTTON)){
            if (check_mode() == RAINBOW_MODE){
                set_mode(TIME_MODE);
            }
  
            else{
			set_mode(RAINBOW_MODE);
            }
            SendData_buffer[2] = 10;
       
		}

        usart_send_string(SendData_buffer, 4);
        buttons[prev_BN] =buttons[curr_BN];
        _delay_ms(1);
        while (!(Read_Buttons()&0xF0) == (0x00)) {}
        _delay_ms(1);
        
	}
	
	return(SendData_buffer[MODE]);
}

uint8_t check_encoders(uint8_t inputs){
	//check encoder 1 (on the left)
	//save encoder data and clear all the other crap
	Encoders[curr_E1] = inputs & 0x03;
	
	//have we got new data?
	if (Encoders[curr_E1] != Encoders[prev_E1]){
		if((Encoders[curr_E1] == (Encoders[prev_E1] ^ 0x01)) && (Encoders[curr_E1] == 0x01 || Encoders[curr_E1] == 0x02)){
			Encoders[state_E1] = CW;

			//usart_send_byte('R');
			SendData_buffer[2]+=2;
		}
		else if((Encoders[curr_E1] == (Encoders[prev_E1] ^ 0x01)) && (Encoders[curr_E1] == 0x00 || Encoders[curr_E1] == 0x03)){
			Encoders[state_E1] = CCW;

			//usart_send_byte('L');
			SendData_buffer[2]-=2;
		}
		else {
			Encoders[state_E1] = 0x00;
		}

	
	}
	Encoders[prev_E1] = Encoders[curr_E1];

	//check encoder 2 (on the right)
	Encoders[curr_E2] = (inputs>>2) & 0x03;
    
    //have we got new data?
	if (Encoders[curr_E2] != Encoders[prev_E2]){
		if((Encoders[curr_E2] == (Encoders[prev_E2] ^ 0x01)) && (Encoders[curr_E2] == 0x01 || Encoders[curr_E2] == 0x02)){
			Encoders[state_E2] = CW;
            
			//usart_send_byte('R');
			SendData_buffer[2] += 2;
		}
		else if((Encoders[curr_E2] == (Encoders[prev_E2] ^ 0x01)) && (Encoders[curr_E2] == 0x00 || Encoders[curr_E2] == 0x03)){
			Encoders[state_E2] = CCW;
            
			//usart_send_byte('L');
			SendData_buffer[2]-=2;
		}
		else {
			Encoders[state_E2] = 0x00;
		}
        
        
	}
	Encoders[prev_E2] = Encoders[curr_E2];

	
	//_delay_ms(1);
	return(1);
}




int main(void){
	//set 32MHz clock
	setClockTo32MHz();

	_delay_ms(200);

	//set up
	Setup_DDR();
	Setup_SPIC();
	Setup_SPID();
	Setup_USARTC();

	//Setup_PWM();
	//Setup_TWI();
	//Setup_ADC();

	//i am in no hurry...
	_delay_ms(200);

	//some variables
	uint8_t i =0;
	uint8_t temp;
	uint8_t array[50];

	array[0] = OPEN_COM;
	array[1] = 0;
	array[2] = 0x00;
	array[3] = CLOSE_COM;

	
	//send a rainbow to start, i just wanna see if it works...
	usart_send_byte(OPEN_COM);
	usart_send_byte(ADDR_MODE);
	for(i =0; i<LED_NUM; i++){
		usart_send_byte((i+1) * 6);
	}
	usart_send_byte(CLOSE_COM);
	
    PORTC.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc;
    PORTC.PIN1CTRL = PORT_OPC_WIREDANDPULL_gc;

	
	while(1){
#if SPECTRUM_TEST
		array[1] = COLOR_MODE;
		array[2] = temp;
		usart_send_string(array, 4);
		_delay_ms(50);
		temp++;
#endif
        

#if RTC_TEST
        
        
        
#endif
        
        

#if BUTTONS_TEST

		//_delay_ms(50);
		temp = Read_Buttons();
		check_buttons(temp);
		check_encoders(temp);
		//check_buttons();
		
        
		if(SendData_buffer[2] != SendData_buffer[4]){
			usart_send_string(SendData_buffer, 4);
			SendData_buffer[4] = SendData_buffer[2];
		}
         
#endif



	}

}
