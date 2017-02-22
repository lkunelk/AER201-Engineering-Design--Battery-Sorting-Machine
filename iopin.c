
/*
 * File:   iopin.c
 * Author: Nam Nguyen
 *
 * Created on Feb 20, 2016, 7:38 PM
 */

#include <xc.h>
#include "iopin.h"

//pin is chosen from constants
void pinMode(int port, int pin, int type){
    pin = 1 << pin;
    if(type == 1){ //input
        pin ^= 0xff;
    } 
    
    switch(port){
        case A: if(type == OUTPUT)TRISA |= pin;
                else TRISA &= pin;
                break;
        
        case B: if(type == OUTPUT)TRISB |= pin;
                else TRISB &= pin;
                break;
              
        case C: if(type == OUTPUT)TRISC |= pin;
                else TRISC &= pin;
                break;
                
        case D: if(type == OUTPUT)TRISD |= pin;
                else TRISD &= pin;
                break;
    }
}

int digitalRead(int port, int pin){
    switch(port){
        case A: return (PORTA>>pin)&1;
        case B: return (PORTB>>pin)&1;
        case C: return (PORTC>>pin)&1;
        case D: return (PORTD>>pin)&1;
    }
    return -1;
}

int digitalWrite(){
    
}