/* h
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

int n = 0; //number of servos
int* servos[4]; //store pins of each servo
unsigned int pulse[4]; //store the duration of pulse (angle) of the servos
int curr = 0; //current servo for which a high signal is being sent

//motor = {port, pin}
void initServo(int* motor, int angle){
    
    //check that we haven't initialized servo yet
    for(int i = 0; i < n; i++){
        if(servos[i] == motor)return; //if we did then do nothing and return
    }
    
    n++;
    servos[n-1] = motor;
    setAngle(motor, angle);
    
    initTimer(0);
    startTimer(0,0);
}

long angleToPulse(int angle){
    long offset = -600; //offset for tuning the range
    long a000 = 1250; // pulse length for angle of 0
    long a180 = 6250; // pulse length for angle of 180
    
    return a000 + angle/180.0*(a180-a000) + offset;
}

void setAngle(int* motor, int angle){
    unsigned int period = 50000; //instruction cycles for 10MHz clock = 20ms
    for(int i = 0; i < n; i++){
        if(servos[i] == motor){
            pulse[i] = angleToPulse(angle);
        }
        period-=pulse[i];
    }
    pulse[n] =  period; // the sum of all n+1 pulses should add up to 20ms
}

char servoInterruptService(){
    
    if(TMR0IF){TMR0IF = 0;
        digitalWrite(servos[curr],LOW); //clear previous one
        digitalWrite(servos[ (curr+1)%(n+1) ],HIGH); //set new one
        startTimer(0, 0xFFFF - pulse[ (curr+1)%(n+1) ]); //set new one for preset amount of time 
        
        curr = (curr+1)%(n+1);
        return 1;
    }
    return 0;
}