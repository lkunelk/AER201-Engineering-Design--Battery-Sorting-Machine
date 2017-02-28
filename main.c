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
#include "iopin.h"
#include "eeprom.h"
#include "I2C.h"
#include "timer.h"
#include "servo.h"
//#include "RTC.h"

void main(){
    
    //**OSCILLATOR**//
    OSCCON = 0xF0; // Force internal oscillator operation at 8 MHz (pg. 7-39)
    OSCTUNEbits.PLLEN = 1; // set  OSCTUNE<6> to enable PLL (4x  phase-lock-loop) thereby setting internal oscillator frequency to 32 MHz
    
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
    //ADCON1 |= 0b11<<4; //use external voltage reference 
    //(an2 -> Vref-) and (an3 -> Vref+)
    
    ADCON2 = 0; //clear before setting
    ADCON2 |= 0b001; //set Tad = 1/ (Fosc/8)
    ADCON2 |= 0b110<<3; //set conversion rate  16 * Tosc per bit
    ADCON2 |= 1<<7; //set right justified result
    
    initLCD();
    
    //initServo(); //servo test
    
    //printf("%ld",analogRead(0)); //analog test
    
    digitalWrite(C, 1, HIGH);
    
    while(1){};//stop here
}


int i = 0, count = 0;
float angle = 1.5;

void interrupt service(void) {
    
    updateAngle(angle); //servo
    
    if(INT1IF){ //keyboard
        
        lcdClear();
        char key = (PORTB & 0xF0) >> 4;
        if(key == 2) angle +=.1;
        if(key == 3) angle -=.1;
        printf("angle %f",angle);
        INT1IF = 0;     //Clear flag bit
    }
}