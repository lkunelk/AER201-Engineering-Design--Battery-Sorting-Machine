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

void initServo(){
    initT0();
    startT0(0xffd1);
    digitalWrite(C,0,HIGH);
}

int flag0 = 1;
void updateAngle(float angle){
    long hi = 0xffe1;
    long lo = 0xfdb7;
    
    if(TMR0IF){ TMR0IF = 0; //clear flag
        
        if(flag0){
            startT0(lo);
            flag0=0;
            digitalWrite(C,0,LOW);
        }
        else{
            startT0(hi);
            flag0=1;
            digitalWrite(C,0,HIGH);
        }
    }
}