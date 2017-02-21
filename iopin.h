/* 
 * File:   iopin.h
 * Author: Nam
 *
 * Created on February 20, 2017, 8:07 PM
 */

#ifndef IOPIN_H
#define	IOPIN_H

#define A 0
#define B 1
#define C 2
#define D 3

#define OUTPUT 0
#define INPUT 1

void pinMode(int port, int pin, int type);
int digitalRead(void);
int digitalWrite(void);

#endif	/* IOPIN_H */

