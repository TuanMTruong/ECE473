/******************************************************************/
// Author: Tuan Truong
// TWI functions
//
/******************************************************************/

//the includes
#include<avr/io.h>
#include"twi.h"

void Setup_TWI(void){
    PORTD.DIRSET |= PIN4_bm;
    PORTD.OUT |= PIN4_bm;
    TWIC.CTRL = TWI_SDAHOLD_50NS_gc;
    TWIC.MASTER.CTRLA = TWI_MASTER_ENABLE_bm;
    TWIC.MASTER.CTRLB = TWI_MASTER_SMEN_bm;
    TWIC.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
    
   // TWIC.CTRLC =
    
}

/**
uint8_t twi_read(){
    TWIC.MASTER.ADDR = RTC_ADDR;
    while(!(TWIC.MASTER.STATUS&TWI_MASTER_WIF_bm));
    
    
}

**/