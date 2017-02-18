
/*
 * lcd library, offers general purpose functions for interfacing with
 * the lcd screen. This library uses D pins for communication with standard
 * 2x16 LCD screen.
 */

#include <xc.h>
#include "configBits.h"
#include <stdio.h>
#include "lcd.h"
#include "constants.h"

void initLCD(void) {
    __delay_ms(15);
    lcdInst(0b00110011); //set to 8 bit mode
    lcdInst(0b00110010); //
    lcdInst(0b00101000); //set to 4 bit, set 2 display lines
    lcdInst(0b00001111);//display on, cursor on, blink cursor on
    lcdInst(0b00000110);//increment cursor after writing byte, don't shift display after writing
    lcdInst(0b00000001);//clear display
    __delay_ms(15);
}

void lcdInst(char data) {
    RS = 0;
    lcdNibble(data);
}

//note printf() uses this to write to screen)
void putch(char data){
    if(data == '\n')lcdNewline();
    else
    {
        RS = 1;
        lcdNibble(data);
    }
}

void lcdNibble(char data){
    // Send of 4 most sig bits, then the 4 least sig bits (MSD,LSD)
    char temp = data & 0xF0;
    LATD = LATD & 0x0F;
    LATD = temp | LATD;

    E = 0;
    __delay_us(LCD_DELAY);
    E = 1;    
    __delay_us(LCD_DELAY);
    
    data = data << 4;
    
    temp = data & 0xF0;
    LATD = LATD & 0x0F;
    LATD = temp | LATD;

    E = 0;
    __delay_us(LCD_DELAY);
    E = 1;
    __delay_us(LCD_DELAY);
}

void lcdClear(void){
    lcdInst(0b00000001);
}

//goes to the next line, if cursor is on the last line then it will stay where it is
void lcdNewline(void){
    lcdInst(0b11000000);
}