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
//#include "keypad.h"
#include "lcd.h"
#include "iopin.h"

void printb(int n);

void printb(int n) {
    for(int a = 1<<8; a > 0; a>>=1){
        if(n & a)printf("1");
        else printf("0");
    }
}

void main(void) {
    
    initLCD();
    TRISA=0;
    printb(TRISB);
    pinMode(B,1,INPUT);
    printf("\n");
    printb(TRISB);
    while(1){
        __delay_ms(1000);
    }
    
    return;
}
 