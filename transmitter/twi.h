/******************************************************************/
// Author: Tuan Truong
// TWI headers
//
/******************************************************************/

#define SDA_PIN		PIN0_bm
#define SCL_PIN		PIN1_bm


#define CPU_SPEED 32000000
#define BAUDRATE    400000 

#define TWI_BAUD(F_SYS, F_TWI) ((F_SYS / (2 * F_TWI)) - 5)
#define TWI_BAUDSETTING TWI_BAUD(CPU_SPEED, BAUDRATE)


void Setup_TWI(void);