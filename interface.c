/* 
 * File:   interface.c
 * Author: Nam
 *
 * Created on March 11, 2017, 3:03 PM
 */

#include <xc.h>
#include <stdio.h>
#include "configBits.h"
#include "eeprom.h"
#include "interface.h"
#include "lcd.h"
#include "keypad.h"
#include "RTC.h"


void showInterface(){
    
    initRTC();
    char key; //for storing keypad input
    while(1){
        showDateTime();
        
        while(1){
            key = showMainMenu();
                
            if(key == 'C'){ // history of runs
                
                int n = showRunSelect();
                
                if(key == '*')continue;
                
                int* run = readPastRun( n - '1');
                if(run[0] == 0xff){ // 0xff is default value of the memory
                    showErrorMessage();
                    continue;
                }
                
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
    
    initRTC();
    //setTime();
    while(keyPressed() != 'A'){
        int* t = getTime();
        lcdClear();
        int sec = t[0], min = t[1], hour = t[2];
        int day = t[4], mon = t[5], year = t[6];
        
        printf("  20%02x/%02x/%02x\n",year,mon,day);
        printf("   %02x:%02x:%02x   A>",hour,min,sec);
        __delay_ms(100);
    }
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
    int time = run[1];
    time/=10;
    
    lcdClear();
    printf("time: %d sec   \n",time);
    printf("              *>");
    
    while(readKeypad() != '*');
}

char showRunStats(int* run){
    int n_AA = run[2], n_9V = run[3], n_C = run[4], n_OTHER = run[5]; 
    
    lcdClear();
    printf("[AA,9V,C,other] \n");
    printf("[%d,%d,%d,%d]   *>",n_AA,n_9V,n_C,n_OTHER);
    
    while(readKeypad() != '*');
}

void showErrorMessage(){
    lcdClear();
    
    printf(" run not \n");
    printf("  available   *>");
    
    while(readKeypad() != '*');
}