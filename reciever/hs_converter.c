/*
 * hs_converter.c
 *
 * Created: 12/2/2013 2:04:21 PM
 *  Author: Tuan
 */

#include<avr/io.h>
#include"hs_converter.h"



void hs_convert(uint8_t color, uint8_t scale, uint8_t *hs_data){
    uint8_t *red = hs_data;
    uint8_t *green = hs_data+1;
    uint8_t *blue = hs_data+2;
    
    *red = 0;
    *green = 0;
    *blue = 0;
    
    //7 state
    
    if(color >= 0 && color <36){
        *red = (MAX_COLOR/36) * color;
    }
    if (color >= 36 && color<72) {
        *red = MAX_COLOR;
        *green = (MAX_COLOR/36) * (color-36);
    }
    if (color >= 72 && color<108) {
        *green = MAX_COLOR;
        *red = 255-((MAX_COLOR/36) * (color-72));
    }
    if (color >= 108 && color<144) {
        *green = MAX_COLOR;
        *blue = (MAX_COLOR/36) * (color-108);
    }
    
    if (color >= 144 && color<180) {
        *blue = MAX_COLOR;
        *green = 255-((MAX_COLOR/36) * (color-144));
    }
    if (color >= 180 && color<216) {
        *blue = MAX_COLOR;
        *red = (MAX_COLOR/36) * (color-180);
    }
    if (color >= 216 && color<252) {
        *red = MAX_COLOR;
        *blue = 255-((MAX_COLOR/36) * (color-216));
    }
    if (color>=252) {
        *red = MAX_COLOR;
        
    }
    return;
}