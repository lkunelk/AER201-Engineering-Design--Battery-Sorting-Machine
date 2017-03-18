
/*
 * File:   iopin.c
 * Author: Nam Nguyen
 *
 * Created on Feb 20, 2016, 7:38 PM
 * 
 * analog pins:
 * an0 an1 an2 an3 an4 an5 an6 an7 an8 an9 an10 an11 an12
 * ra0 ra1 ra2 ra3 ra5 re0 re1 re2 rb2 rb3 rb1  rb4  rb0 
 * 
 */

#include <xc.h>
#include "iopin.h"

//pin is chosen from constants
void digitalPinMode(int port, int pin, int type){
    pin = 1 << pin;
    if(type == OUTPUT){
        pin ^= 0xff;
    } 
    
    switch(port){
        case A: if(type == INPUT)TRISA |= pin;
                else TRISA &= pin;
                break;
        
        case B: if(type == INPUT)TRISB |= pin;
                else TRISB &= pin;
                break;
              
        case C: if(type == INPUT)TRISC |= pin;
                else TRISC &= pin;
                break;
                
        case D: if(type == INPUT)TRISD |= pin;
                else TRISD &= pin;
                break;
    }
}

int digitalRead(int* pin){
    switch(pin[0]){
        case A: return (PORTA>>pin[1])&1;
        case B: return (PORTB>>pin[1])&1;
        case C: return (PORTC>>pin[1])&1;
        case D: return (PORTD>>pin[1])&1;
    }
    return -1;
}

void digitalWrite(int* pin, int val){
    int p = 1 << pin[1];
    if(val == LOW){
        p ^= 0xff;
    } 
    
    switch(pin[0]){
        case A: if(val == HIGH) LATA |= p;
                else LATA &= p;
                break;
        case B: if(val == HIGH) LATB |= p;
                else LATB &= p;
                break;
        case C: if(val == HIGH) LATC |= p;
                else LATC &= p;
                break;
        case D: if(val == HIGH) LATD |= p;
                else LATD &= p;
    }
}

//I don't actually use this, this serves as a reference
void analogPinMode(){
    ADCON0 = 0; //turn off AD module for now
    
    //ADCON1 |= 0b11<<4; //use external voltage reference 
    //(an2 -> Vref-) and (an3 -> Vref+)
    ADCON1 |= 0x0A; //set V reference and set an0-4 (ra0-5) as analog
    
    ADCON2  = 0; //clear before setting
    ADCON2 |=     1<<0; //set Tad = 1/ (Fosc/8)
    ADCON2 |= 0b110<<3; //set conversion rate  16 * Tosc per bit
    ADCON2 |=     1<<7; //set right justified result
}

long analogRead(int channel){
    ADCON0 = channel<<2; // select channel bits<5-2>
    ADCON0 |=      1<<0; //turn on the AD module
    ADCON0 |=      1<<1; //tell AD module to start conversion
    
    while(ADCON0 & 0b10); //waiting for result
    
    return (ADRESH<<8) + ADRESL; //return the result as long
}