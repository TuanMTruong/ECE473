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

/******************************************************************/
//macros
/******************************************************************/
#define AUD_IN_PIN	PIN1_bm
#define AUD_VOL_PIN	PIN0_bm
#define TEMP_PIN	PIN0_bm

#define OPEN_COM    0xFF    //starting communication byte from USART
#define CLOSE_COM   0xBA    //Closing communication byte
#define MODE        1       //Mode index
#define OPEN        0       //open com index

#define ADDR_MODE       0xCB    //operate in ADDR, all leds are addressable
#define COLOR_MODE      0x0C    //operate in full single color
#define RAINBOW_MODE    0xAC    //rainbow mode (show rainbow spectrum)
#define TIME_MODE       0xEE    //Show BCD clock on strip
#define TIME_MODE_2     0xAA    //unused
#define RTC_REQUEST     0xAD    //send request for RTC time and date data

#define RED_BUTTON      5   //Red button bit index
#define GREEN_BUTTON    7   //Green buton bit index
#define BLUE_BUTTON     4   //Blue button bit index
#define YELLOW_BUTTON   6   //Yellow button bit index

#define LED_NUM         32  //maximum LED for LED strip

#define SPECTRUM_TEST   0   //used in earlier dev to show hue and saturation spectrum
#define RTC_TEST        0   //used to test RTC data transfer
#define LCD_TEST        0   //test LCD write functions
#define BUTTONS_TEST    1   //interactive buttons (full operational mode)


//global variables
uint8_t SendData_buffer[50];

//encoder = [prev_E1, curr_E1, state_E1, prev_E2, curr_E2, state_E2]
uint8_t Encoders[10];       //array to store previous and current encoder data
#define prev_E1     0       //index of previous encoder 1 data
#define curr_E1     1       //index of current encoder 1 data
#define state_E1    2       //encoder stat CW vs CCW
#define prev_E2     3       //index of previous encoder 2 data
#define curr_E2     4       //index of current encoder 1 data
#define state_E2    5       //encoder stat CW vs CCW
#define CW          0x0F    //byte for Clockwise
#define CCW         0xF0    //byte for Counter clockwise

//save data from button press
//buttons = [prev_BN, curr_BN]
uint8_t buttons[2];         //array to hold previous and current button data
#define prev_BN     0       //index of previous button data
#define curr_BN     1       //index of current button data

/******************************************************************/
// Set up DDR for non serial pins
/******************************************************************/
void Setup_DDR(void){
	//1 = output, 0 = input
	PORTE.DIRSET = SHIFT_LATCH_PIN | SHIFT_LOAD_PIN | AUD_VOL_PIN;
	PORTB.DIRSET = FQD_RST_PIN | FQD_AUDIO_PIN;
	PORTD.DIRSET = LCD_SS_PIN | LCD_RST_PIN | LCD_SIG_PIN | LCD_LIGHT_PIN;

	//set audio and temp sensor to input
	PORTA.DIRCLR = AUD_IN_PIN | TEMP_PIN;
	return;
}


/******************************************************************/
//cheange operational mode
/******************************************************************/
uint8_t set_mode(uint8_t mode){
	SendData_buffer[MODE] = mode;       //set new mode
	SendData_buffer[OPEN] = OPEN_COM;   //also set Open communication byte
	if (mode != ADDR_MODE){             //Is not addressable mode set Close byte
		SendData_buffer[3] = CLOSE_COM;
	}
	return(SendData_buffer[MODE]);      //return mode to confirm
}

/******************************************************************/
//check mode, return current mode
/******************************************************************/
uint8_t check_mode(void){
	return(SendData_buffer[MODE]);      //return current mode
}




/******************************************************************/
//check buttons
//take in input from readbutton function and run operation
//depending on buttons
/******************************************************************/
uint8_t check_buttons(uint8_t inputs){
	buttons[curr_BN] = inputs & 0xF0;                   //mask out encoder data
	if(buttons[curr_BN]){                               //check if button pressed
		if(buttons[curr_BN] & (1<<RED_BUTTON)){         //is it the red button
			set_mode(COLOR_MODE);                   //set full single color
			SendData_buffer[2] = 30;                //color = red
		}
		else if(buttons[curr_BN] & (1<<GREEN_BUTTON)){  //is it the green button
			set_mode(COLOR_MODE);                   //set full single color
			SendData_buffer[2] = 108;               //color = green
		}
		else if(buttons[curr_BN] & (1<<BLUE_BUTTON)){   //is it the blue button
			set_mode(COLOR_MODE);                   //set full single color
			SendData_buffer[2] = 180;               //color = blue
		}
		else if(buttons[curr_BN] & (1<<YELLOW_BUTTON)){ //is it the yellow button
			if (check_mode() == RAINBOW_MODE){      //if rainbow mode
				set_mode(TIME_MODE);            //then go to time mode
			}
			else{
				set_mode(RAINBOW_MODE);         //else sgo to rainbow mode
			}
			SendData_buffer[2] = 10;		//dummy color data
		}

		usart_send_string(SendData_buffer, 4);          //send out button actions
		buttons[prev_BN] =buttons[curr_BN];             //save current data to previous
		_delay_ms(1);                                   //might as well wait
		while (!(Read_Buttons()&0xF0) == (0x00)) {}     //wait till button is lifted off
		_delay_ms(1);                                   //no rush
	}

	return(SendData_buffer[MODE]);                      //return mode for comfirmation
}

/******************************************************************/
//check encoder
// encoder turn = single color scale shift in hue and saturation spectrum
/******************************************************************/
uint8_t check_encoders(uint8_t inputs){
	Encoders[curr_E1] = inputs & 0x03;  //mask out buttons data

	//have we got new data?
	if (Encoders[curr_E1] != Encoders[prev_E1]){
		//xor data to replicate grey bti counting 
		if((Encoders[curr_E1] == (Encoders[prev_E1] ^ 0x01)) && (Encoders[curr_E1] == 0x01 || Encoders[curr_E1] == 0x02)){
			Encoders[state_E1] = CW;    //set to clockwise

			//usart_send_byte('R'); //send data for debug
			SendData_buffer[2]+=2;  //increment hue and saturation scale
		}
		//xor data to replicate grey bti counting
		else if((Encoders[curr_E1] == (Encoders[prev_E1] ^ 0x01)) && (Encoders[curr_E1] == 0x00 || Encoders[curr_E1] == 0x03)){
			Encoders[state_E1] = CCW;   //set stake to counter clockwise

			//usart_send_byte('L'); //send debug data
			SendData_buffer[2]-=2;  //increment hue and saturation scale
		}
		else {
			Encoders[state_E1] = 0x00;  //if not CW or CCW state is NULL
		}


	}
	Encoders[prev_E1] = Encoders[curr_E1];  //save current data to previous data

	//check encoder 2 (on the right)
	Encoders[curr_E2] = (inputs>>2) & 0x03;

	//have we got new data?
	if (Encoders[curr_E2] != Encoders[prev_E2]){
		//xor data to replicate grey bti counting
		if((Encoders[curr_E2] == (Encoders[prev_E2] ^ 0x01)) && (Encoders[curr_E2] == 0x01 || Encoders[curr_E2] == 0x02)){
			Encoders[state_E2] = CW;

			//usart_send_byte('R');     //send data for debug
			SendData_buffer[2] += 2;    //increment hue and saturation scale
		}
		//xor data to replicate grey bti counting
		else if((Encoders[curr_E2] == (Encoders[prev_E2] ^ 0x01)) && (Encoders[curr_E2] == 0x00 || Encoders[curr_E2] == 0x03)){
			Encoders[state_E2] = CCW;

			//usart_send_byte('L'); //send data for debug
			SendData_buffer[2]-=2;  //increment hue and saturation scale
		}
		else {
			Encoders[state_E2] = 0x00;  //if not CW or CCW state is NULL
		}


	}
	Encoders[prev_E2] = Encoders[curr_E2];  //save current data to previous data


	return(1);
}


/******************************************************************/
//Lets get this main party started!
/******************************************************************/
int main(void){
	setClockTo32MHz();	//set 32MHz clock
	_delay_ms(200);     //no rush

	Setup_DDR();        //DDR
	Setup_SPIC();       //SPIC
	Setup_SPID();       //SPID
	Setup_USARTC();     //USART
	Setup_LCD();        //WIP
	//Setup_PWM();      //WIP
	//Setup_TWI();      //WIP
	//Setup_ADC();      //WIP

	//i am in no hurry...
	_delay_ms(200);

	//some variables
	uint8_t i =0;
	uint8_t temp;
	uint8_t array[50];



	/**
	//send a rainbow to start, i just wanna see if it works...
	usart_send_byte(OPEN_COM);
	usart_send_byte(ADDR_MODE);
	for(i =0; i<LED_NUM; i++){
	usart_send_byte((i+1) * 6);
	}
	usart_send_byte(CLOSE_COM);

	_delay_ms(200);
	 **/

	usart_send_byte(OPEN_COM);      //Open communication
	usart_send_byte(RTC_REQUEST);   //request current time & date
	usart_send_byte(0x00);          //dummy data
	usart_send_byte(CLOSE_COM);     //close communication

	//WIP: until LCD is done there is no way to show RTC data
	// for (i=0; i<7; i++) {
	//while(!(USARTC0.STATUS & USART_RXCIF_bm)){}
	//temp = USARTC0.DATA;
	//}


	PORTD.OUTSET = LCD_LIGHT_PIN;   //lCD back light on
	LCD_update();                   //write to LCD (WIP)

	while(1){
#if SPECTRUM_TEST
		array[1] = COLOR_MODE;
		array[2] = temp;
		usart_send_string(array, 4);
		_delay_ms(50);
		temp++;
#endif


#if LCD_TEST
		LCD_update();
		_delay_ms(200);
		//while (1) {}


#endif



#if BUTTONS_TEST

		temp = Read_Buttons();  //read in input data
		check_buttons(temp);    //check buttons with input data
		check_encoders(temp);   //check encoder data

		//WIP (need to create a flag to indicate new data to send out
		if(SendData_buffer[2] != SendData_buffer[4]){   	//currently using index 4 (not a good idea)
			usart_send_string(SendData_buffer, 4);		//send buffer
			SendData_buffer[4] = SendData_buffer[2];	//save previous 
		}

#endif



	}

}
