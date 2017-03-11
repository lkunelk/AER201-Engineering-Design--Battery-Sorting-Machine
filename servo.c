/* 
 * File:   servo.c
 * Author: Nam
 *
 * Created on February 27, 2017, 1:44 PM
 */

#include <xc.h>
#include "servo.h"
#include "iopin.h"
#include "timer.h"

int port[4], pin[4];
long pulse[4];

void initServo(int timer, int po, int pi){
    port[timer] = po;
    pin[timer]  = pi;
    pulse[timer] = 61785; //default 1.5ms
    
    initTimer(timer);
    startTimer(timer,61785);
    digitalWrite(po,pi,HIGH);
}

void setAngle(int timer, long angle){
    pulse[timer] = angle;
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