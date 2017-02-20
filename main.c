/*
 * File:   main.c
 * Author: Nam Nguyen
 *
 * library offers basic functions to interface with standard
 * LCD screen. The main file tests these functions as they are implemented.
 * 
 * Created on July 18, 2016, 12:11 PM
 */


#include <xc.h>
#include <stdio.h>
#include "configBits.h"
#include "constants.h"
#include "keypad.h"
#include "lcd.h"


void main(void) {
    
    initLCD();
    initKeypad();
    
    
    while(1){
        lcdClear();
        printf("Hello world!\n");
        char c = read();
        lcdClear();
        printf("%c was pressed",c);
        
        __delay_ms(1000);    
    }
    
    return;
}
 