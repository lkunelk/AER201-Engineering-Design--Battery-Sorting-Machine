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

int nServo = 2;
int curr = 0;
;int order[4]  = {0,1,2,3}; //store id number of servo
int port[4]   = {C, C, -1, -1};
int pin[4]    = {0, 1, 0, 0};
long pulse[4] = {1500,1500,0,0};
long sequence[4] = {1250,1250,50000,0};

//return id (int) of servo
void initServo(int timer, int po, int pi){
    initTimer(0);
    startTimer(0,61785);
}

void setAngle(int id, long puls){
    pulse[id] = puls;
    
    //reorder
    int seen[10];
    for(int a = 0; a < nServo; a++)
        seen[a] = 0;
    
    int ind = 0;
    for(int a = 0; a < nServo; a++){
        long smallest = 0xffff;
        int smallest_ind = 0;
        for(int b = 0; b < nServo; b++){
            if(!seen[b] && pulse[b] < smallest){
                seen[b] = 1;
                smallest = pulse[b];
                smallest_ind = b;
            }
        }
        order[ind++] = smallest_ind;
    }
    
    //determine times for the sequence
    int prev = 0;
    for(int a = 0; a < nServo; a++){
        sequence[order[a]] = (pulse[order[a]] - prev) * 5 / 2;
        prev = pulse[order[a]];
    }
    lcdClear();
    printf("%ld %ld",sequence[0], sequence[1]);
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
            startTimer(0, 0xffff - sequence[order[curr]]);
            digitalWrite(port[order[curr-1]],pin[order[curr-1]],LOW);
        }
    }
}