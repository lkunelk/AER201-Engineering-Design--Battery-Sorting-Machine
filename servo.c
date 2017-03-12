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

int nServo = 0;
int curr = 0;
int order[4]; //store id number of servo
int port[4];
int pin[4];
long pulse[4];
long sequence[4];

//return id (int) of servo
int initServo(int po, int pi, int angle){
    int id = nServo;
    nServo++; //increment muber of servos
    port[id] = po;
    pin[id] = pi;
    //setAngle(id, angle);
    
    port = {C,C,C};
    pin = {0,1,2};
    order = {0,1,2};
    sequence = {750, 0, 0};
    
    
    initTimer(0);
    startTimer(0,0);
    
    
    
    //printf("%d %d\n",order[0],order[1]);
    //printf("%d %d",order[2],order[3]);
    
    return id;
}

void setAngle(int id, int angle){
    pulse[id] = angleToPulse(angle);
    
    //reorder
    int taken[10];
    for(int a = 0; a < nServo; a++) taken[a] = 0; //set all to 0
    
    int ind = 0;
    for(int a = 0; a < nServo; a++){
        long smallest = 0xffff;
        int smallest_ind = -1;
        for(int b = 0; b < nServo; b++){
            if(!taken[b] && pulse[b] < smallest){
                smallest = pulse[b];
                smallest_ind = b;
            }
        }
        taken[smallest_ind] = 1;
        order[ind++] = smallest_ind;
    }
    
    //determine times for the sequence
    int prev = 0;
    for(int a = 0; a < nServo; a++){
        sequence[a] = pulse[order[a]] - prev;
        prev = pulse[order[a]];
    }
    sequence[nServo] = 50000;
    
    lcdClear();
    printf("%ld %ld %ld %ld\n",pulse[0],pulse[1],pulse[2],pulse[3]);
    printf("%ld %ld %ld %ld",sequence[0],sequence[1],sequence[2],sequence[3]);
}

long angleToPulse(int angle){
    long offset = -500; //offset for tuning range
    long a000 = 1250 + offset; // pulse for angle of 0
    long a180 = 6250 + offset; // pulse for angle of 180
    
    return (long)(a000 + angle/180.0*(a180-a000));
}

void servoInterruptService(){
    if(TMR0IF){ TMR0IF = 0;
        if(curr == nServo){
            curr = 0;
            for(int i = 0; i < nServo; i++)
                digitalWrite(port[i], pin[i], HIGH);
            startTimer(0, 0xffff - sequence[order[curr]]);
        }
        else{
            curr++;
            startTimer(0, 0xfffe - sequence[order[curr]]);
            digitalWrite(port[order[curr-1]],pin[order[curr-1]],LOW);
        }
    }
}