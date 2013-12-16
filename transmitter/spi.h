/******************************************************************/
// Author: Tuan Truong
// SPI headers
//
/******************************************************************/

//macros
#define SS_PIN		PIN4_bm
#define MOSI_PIN 	PIN5_bm
#define MISO_PIN	PIN6_bm
#define SCK_PIN		PIN7_bm

#define SHIFT_LOAD_PIN	PIN2_bm
#define SHIFT_LATCH_PIN PIN3_bm

#define FQD_RST_PIN	PIN0_bm
#define FQD_AUDIO_PIN	PIN2_bm

#define LCD_SS_PIN	PIN0_bm
#define LCD_RST_PIN	PIN1_bm
#define LCD_SIG_PIN	PIN2_bm
#define LCD_LIGHT_PIN	PIN3_bm



void Setup_SPIC(void);
void Setup_SPID(void);

void Setup_SPIC(void);
void Setup_SPID(void);
uint8_t Read_Buttons(void);
