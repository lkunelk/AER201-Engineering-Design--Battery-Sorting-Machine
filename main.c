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
#include "interface.h"
//#include "RTC.h"

void pinSetup(void);
void showInterface(void);

void pinSetup(){
    //**OSCILLATOR**//
    //OSCCON = 0xF0; // Force internal oscillator operation at 8 MHz (pg. 7-39)
    //OSCTUNEbits.PLLEN = 1; // set  OSCTUNE<6> to enable PLL (4x  phase-lock-loop) thereby setting internal oscillator frequency to 32 MHz
    
    //set direction of pins
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
    ADCON2 |= 0b010; //set Tad = 1/ (Fosc/32), (must be Tas>=0.7us)
    ADCON2 |= 0b110<<3; //set conversion rate  16 * Tosc per bit
    ADCON2 |= 1<<7; //set right justified result
    
    //interrupts
    //INT1IE = 1;
}

void showInterface(){
    char key; //for storing keypad input
    while(1){
        dateTime();
        while(readKeypad() != 'A');
        
        while(1){
            //---------
            mainMenu();
            //---------
            
            do{key = readKeypad();}
            while(key != 'B' && key != 'C' && key != '*');
                
            if(key == 'C'){ 
                //---------
                runSelect();
                //---------
                
                //read eeprom for past runs
                
                do{key = readKeypad();}
                while(key != '1' && key != '2' && key != '3' && key != '4' && key != '*');
                
                if(key == '*')continue;
                
                //check that there's something in memory
                
                while(1){ 
                    //---------
                    runMenu(key);
                    //---------

                    do{key = readKeypad();}
                    while(key != 'A' && key != 'B' && key != '*');

                    if(key == 'A'){ 
                        //---------
                        runTime();
                        //---------
                    }
                    else if(key == 'B'){
                        //---------
                        runStats();
                        //---------
                    }
                    else if(key == '*')break;
                    
                    while(readKeypad() != '*');
                }
                        
            }
            else if(key == 'B'){
                //start
            }
            else if(key == '*')break;
        }
    }
}

long angle = 61785;

void main(){
    pinSetup();
    initLCD();
    
    //printf("done");
    
    initServo(C,0,0);
    initServo(C,1,180);
    //initServo(C,2,180);
    
    //printf("f: %f",testFrequency());
    
    while(1){};//stop here
}

int a0 = 0;
int a1 = 90;
int a2 = 180;
void interrupt service(void) {
    
    servoInterruptService();
    
    //RB1 interrupt
    if(INT1IF){INT1IF = 0;     //Clear flag bit
        char key = (PORTB & 0xF0) >> 4;
        //if(key == 0)a0+=10;
        //if(key == 1)a0-=10;
        
        if(key == 4)setAngle(1,0);
        if(key == 5)setAngle(1,90);
        if(key == 6)setAngle(1,180);
        
        //if(key == 8)a2+=10;
        //if(key == 9)a2-=10;
        
        //lcdClear();
        //printf("%d %d %d",a0,a1,a2);
    }
}