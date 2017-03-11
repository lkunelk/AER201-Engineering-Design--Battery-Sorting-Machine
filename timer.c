/* 
 * File:   timer.c
 * Author: Nam Nguyen
 *
 * Created on February 25, 2017, 12:25 PM
 * 
 * pic18 has 4 timer modules
 * timer0 
 * - can be configured to be 8or16 bit timer/counter
 * - 8 bit mode set T0CON<6> (6th bit)
 * - has 3 registers:
 *  - T0CON (control register)
 *  - TMR0L (lower data)
 *  - TMR0H (higher data)
 * timer0 operates as timer or counter depending on clock source
 * timer - internal instruction cycle clock
 * counter - RA4/T0CLK pin (set T0CON<5> to get counter mode)
 * - to start counter/timer set T0CON<7>
 * 
 * prescaler (clear T0CON<3> to assign prescaler)
 * - can increase number of pulses per increment of counter/timer in pow of 2
 * - max 256, min 2
 * - prescaler value determined by T0CON<2:0> -> 111 is 1/256 and 000 is 1/2
 * 
 * TMR0H is buffered version of real high bit
 * writing to it stores the value in a buffer
 * only when TMR0L is set TMR0H is set simultaneously
 * 
 * TMR0 interrupt generated when TMR0 register overflows 
 * from 0xff t0 0x00 (8bit)
 * from 0xffff to 0x0000 (16bit)
 * 
 * interrup is set by setting INTCON<5>
 * overflow sets flag INTCON<2>
 *
 * the external oscillator in use ~10MHz
 * experimental measurement: 9.982464MHz +- 0.000500Hz
 * 
 * 9982464 - too slow
 * 9959472 - too slow
 * 9893064 - too slow
 * 9884827 - just right?
 * 9826656 - too fast
 */

#include <xc.h>
#include <stdio.h>
#include "timer.h"
#include "I2C.h"


void initTimer(int timer){
    switch(timer){
        case 0: //default 16bit timer
            T0CON = 0; //clear settings
            T0CON |= 1<<3; //turn off prescaler
            //T0CON |= 0b111; // prescaler = 2^(0b110 + 1)) = 128
            TMR0IE = 1; //enable timer0 interrupt
            break;
        case 1:
            T1CON = 0; //clear settings
            //prescaler default = 1
            TMR1IE = 1; //enable timer0 interrupt
            break;
        case 3:
            T3CON = 0; //clear settings
            TMR3IE = 1;
            break;
    }
    PEIE = 1; //turn off interrupt priorities
    ei(); //enable general interrupts
}

//start timer with precalculated time
void startTimer(int timer, long val){
    switch(timer){
        case 0:
            TMR0H = val>>8; //high bits of val
            TMR0L = val;    // low bits of val
            T0CON |= 1<<7;  //start timer
            break;
        case 1:
            TMR1H = val>>8; //high bits of val
            TMR1L = val;    // low bits of val
            T1CON |= 1<<0;  //start timer
            break;
        case 3:
            TMR3H = val>>8; //high bits of val
            TMR3L = val;    // low bits of val
            T3CON |= 1<<0;  //start timer
    }
}

long getTimerCount(int timer){
    switch(timer){
        case 0: return TMR0L + ( TMR0H >> 8 ) ;
        case 1: return TMR1L + ( TMR1H >> 8 ) ;
        case 3: return TMR3L + ( TMR3H >> 8 ) ;
    }
}

//counts number of oscillations of crystal in 1 second
float testFrequency(){
    I2C_Master_Init(10000); //initialize master with 100KHz clock
    
    int time;
    int prev = 0;
    int timerOff = 1;
    int first = 1;
    
    while(1){
        //Reset RTC memory pointer 
        I2C_Master_Start(); //Start condition
        I2C_Master_Write(0b11010000); //7 bit RTC address + Write
        I2C_Master_Write(0x00); //Set memory pointer to seconds
        I2C_Master_Stop(); //Stop condition

        //Read Current Time
        I2C_Master_Start();
        I2C_Master_Write(0b11010001); //7 bit RTC address + Read
        time = I2C_Master_Read(0);
        I2C_Master_Stop();
        
        if(time^prev){ //if time changes (is different from prev value))
            if(first) //ignore the first difference in time and prev
                first = 0;
            else
                if(timerOff){
                    printf("t1: %x ",time);
                    initT0();
                    startT0(0);
                    timerOff = 0;
                }
                else{
                    di();
                    printf("[%x %x]\n",TMR0L, TMR0H);
                    T0CON = 0; //stop timer
                    long count = TMR0L + (TMR0H<<8);
                    return count*256*4 / 1000000.0; //convert to MHz
                }
        }
        
        prev = time;
    }
    return -1;
}

