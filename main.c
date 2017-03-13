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
#include "RTC.h"

void pinSetup(void);
void showInterface(void);
void sortBattery(void);

int switchFlag = 0;

int redirectAngle_AA = 60; //angles for the re-directing arm
int redirectAngle_C  = 80;
int redirectAngle_9V = 100;
int redirectAngle_OTHER = 120;

int padAngle_CLOSE = 70; //angles for voltage testing pads
int padAngle_CATCH = 90;
int padAngle_OPEN  = 180;

int    cylinderMotor[2] = {   C, 0}; //         port C, pin 1
int    conveyorServo[3] = {0, C, 1}; //timer 0, port C, pin 1
int         padServo[3] = {1, C, 2}; //timer 1, port C, pin 2
int redirectingServo[3] = {3, C, 3}; //timer 3, port C, pin 2

void main(){
    pinSetup();
    initLCD();
    //initRTC();
    
    while(1){
        di();
        showInterface();
        ei();

        //start up motors
        initServo(conveyorServo[0],    conveyorServo[1],   conveyorServo[2],    90);
        initServo(padServo[0],         padServo[1],        padServo[2],         padAngle_CATCH);
        initServo(redirectingServo[0], redirectingServo[1],redirectingServo[2], 90);
        
        // poll the time if it exceeds some amount stop process
        while(1){
            while(!switchFlag);
            switchFlag = 0;
            sortBattery();
        }
        //display results
        while(1); //stop here for now
    };//stop here
}

void sortBattery(){
    
    lcdClear();
    printf("interrupt!");
    
    //stop cylinder and conveyor belt
    stopServo(conveyorServo[0]);
    digitalWrite(cylinderMotor[0], cylinderMotor[1], LOW);
    
    //wait for battery to fall in
    //__delay_ms(1000);
    
    readKeypad();
    
    //compress battery
    setAngle(padServo[0], padAngle_CLOSE);
    
    lcdClear();
    printf("open?");
    //measure voltage
    readKeypad();
    //set the angle for directing arm
    lcdClear();
    printf("open!");
    //release battery
    setAngle(padServo[0], padAngle_OPEN);
    //set gate to the resting state
    
    readKeypad();
    //turn on the conveyor belt and cylinder
    initServo();
    while(1);
}

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
    INT1IE = 1; // external interrupt for keypad
    INT0IE = 1; // external interrupt on B0 for battery touch sensor
    ei();
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
            
            do{key = readKeypad();} //proceed only if 
            while(key != 'B' && key != 'C' && key != '*');
                
            if(key == 'C'){ // history of runs
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

                    if(key == 'A'){ //how long the process ran
                        //---------
                        runTime();
                        //---------
                    }
                    else if(key == 'B'){ //how  many batteries for each category
                        //---------
                        runStats();
                        //---------
                    }
                    else if(key == '*')break;
                    
                    while(readKeypad() != '*');
                }
                        
            }
            else if(key == 'B'){ //start
                return; //begin the process
            }
            else if(key == '*')//go back
                break;
        }
    }
}



void interrupt service(void) {
    
    servoInterruptService();
    
    //Contact sensor - port B, pin 0 external interrupt
    if(INT0IF){ INT0IF = 0; //clear flag
        switchFlag = 1;
    }
    
    //Keyboard - port B, pin 1 external interrupt
    if(INT1IF){INT1IF = 0;     //Clear flag bit
        char key = (PORTB & 0xF0) >> 4;
        if(key == 0)setAngle(0,30);
        if(key == 1)setAngle(0,90);
        if(key == 2)setAngle(0,135);
        
        if(key == 4)setAngle(3,80);
        if(key == 5)setAngle(3,90);
        if(key == 6)setAngle(3,100);
    }
}