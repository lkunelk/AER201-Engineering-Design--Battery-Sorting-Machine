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
#include "keypad.h"
#include "lcd.h"


void main(void) {
    
    initLCD();
    initKeypad();
    
    int time = 0;
    while(1){
        int min = time/60;
        int sec = time%60;
        printf("%d:%d",min,sec);
        
        __delay_ms(1000);
        lcdClear();
        time++;
    }
    
    return;
}
 