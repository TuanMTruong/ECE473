/*
 * twi.c
 *
 * Created: 12/2/2013 2:04:21 PM
 *  Author: Tuan
 */
//includes
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/twi.h>
#include<stdlib.h>
#include "twi.h"

/****************************************************************/
//twi data buffers
/****************************************************************/
volatile uint8_t *twi_buffer;   //pointer to buffer
volatile uint8_t twi_addr;      //addr of slave
volatile uint8_t twi_pkg_size;  //how many bytes do you want?

/****************************************************************/
//twi interrupt that checks the twi status register for instructions
//on next task.
/****************************************************************/
ISR(TWI_vect){
    static uint8_t twi_index;   //keeps buffer location

    //switching between twi states
    //looks at the status regiter to figure out the state
    
    switch (TWSR){
            
    //START States
    case TW_START:              //START has been sent so go to next case
    case TW_REP_START:          //send a start condition
            TWDR = twi_addr;    //load address into data buffer to be writen
            twi_index = 0;      //reset index
            TWCR = TWCR_SEND;   //send byte
            break;
            
    //Master transmitte ACK and NACKS
    case TW_MT_SLA_ACK:         //SLA+W was xmitted and ACK rcvd, fall through
    case TW_MT_DATA_ACK:
            if(twi_index< twi_pkg_size){
                TWDR = twi_buffer[twi_index++];
                TWCR = TWCR_SEND;
            }
            else {
                TWCR = TWCR_STOP;
            }
            break;
    
    //Master recive ACKs and NACKs
    case TW_MR_DATA_ACK:
            twi_buffer[twi_index++] = TWDR;     //read in data to buffer
    case TW_MR_SLA_ACK:
            if(twi_index < (twi_pkg_size-1)){
                TWCR = TWCR_RACK;               //return ack to slave
            }
            else {
                TWCR = TWCR_RNACK;              //or return NACK =(
            }
            break;
    case TW_MR_DATA_NACK:
            twi_buffer[twi_index] = TWDR;
            TWCR = TWCR_STOP;
            break;
    case TW_MT_ARB_LOST:    //Arbitration lost
        TWCR = TWCR_START;  //initiate RESTART
        break;
    default:
        TWCR = TWCR_RST;
    }
    
    
    
}

/****************************************************************/
//sets up twi
/****************************************************************/
void Setup_twi(void){
    
    TWDR = 0xFF;    //clear all data on twi data register
    TWSR = 0x00;    //clear status and prescale = 1
    TWBR = TWI_TWBR; //set to 400KHz
    
}

/****************************************************************/
//chect twi is in interrupt, sending data, or someone else has the bus
/****************************************************************/
uint8_t twi_busy( void ){
    return ( TWCR & (1<<TWIE) );    // IF TWI Interrupt is enabled then the Transceiver is busy
}


/****************************************************************/
//copy data to buffer to begin writing process, interrupt handles
//acks and nacks for you.
/****************************************************************/
void twi_write(uint8_t addr, uint8_t *buffer, uint8_t length){
    
    while (twi_busy()) {}   //wait for twi if busy
    twi_addr = addr & ~TW_READ; //set up address of slave and write mode
    twi_buffer = buffer;    //point twi buffer pointer to data buffer
    twi_pkg_size = length;  //save length of data package
    TWCR = TWCR_START;      //Enable twi interrupt and let ISR handle it
    
    return;
    
}

/****************************************************************/
//brign reading process, interrupt handles
//acks and nacks for you.
/****************************************************************/
void twi_read(uint8_t addr, uint8_t *buffer, uint8_t length){
    while (twi_busy()) {}   //wait for twi if busy
    twi_addr = addr | TW_READ; //set up address of slave and read mode
    twi_buffer = buffer;    //point twi buffer pointer to data buffer
    twi_pkg_size = length;  //save length of data package
    TWCR = TWCR_START;      //Enable twi interrupt and let ISR handle it
    
    return;

}

