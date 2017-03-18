/* 
 * File:   interface.c
 * Author: Nam
 *
 * Created on March 11, 2017, 3:03 PM
 */

#include <xc.h>
#include <stdio.h>
#include "interface.h"
#include "lcd.h"
#include "keypad.h"
#include "RTC.h"


void showInterface(){
    
    //initRTC();
    //setTime();
    char key; //for storing keypad input
    while(1){
        
            dateTime();
            while(readKeypad() != 'A');
        
        while(1){
            //---------
            mainMenu();
            //---------
            
            do{key = readKeypad();} //proceed only if 
            while(key != 'B' && key != 'C' && key != '*');
                
            if(key == 'C'){ // history of runs
                //---------
                runSelect();
                //---------
                
                //read eeprom for past runs
                
                do{key = readKeypad();}
                while(key != '1' && key != '2' && key != '3' && key != '4' && key != '*');
                
                if(key == '*')continue;
                
                //check that there's something in memory
                
                while(1){ 
                    //---------
                    runMenu(key);
                    //---------

                    do{key = readKeypad();}
                    while(key != 'A' && key != 'B' && key != '*');

                    if(key == 'A'){ //how long the process ran
                        //---------
                        runTime();
                        //---------
                    }
                    else if(key == 'B'){ //how  many batteries for each category
                        //---------
                        runStats();
                        //---------
                    }
                    else if(key == '*')break;
                    
                    while(readKeypad() != '*');
                }
                        
            }
            else if(key == 'B'){ //start
                return; //begin the process
            }
            else if(key == '*')//go back
                break;
        }
    }
}


int prevTime = 0; //stores last second on RTC
int count = 0;
void dateTime(){
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
}

void mainMenu(){
    lcdClear();
    printf("start   <B      \n");
    printf("history <C    *>");
}

void runSelect(){
    lcdClear();
    printf("select run:     \n");
    printf("[1,2,3,4]     *>");
}

void runMenu(char key){
    lcdClear();
    printf("%c) time  <A     \n",key);
    printf( "   stats <B   *>");
}

void runTime(){
    lcdClear();
    printf("time: nnn sec   \n");
    printf("              *>");
}

void runStats(){
    lcdClear();
    printf("[AA,9V,C,other] \n");
    printf("[nn,n,n,nn]   *>");
}