/******************************************************************/
// Author: Tuan Truong
// TIMER functions
//
/******************************************************************/

//the includes
#include<avr/io.h>
#include"timer.h"

//Sets up the clock for 32MHz (very self explanatory)
void setClockTo32MHz() {
	CCP = CCP_IOREG_gc;              // disable register security for oscillator update
	OSC.CTRL = OSC_RC32MEN_bm;       // enable 32MHz oscillator
	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator to be ready
	CCP = CCP_IOREG_gc;              // disable register security for clock update
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc; // switch to 32MHz clock
}

//Set up the PWM on PE0 (timer TCE0) to control the gain of mic amplifier
void Setup_PWM(){
	//Set up TCE0 CLKSEL clk/64
	TCD0.CTRLA = TC_CLKSEL_DIV64_gc; //needs to be adjusted for RC value to create stable DC voltage
	//Enable output compare on channel 0A for timmer
	//Set up single slope PWM mode
	TCD0.CTRLB = TC0_CCDEN_bm | TC_WGMODE_SINGLESLOPE_gc;
	//enable interrupt
    TCD0.PER = 1024;
    TCD0.CCA = 500;
    TCD0.CCB = 500;
    TCD0.CNT = 0;
    
    
	return;
}

//Set up internal timer for keeping track of the time
void Setup_Timer(){
	
}

//Enable internal 32KHz clock for time counting
void Setup_32KHz(){
    
}