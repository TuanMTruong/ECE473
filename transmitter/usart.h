/******************************************************************/
// Author: Tuan Truong
// USART headers
//
/******************************************************************/

#define RX_PIN		PIN2_bm
#define TX_PIN		PIN3_bm

#define USART_Baudrate_Set(_usart, _bselValue, _bScaleFactor)            \
(_usart).BAUDCTRLA =(uint8_t)_bselValue;                             \
(_usart).BAUDCTRLB =(_bScaleFactor << USART_BSCALE0_bp)|(_bselValue >> 8)

void Setup_USARTC(void);
void usart_send_byte(uint8_t data);
void usart_send_string(uint8_t *data, uint8_t length);
