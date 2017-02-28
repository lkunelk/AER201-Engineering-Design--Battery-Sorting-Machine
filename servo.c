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
    startT0(1.5);
    digitalWrite(D,0,HIGH);
}

int flag = 1;
void updateAngle(float angle){
    if(TMR0IF){
        TMR0IF = 0; //clear flag
        
        if(flag){
            flag=0;
            digitalWrite(D,0,LOW);
            startT0(20.0-angle);
        }
        else{
            flag=1;
            digitalWrite(D,0,HIGH);
            startT0(angle);
        }
    }
}