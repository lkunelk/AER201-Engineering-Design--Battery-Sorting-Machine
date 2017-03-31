/* 
 * File:   interface.c
 * Author: Nam
 *
 * Created on March 11, 2017, 3:03 PM
 */

#include <xc.h>
#include <stdio.h>
#include "eeprom.h"
#include "interface.h"
#include "lcd.h"
#include "keypad.h"
#include "RTC.h"


void showInterface(){
    
    //initRTC();
    //setTime();
    char key; //for storing keypad input
    while(1){
        showDateTime();
        
        while(1){
            key = showMainMenu();
                
            if(key == 'C'){ // history of runs
                
                int n = showRunSelect();
                
                if(key == '*')continue;
                
                int* run = readPastRun( n - '1');
                //if(run[6] == 0xffff)continue; //empty
                
                while(1){
                    key = showRunMenu(n);

                         if(key == 'A') showRunTime(run);
                    else if(key == 'B') showRunStats(run);
                    else if(key == '*') break;
                }
                        
            }
            else if(key == 'B') return; //begin sorting process
            else if(key == '*') break; //go back
        }
    }
}


int prevTime = 0; //stores last second on RTC
int count = 0;

void showDateTime(){
    /*int* time = getTime();
    if(prevTime^time[0]){ //if seconds change update screen
        
        lcdClear();
        printf("  %02x:%02x:%02x      \n",time[2],time[1],time[0]);
        printf("  %02x/%02x/%02x A>",time[6],time[5],time[4]);
        
        prevTime = getTime()[0];
    }*/
    lcdClear();
    printf("  hh:mm:ss      \n");
    printf("  mm:dd:yy    A>");
    
    while(readKeypad() != 'A');
}

char showMainMenu(){
    lcdClear();
    printf("start   <B      \n");
    printf("history <C    *>");
    
    char k;
    do{k = readKeypad();} //proceed only if 
    while(k != 'B' && k != 'C' && k != '*');
    return k;
}

char showRunSelect(){
    lcdClear();
    printf("select run:     \n");
    printf("[1,2,3,4]     *>");
    
    char k;
    do{k = readKeypad();}
    while(k != '1' && k != '2' && k != '3' && k != '4' && k != '*');
    return k;
}

char showRunMenu(char key){
    lcdClear();
    printf("%c) time  <A     \n",key);
    printf( "   stats <B   *>");
    
    char k;
    do{k = readKeypad();}
    while(k != 'A' && k != 'B' && k != '*');
    return k;
}

char showRunTime(int* run){
    float time = (float)(run[0]<<8 + run[1])/10.0;
    
    lcdClear();
    printf("time: %.1f sec   \n",time);
    printf("              *>");
    
    while(readKeypad() != '*');
}

char showRunStats(int* run){
    lcdClear();
    printf("[AA,9V,C,other] \n");
    printf("[%d,%d,%d,%d]   *>",run[2],run[3],run[4],run[5]);
    
    while(readKeypad() != '*');
}