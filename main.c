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
    for(int a = 1<<7+0; a > 0; a>>=1){
        if(n & a)printf("1");
        else printf("0");
    }
}

void main(void) {
    
    //set direction
    TRISA = 0xFF; // Set Port A as all input
    TRISB = 0xFF; // input for keypads
    TRISC = 0x00;
    TRISD = 0x00; //All output mode for LCD
    TRISE = 0x00;  
    
    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;
    
    ADCON0 = 0x00; //turn off AD module for now
    ADCON1 |= 0x0A; //set V reference and set an0-4 (ra0-5) as analog
    ADCON1 |= 0b11<<4; //use external voltage reference 
    //(an2 -> Vref-) and (an3 -> Vref+)
    
    ADCON2 = 0; //clear before setting
    ADCON2 |= 0b001; //set Tad = 1/ (Fosc/8)
    ADCON2 |= 0b110<<3; //set conversion rate  16 * Tosc per bit
    ADCON2 |= 1<<7; //set right justified result
    
    initLCD();
    
    while(1){
        lcdClear();
        long voltage = analogRead(0);
        printf("%f\n",(float)voltage*(5.0/0x3ff));
        __delay_ms(1000);
    }
    
    return;
}