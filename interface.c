/* 
 * File:   interface.c
 * Author: Nam
 *
 * Created on March 11, 2017, 3:03 PM
 */

#include <xc.h>
#include <stdio.h>
#include "lcd.h"

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