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
/******************************************************************/


#define F_CPU 320000000UL

//the includes
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

//macros
#define SS_PIN		PIN4_bm
#define MOSI_PIN 	PIN5_bm
#define MISO_PIN	PIN6_bm
#define SCK_PIN		PIN7_bm

#define SHIFT_LOAD_PIN	PIN2_bm
#define SHIFT_LATCH_PIN PIN3_bm

#define FQD_RST_PIN	PIN0_bm
#define FQD_AUDIO_PIN	PIN2_bm

#define SDA_PIN		PIN0_bm
#define SCL_PIN		PIN1_bm

#define LCD_SS_PIN	PIN0_bm
#define LCD_RST_PIN	PIN1_bm
#define LCD_SIG_PIN	PIN2_bm
#define LCD_LIGHT_PIN	PIN3_bm

#define RX_PIN		PIN2_bm
#define TX_PIN		PIN3_bm

#define AUD_IN_PIN	PIN1_bm
#define AUD_VOL_PIN	PIN0_bm

#define TEMP_PIN	PIN0_bm



#define USART_Baudrate_Set(_usart, _bselValue, _bScaleFactor)            \
	(_usart).BAUDCTRLA =(uint8_t)_bselValue;                             \
	(_usart).BAUDCTRLB =(_bScaleFactor << USART_BSCALE0_bp)|(_bselValue >> 8)

//global variables


//Sets up the clock for 32MHz (very self explanatory)
void setClockTo32MHz() {
	CCP = CCP_IOREG_gc;              // disable register security for oscillator update
	OSC.CTRL = OSC_RC32MEN_bm;       // enable 32MHz oscillator
	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator to be ready
	CCP = CCP_IOREG_gc;              // disable register security for clock update
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc; // switch to 32MHz clock
}

//set up Data Direction Register non-serial protocols
void Setup_DDR(){
	//1 = output, 0 = input
	PORTE.DIRSET = SHIFT_LATCH_PIN | SHIFT_LOAD_PIN | AUD_VOL_PIN;
	PORTB.DIRSET = FQD_RST_PIN | FQD_AUDIO_PIN;
	PORTD.DIRSET = LCD_SS_PIN | LCD_RST_PIN | LCD_SIG_PIN | LCD_LIGHT_PIN;
	
	//set audio and temp sensor to input
	PORTA.DIRCLR = AUD_IN_PIN | TEMP_PIN;

	return;
}

//sets up the SPI on port C
void Setup_SPIC(){
	//Setup DDR for SPIC (1 = output, 0 = input)
	PORTC.DIRSET = SS_PIN | SCK_PIN | MOSI_PIN;
	PORTC.DIRCLR = MISO_PIN;

	//Set up for 2x speed, enabled, master, at 64 prescale
	SPIC.CTRL = SPI_CLK2X_bm | SPI_ENABLE_bm | SPI_MASTER_bm |SPI_PRESCALER_DIV64_gc;
	
	return;
}

//sets up the SPI on port D
void Setup_SPID(){
	//Setup DDR for SPIC (1 = output, 0 = input)
	PORTD.DIRSET = SS_PIN | SCK_PIN | MOSI_PIN;
	PORTD.DIRCLR = MISO_PIN;

	//Set up for 2x speed, enabled, master, at 64 prescale
	SPID.CTRL = SPI_CLK2X_bm | SPI_ENABLE_bm | SPI_MASTER_bm |SPI_PRESCALER_DIV64_gc;
	
	return;

}

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
	USART_Baudrate_Set(USARTC0, 1079, -5);
	return;

}

void Setup_TWIC(){

}

//Set up the PWM on PE0 (timer TCE0) to control the gain of mic amplifier
void Setup_PWM(){
	//Set up TCE0 CLKSEL clk/64
	TCE0.CTRLA = TC_CLKSEL_DIV64_gc; //needs to be adjusted for RC value to create stable DC voltage
	//Enable output compare on channel 0A for timmer
	//Set up single slope PWM mode
	TCE0.CTRLB = TC0_CCAEN_bm | TC_WGMODE_SINGLESLOPE_gc;
	//enable interrupt 


	return;
}


//Set up internal timer for keeping track of the time
void Setup_Timer(){
	
}

//Enable internal 32KHz clock for time counting
void Setup_32KHz(){

}




int main(){
	//set 32MHz clock
	setClockTo32MHz();
	//set up DDR
	Setup_DDR();
	//set up SPIC
	Setup_SPIC();
	//set up SPID
	Setup_SPID();
	//set up USARTC
	Setup_USARTC();
	//set up TWIC

	//set up PWM
	Setup_PWM();
	//set up ADC

	while(1){


	}

}
