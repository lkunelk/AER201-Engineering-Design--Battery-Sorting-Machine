/*
 * File:   main.c
 * Author: Nam Nguyen
 * Created on Feb 18, 2017, 12:11 PM
 *
 * Main procedure for the Battery Sorting Machine
 * 
 * Goal of Machine:
 * - given 15 batteries or less (type: AA, 9V, C) seperate the batteries
 * autonomously into 4 different categories: Charged AA, Charged 9V, Charged C,
 * Discharged and place each category into a separate bin.
 * 
 * Structure of physical machine:
 * 
 * rotating cylinder
 * ---------------------------------------------------------
 * batteries are placed into a cylinder which picks up batteries
 * and drops them onto a conveyor belt running in the middle of cylinder. 
 * The cylinder is run by a DC motor
 * 
 * conveyor belt
 * -------------------------------------------------------------
 * transports batteries outside of cylinder. 
 * Uses a continuous servo motor
 * 
 * testing channel
 * -----------------------------------------------------------
 * the batteries then drop into a testing v-shaped channel.
 * The battery triggers a contact switch as it falls into place. The testing channel
 * consists of 2 voltage measuring pads, which are embedded with metal contacts at
 * different locations. This helps to differentiate between batteries depending 
 * on which contacts detect a voltage. One pad is static while the other can rotate
 * to clamp or release the battery. The dynamic pad is controlled using a servo.
 * 
 * redirecting arm
 * -----------------------------------------------------------
 * below the testing channel is the redirecting arm. It points towards the 4
 * different bins. Depending on what type the testing channel detected the arm
 * will point to the correct bin. The dynamic pad above will release the battery
 * which will fall onto the redirecting arm and make its way to the bin. The 
 * directing arm is controlled using a servo motor.
 * 
 * 
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

void pinSetup(void);
void sortBattery(void);
void pause(char* message);

int batteryDetected = 0; //1 == contact switch activated on machine

int    redirectAngle_AA = 60; //angles for the re-directing arm
int     redirectAngle_C = 80;
int    redirectAngle_9V = 100;
int redirectAngle_OTHER = 120;

int   padAngle_CLOSE = 70; //angles for voltage testing pads
int padAngle_NEUTRAL = 90;
int   padAngle_OPEN  = 180;

int    cylinderMotor[2] = {   C, 0}; //no timer port C, pin 0
int    conveyorServo[3] = {0, C, 1}; //timer 0, port C, pin 1
int         padServo[3] = {1, C, 2}; //timer 1, port C, pin 2
int redirectingServo[3] = {3, C, 3}; //timer 3, port C, pin 3

//measure voltage and determine battery type
int AA_float[2] = {A,3}; //pin for helping differentiate AA from 9V, for some case
int  padPin1[2] = {A,1}; //digital port A, pin 1
int  padPin2[2] = {A,2}; //digital port A, pin 2
int  padPin3[2] = {A,0}; //analog 0 is the channel not the pin 
                     //(in this case channel 0 is pin 0),
                     //port value not needed only for reference

float V_LIM_AA = 0; //Voltage limits (above => charged, below => discharged)
float V_LIM_C  = 0;
float V_LIM_9V = 0;

void main(){
    pinSetup();
    initLCD();
    
    while(1){
        showInterface();

        //start up motors
        digitalWrite(cylinderMotor[0], cylinderMotor[1], HIGH);
        initServo(conveyorServo[0],    conveyorServo[1],   conveyorServo[2],    90);
        initServo(padServo[0],         padServo[1],        padServo[2],         padAngle_NEUTRAL);
        initServo(redirectingServo[0], redirectingServo[1],redirectingServo[2], 90);
        
        lcdClear();
        printf("running");
        
        // poll the time if it exceeds some amount stop process
        while(1){
            while(!batteryDetected){
                //poll timer if waiting exceeds limit stop the whole process
            }
            
            sortBattery();
            
            //reset flag after sorting
            batteryDetected = 0;
        }
        
        //display results
        
        
        while(1); //stop here for now
    };//stop here
}

void sortBattery(){
    
    //stop cylinder and conveyor belt
    stopServo(conveyorServo[0]);
    digitalWrite(cylinderMotor[0], cylinderMotor[1], LOW);
    
    //wait for battery to fall in
    //__delay_ms(1000);
    
    //compress battery
    pause("interrupt!!!\nclose?");
    setAngle(padServo[0], padAngle_CLOSE);
    
    //measure voltage
    pause("read voltage?");
    float Vcc = 4.8; //voltage of vcc pin on pic
    float resolution = (1<<10) - 1;
    
    int targetAngle;
    int signal = digitalRead(padPin1[0],padPin1[1])<<1; //combine 2 reads
    signal    |= digitalRead(padPin2[0],padPin2[1]);
    
    digitalWrite(AA_float[0], AA_float[1], HIGH); //set floating pin before reading
    float V = analogRead(padPin3[1]) / resolution * Vcc; //voltage read
    
    lcdClear();
    printf("target: %d\nV: %f",signal,V);
    readKeypad();
    //set the angle for directing arm
    /*pause("set redirect angle?");
    switch(signal){
        case 0b00: 
            //float pin to ground to differentiate AA from 9V
            digitalWrite(AA_float[0], AA_float[1], LOW);
            __delay_ms(1); //let voltage changes occur
            float V_float = analogRead(padPin3[1]) / resolution * Vcc; //voltage read
            if(V_float < 0.1){ //if below then it's AA battery
                if(V > V_LIM_AA) targetAngle = redirectAngle_AA;
                else             targetAngle = redirectAngle_OTHER;
                break;
            }
            //else it's a 9V, fall through from case 0
        case 0b01:
            if(V > V_LIM_9V) targetAngle = redirectAngle_9V;
            else             targetAngle = redirectAngle_OTHER;
            break;
        case 0b10:
            if(V > V_LIM_C) targetAngle = redirectAngle_AA;
            else             targetAngle = redirectAngle_OTHER;
            break;
    }
    
    setAngle(redirectingServo[0], targetAngle);
    */
    
    lcdClear();
    printf("pick angle: 1,2,3,4?");
    int dir[4] = {100,95,90,85};
    setAngle(redirectingServo[0], dir[readKeypad() - '0']);
    
    //release battery
    pause("release battery?");
    setAngle(padServo[0], padAngle_OPEN);
    
        
    //set gate to the resting state
    pause("reset the pad?");
    setAngle(padServo[0], padAngle_NEUTRAL);
    
    //turn on the conveyor belt and cylinder
    pause("conveyor & cylinder on?");
    initServo(conveyorServo[0],    conveyorServo[1],   conveyorServo[2],    90);
    digitalWrite(cylinderMotor[0], cylinderMotor[1], HIGH);
    
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
    ADCON1 |= 0x0E; //set V reference and set an0 (ra0) as analog
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

void interrupt service(void) {
    
    servoInterruptService();
    
    //Contact sensor - port B, pin 0 external interrupt
    if(INT0IF){ INT0IF = 0; //clear flag
        batteryDetected = 1;
    }
    
    //Keyboard - port B, pin 1 external interrupt
    if(INT1IF){INT1IF = 0;     //Clear flag bit
        keyPressedInterruptService();
    }
}

void pause(char* message){
    lcdClear();
    printf(message);
    readKeypad();
}