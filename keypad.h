/* 
 * File:   keypad.h
 * Author: Nam
 *
 * Created on February 20, 2017, 12:55 PM
 */

#ifndef KEYPAD_H
#define	KEYPAD_H

void initKeypad();
char readKeypad();
char keyPressed();
void keyPressedInterruptService();


#endif	/* KEYPAD_H */

