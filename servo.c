/* 
 * File:   servo.c
 * Author: Nam
 *
 * Created on February 27, 2017, 1:44 PM
 */

#include <xc.h>
#include <stdio.h>
#include "lcd.h"
#include "servo.h"
#include "iopin.h"
#include "timer.h"

int servo[4][2];
long pulse[4];

//motor = {timer, port, pin}
void initServo(int* motor, int angle){
    int timer = motor[0];
    int port = motor[1];
    int pin = motor[2];
    
    servo[timer][0] = port;
    servo[timer][1]  = pin;
    setAngle(&timer, angle); //initial angle
    
    initTimer(timer);
    startTimer(timer,0);
}

void stopServo(int timer){
    switch(timer){
        case 0: T0CON = 0; break;
        case 1: T1CON = 0; break;
        case 2: T2CON = 0; break;
        case 3: T3CON = 0; break;
    }
}

long angleToPulse(int angle){
    long offset = 0; //offset for tuning range
    long a000 = 1250; // pulse for angle of 0
    long a180 = 6250; // pulse for angle of 180
    
    return a000 + angle/180.0*(a180-a000) + offset;
}

void setAngle(int* motor, int angle){
    pulse[motor[0]] = 0xffff - angleToPulse(angle);
}

void servoInterruptService(){
    
    int flags[4];
    flags[0] = TMR0IF;
    flags[1] = TMR1IF;
    flags[2] = TMR2IF;
    flags[3] = TMR3IF;
    
    for(int i = 0; i < 4; i++){
        if(flags[i]){

            if(digitalRead(servo[i])){ //if it was high
                startTimer(i,15535); //20ms
                digitalWrite(servo[i],LOW);
            }
            else{
                startTimer(i, pulse[i]);
                digitalWrite(servo[i],HIGH);
            }
            
            //clear flag
            switch(i){
                case 0: TMR0IF = 0; break;
                case 1: TMR1IF = 0; break;
                case 2: TMR2IF = 0; break;
                case 3: TMR3IF = 0; break;
            }
        }
    }
}