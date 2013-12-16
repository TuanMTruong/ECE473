/*
 * twi.h
 *
 * Created: 12/2/2013 2:04:12 PM
 *  Author: Tuan
 */

#define SCL_PIN 0
#define SDA_PIN 1

#define TWI_TWBR 0x0C //set for 400KHz
#define RTC_ADDR 0xD0
#define RTC_ADDR_SEC 	0x00
#define RTC_ADDR_MIN 	0x01
#define RTC_ADDR_HOUR 	0x02
#define RTC_ADDR_DAY	0x03
#define RTC_ADDR_DATE   0x04
#define RTC_ADDR_MONTH  0x05
#define RTC_ADDR_YEAR   0x06
#define RTC_ADDR_CTRL   0x07


#define TWCR_START  0xA5 //send START
#define TWCR_SEND   0x85 //poke TWINT flag to send another byte
#define TWCR_RACK   0xC5 //receive byte and return ACK to slave
#define TWCR_RNACK  0x85 //receive byte and return NACK to slave
#define TWCR_RST    0x04 //reset TWI
#define TWCR_STOP   0x94 //send STOP,interrupt off, signals completion


void Setup_twi(void);
uint8_t twi_busy( void );
void twi_write(uint8_t addr, uint8_t *buffer, uint8_t length);
void twi_read(uint8_t addr, uint8_t *buffer, uint8_t length);