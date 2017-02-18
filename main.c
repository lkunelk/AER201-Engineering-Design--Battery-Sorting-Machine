/*
 * File:   main.c
 * Author: Nam Nguyen
 *
 * library offers basic functions to interface with standard
 * LCD screen. The main file tests these functions as they are implemented.
 * 
 * Created on July 18, 2016, 12:11 PM
 */


#include <xc.h>
#include <stdio.h>
#include "configBits.h"
#include "constants.h"
#include "lcd.h"

#define __delay_1s() for(char i=0;i<100;i++){__delay_ms(10);}
#define __lcd_newline() lcdInst(0b11000000);
#define __lcd_clear() lcdInst(0x01);
#define __lcd_home() lcdInst(0b10000000);

const char keys[] = "123A456B789C*0#D";

void main(void) {
    TRISC = 0x00;
    TRISD = 0x00;   //All output mode
    TRISB = 0xFF;   //All input mode
    LATB = 0x00; 
    LATC = 0x00;
    ADCON0 = 0x00;  //Disable ADC
    ADCON1 = 0xFF;  //Set PORTB to be digital instead of analog default  
    
    
    while(1){
        initLCD();
        printf("Choose Option:");
        __lcd_newline();
        printf("A:show data B:start");
        
        while(PORTBbits.RB1 == 0){ 
            // RB1 is the interrupt pin, so if there is no key pressed, RB1 will be 0
            // the PIC will wait and do nothing until a key press is signaled
        }
        unsigned char keypress = (PORTB & 0xF0)>>4; // Read the 4 bit character code
        
        while(PORTBbits.RB1 == 1){
            // Wait until the key has been released
        }
        
        initLCD();
        
        Nop();  //Apply breakpoint here because of compiler optimizations
        Nop();
        unsigned char temp = keys[keypress];
        putch(temp);   // Push the character to be displayed on the LCD
        
        __delay_1s();
    }
    
    return;
}
 