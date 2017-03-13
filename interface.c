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

void showInterface(){
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

void dateTime(){
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