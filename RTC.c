/* 
 * File:   RTC.c
 * Author: Nam
 *
 * Created on February 27, 2017, 2:10 PM
 */

#include <xc.h>
#include "I2C.h"

const char happynewyear[7] = {  0x00, //45 Seconds 
                            0x00, //59 Minutes
                            0x00, //24 hour mode, set to 23:00
                            0x01, //Saturday
                            0x08, //31st
                            0x04, //December
                            0x17};//2016

void initRTC(){
    I2C_Master_Init(10000); //Initialize I2C Master with 100KHz clock
}

int* getTime(){
    int time[7];
    
    di();
    
    ///Reset RTC memory pointer 
    I2C_Master_Start(); //Start condition
    I2C_Master_Write(0b11010000); //7 bit RTC address + Write
    I2C_Master_Write(0x00); //Set memory pointer to seconds
    I2C_Master_Stop(); //Stop condition

    //Read Current Time
    I2C_Master_Start();
    I2C_Master_Write(0b11010001); //7 bit RTC address + Read
    for(unsigned char i=0;i<6;i++){
        time[i] = I2C_Master_Read(1);
    }
    time[6] = I2C_Master_Read(0);       //Final Read without ack
    I2C_Master_Stop();
    
    ei();
    
    return time;
}

void setTime(void){
    di();
    I2C_Master_Start(); //Start condition
    I2C_Master_Write(0b11010000); //7 bit RTC address + Write
    I2C_Master_Write(0x00); //Set memory pointer to seconds
    for(char i=0; i<7; i++){
        I2C_Master_Write(happynewyear[i]);
    }    
    I2C_Master_Stop(); //Stop condition
    ei();
}