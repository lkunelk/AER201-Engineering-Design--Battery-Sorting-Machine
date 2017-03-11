/* 
 * File:   timer.h
 * Author: Nam
 *
 * Created on February 25, 2017, 12:25 PM
 */

#ifndef TIMER_H
#define	TIMER_H

void initTimer(int timer);
void startTimer(int timer, long val);
long getTimerCount(int timer);

float testFrequency();
void angleToPulseLength(float mill, int prescaler);

#endif	/* TIMER_H */

