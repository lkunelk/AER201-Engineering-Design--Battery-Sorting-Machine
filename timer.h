/* 
 * File:   timer.h
 * Author: Nam
 *
 * Created on February 25, 2017, 12:25 PM
 */

#ifndef TIMER_H
#define	TIMER_H

void initT0(void);
void startT0(long val);
float testFrequency();
void angleToPulseLength(float mill, int prescaler);

long extFreq = 31500000; //in Hertz
#endif	/* TIMER_H */

