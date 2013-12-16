/******************************************************************/
// Author: Tuan Truong
// SPI functions
//
/******************************************************************/


//the includes
#include<avr/io.h>
#include"spi.h"
#include<util/delay.h>


/******************************************************************/
// Set up SPIC and SPID
/******************************************************************/
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
	SPID.CTRL =  SPI_ENABLE_bm | SPI_MASTER_bm |SPI_PRESCALER_DIV128_gc;
	
	return;
    
}

/******************************************************************/
// Send byte to LCD
/******************************************************************/
void LCD_send_byte(uint8_t data){
	PORTD.OUTCLR = LCD_SS_PIN;          //clear slave select
	SPID.DATA = data;                   //send data
	while(!(SPID.STATUS & SPI_IF_bm)){};//wait till data send out
	PORTD.OUTSET = LCD_SS_PIN;          //set slave select
}


/******************************************************************/
// Initialize LCD
/******************************************************************/
void Setup_LCD(){
	//Setup DDR
	PORTD.DIRSET = LCD_SS_PIN | LCD_RST_PIN | LCD_SIG_PIN | LCD_LIGHT_PIN | LCD_WR_PIN;
    PORTA.DIRSET = LCD_E_PIN;
    
    PORTD.OUTSET = LCD_LIGHT_PIN;   //turn on back light
    PORTD.OUTSET = LCD_WR_PIN;          //set to write mode
    PORTA.OUTSET = LCD_E_PIN;       //enable operation
    
    PORTD.OUTSET = LCD_RST_PIN;     //set reset
	PORTD.OUTCLR = LCD_SIG_PIN;     //A0 low
	_delay_ms(20);

	LCD_send_byte(0xA2);    // 1/9 bias
	_delay_us(1);
	LCD_send_byte(0xA0);    //ADC Select (revers)
	_delay_us(1);
	LCD_send_byte(0xC8);    //COM output leve to reverse
	_delay_us(1);
	LCD_send_byte(0xA4);    //Display all points normal
	_delay_us(1);
	LCD_send_byte(0x40);    //Display Start Line set
	_delay_us(1);
	LCD_send_byte(0x25);    //Internal resistor ration
	_delay_us(1);
	LCD_send_byte(0x81);    //Electronic volume mode set
	_delay_us(1);
	LCD_send_byte(0x10);    //electronic volume (contrast)
	_delay_us(1);
	LCD_send_byte(0x2F);    //pwr ctrl set
	_delay_us(1);
	LCD_send_byte(0xAF);    //Display one
	_delay_us(1);           //wait
    _delay_us(1);

}

/******************************************************************/
// write to lcd (still in progress)
/******************************************************************/
void LCD_update(){
    uint8_t i, j;                   //counters
    //PORTD.OUTCLR = LCD_WR_PIN;          //set to writing mode
    for (i=0; i<8; i++) {
        PORTD.OUTCLR = LCD_SIG_PIN; //clear A0
        LCD_send_byte(0xB0 | i);    //select page address
        _delay_us(1);
        LCD_send_byte(0x10);        //Select MS column addr
        _delay_us(1);
        LCD_send_byte(0x00);        //select LS column addr
        _delay_us(1);
        PORTD.OUTSET = LCD_SIG_PIN; //set A0
        for (j=0; j<128; j++) {
            LCD_send_byte(0xff);    //write to RAM
            _delay_us(1);
        }
    }
    
}


/******************************************************************/
// Read SPIC for buttons and Encoders
// return byte 4 bits encoder, 4 bits buttons
/******************************************************************/
uint8_t Read_Buttons(){
	uint8_t temp;

	//Pull SHIFT_LOAD low for >= 120 ns to load in button data
	PORTE.OUTCLR = SHIFT_LOAD_PIN;
	//_delay_us(1);
	PORTE.OUTSET = SHIFT_LOAD_PIN;

	//Pull SHIFT_LATCH low to send out serial data.
	PORTE.OUTCLR = SHIFT_LATCH_PIN;

	//send dummy spi 
	SPIC.DATA = 0x00;
	while(!(SPIC.STATUS & SPI_IF_bm)){};
	temp = SPIC.DATA;


	//After data read and saved set SHIFT_LOAD and SHIFT_LATCH
	PORTE.OUTSET = SHIFT_LATCH_PIN | SHIFT_LOAD_PIN;
	return(temp);
}


