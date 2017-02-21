
/*
 * File:   iopin.c
 * Author: Nam Nguyen
 *
 * Created on Feb 20, 2016, 7:38 PM
 */

#include <xc.h>

//pin is chosen from constants
void pinModeA(int pin, int type){
    pin = 1 << pin;
    if(type == 1){ //input
        pin ^= 0xff;
        TRISA &= pin;
    } 
    else TRISA |= pin; //output
}

void pinModeB(int pin, int type){
    
}

void pinModeC(int pin, int type){
    
}

void pinModeD(int pin, int type){
    
}

int digitalReadA(){
    
}

int digitalReadB(){
    
}

int digitalReadC(){
    
}

int digitalReadD(){
    
}

int digitalWrite(){
    
}