/*
 * usart.h
 *
 * Created: 12/2/2013 2:04:12 PM
 *  Author: Tuan
 */ 


#ifndef USART_H_
#define USART_H_


#define RX_PIN	2
#define TX_PIN	3

#define buffersize 50

void Setup_USART();

void PushData(uint8_t data);
uint8_t PopData();
uint8_t ReadData(uint8_t location);
uint8_t get_buff_location();
void set_buff_location(uint8_t location);


#endif /* USART_H_ */
