/*
 * File:   eeprom.c
 * Author: Nam Nguyen
 *
 * Created on Feb 24, 2016, 9:46AM
 * 
 * EEPROM is a type of data memory which remains on the device even after power
 * is turned off.
 * 
 * PIC18F4620 comes with 1024byte EEPROM
 * 5 special function registers control its operation:
 * - EEDATA - store data that is read or written to
 * - EECON1 - control operations
 * - EECON2 - control operations
 * - EEADR - specifies lower part of 10 bit address
 * - EEADRH - specifies the higher part of 10 bit address
 */

#include <xc.h>
#include "eeprom.h"

//
int* readPastRun(int run){
    int data[7];
    for(int i = 0; i < 7; i++){
        data[i] = eepromRead((run*7+1) + i);
    }
    return data;
}

void saveNewRun(int* data){
    //update total number of runs being stored
    eepromWrite(eepromRead(0)+1,0);
    int n = eepromRead(0);
    
    for(int i = 0; i < 7; i++){
        eepromWrite(data[i], (n*7+1) + i);
    }
}

void eepromWrite(int data, long address){
    
    while(EECON1 & 0b10); //wait for any previous procedure to finish
    
    int ien = _INTCONbits.GIE; //save the state of interrupt flag
    di(); //disable interrupts
    EEADR  =  address; //put lower part in
    EEADRH = (address>>8) & 0b11; //put higher part of address
    EEDATA = data;
    EECON1 = 0b100; //set WREN bit to enable operations
    
    EECON2 = 0x55; //special procedure before we write
    EECON2 = 0xaa; //setting EECON2 prevents inadvertent writes 
    //ie. we actually want to write to the eeprom and we didn't 
    //accidently set the EECON1 WR by mistake
    
    EECON1 |= 0b10; //set WR bit to start writing
    
    while(EECON1 & 0b10); //wait for write procedure to finish
    EEIF = 0; //clear the flag
    
    EECON1 = 0; //clear WREN to disable operations
    _INTCONbits.GIE = ien; //restore the interrupt setting
}   

int eepromRead(long address){
    EEADR  =  address; //put lower part in
    EEADRH = (address>>8) & 0b11; //put higher part of address
    EECON1 = 0; //clear EEPGD, now eeprom is selected
    EECON1 = 1; //start reading procedure
    return EEDATA; //return result
}