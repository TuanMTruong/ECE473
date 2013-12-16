/*
 * hs_converter.c
 *
 * Created: 12/2/2013 2:04:21 PM
 *  Author: Tuan
 */

#include<avr/io.h>
#include"hs_converter.h"

/****************************************************************/
//takes an 8bit vaule from 0-255 and converts it into a rainbow spectrum
//must pass in an 3 element array to store RGB data in
//WIP: fix scale factor to control saturation 
/****************************************************************/
void hs_convert(uint8_t color, uint8_t scale, uint8_t *hs_data){
    uint8_t *red = hs_data;     //create red pointer
    uint8_t *green = hs_data+1; //green pointer
    uint8_t *blue = hs_data+2;  //blue pointer
    
    *red = 0;   //clear pointer data
    *green = 0; //clear pointer data
    *blue = 0;  //clear pointer data
    
    if(color >= 0 && color <36){                    //check red spectrum
        *red = (MAX_COLOR/36) * color;              //calculate red
    }
    if (color >= 36 && color<72) {                  //check red-green
        *red = MAX_COLOR;                           //red to max
        *green = (MAX_COLOR/36) * (color-36);       //mix in green
    }
    if (color >= 72 && color<108) {                 //green -red
        *green = MAX_COLOR;                         //green max
        *red = 255-((MAX_COLOR/36) * (color-72));   //lower red
    }
    if (color >= 108 && color<144) {                //green +blue
        *green = MAX_COLOR;                         //green max
        *blue = (MAX_COLOR/36) * (color-108);       //add in blue
    }
    
    if (color >= 144 && color<180) {                //blue -green
        *blue = MAX_COLOR;                          //blue max
        *green = 255-((MAX_COLOR/36) * (color-144));//lower green
    }
    if (color >= 180 && color<216) {                //blue +red
        *blue = MAX_COLOR;                          //blue max
        *red = (MAX_COLOR/36) * (color-180);        //mix in red
    }
    if (color >= 216 && color<252) {                //red - blue
        *red = MAX_COLOR;                           //red max
        *blue = 255-((MAX_COLOR/36) * (color-216)); //lower blow
    }
    if (color>=252) {                               //default limit
        *red = MAX_COLOR;
        
    }
    return;
}
