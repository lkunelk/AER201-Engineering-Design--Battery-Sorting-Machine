/* 
 * File:   servo.c
 * Author: Nam
 *
 * Created on February 27, 2017, 1:44 PM
 */

#include <xc.h>
#include <stdio.h>
#include "servo.h"
#include "iopin.h"
#include "timer.h"

int port[4], pin[4];
long pulse[4];

void initServo(int timer, int po, int pi, int angle){
    port[timer] = po;
    pin[timer]  = pi;
    setAngle(timer, angle); //default 1.5ms
    
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

void setAngle(int timer, int angle){
    pulse[timer] = 0xffff - angleToPulse(angle);
    lcdClear();
    printf("%ld",pulse[timer]);
}

void servoInterruptService(){
    
    int flags[4];
    flags[0] = TMR0IF;
    flags[1] = TMR1IF;
    flags[2] = TMR2IF;
    flags[3] = TMR3IF;
    
    for(int i = 0; i < 4; i++){
        if(flags[i]){

            if(digitalRead(port[i],pin[i])){ //if it was high
                startTimer(i,15535); //20ms
                digitalWrite(port[i],pin[i],LOW);
            }
            else{
                startTimer(i, pulse[i]);
                digitalWrite(port[i],pin[i],HIGH);
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