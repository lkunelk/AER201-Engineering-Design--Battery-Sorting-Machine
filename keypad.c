
/*
 * Keypad library, this is specific to the development board in use.
 * provides basic functions for reading values from the 4x4 keypad
 * 
 *  the key layout is:
 *  |1|2|3|A|
 *  |4|5|6|B|
 *  |7|8|9|C|
 *  |*|0|#|D|
 * 
 * the keypad is read by another microcontroller which interfaces with the PIC
 * through the B ports. Port B1 is an interrupt port and it goes high whenever
 * a button is pressed. Ports B4-7 maintain the value of last pressed button.
 * Each button is assigned a binary number from 0 to 15 going from left to right
 * Port 4 holds lowest bit while port 7 holds the highest bit.
 */

#include <xc.h>
#include "keypad.h"

const char keys[] = "123A456B789C*0#D";

//sets up ports for input
void initKeypad(void){
    TRISB = 0xFF; //set all B ports as input
    LATB = 0x00;
    ADCON0 = 0x00;  //Disable ADC
    ADCON1 = 0xFF; //set PORTB to be digital instead of analog by default
}

//blocking function, waits until key is pressed
char read(){
    while(PORTBbits.RB1 == 0); //wait for press
    char key = (PORTB & 0xF0) >> 4; //read the keypress
    while(PORTBbits.RB1 == 1); //wait for release
    return keys[key];
}