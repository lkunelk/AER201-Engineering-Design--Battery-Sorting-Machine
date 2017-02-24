/* 
 * File:   iopin.h
 * Author: Nam
 *
 * Created on February 20, 2017, 8:07 PM
 * 
 * analog pins:
 * an0 an1 an2 an3 an4 an5 an6 an7 an8 an9 an10 an11 an12
 * ra0 ra1 ra2 ra3 ra5 re0 re1 re2 rb2 rb3 rb1  rb4  rb0 
 * 
 * 10MHz oscillator
 */

#ifndef IOPIN_H
#define	IOPIN_H

#define A 0
#define B 1
#define C 2
#define D 3

#define OUTPUT 0
#define INPUT 1

#define HIGH 1
#define LOW 0

void digitalPinMode(int port, int pin, int type);
int digitalRead(int port, int pin);
void digitalWrite(int port, int pin, int val);
void analogPinMode(void);
long analogRead(int channel);

#endif	/* IOPIN_H */

