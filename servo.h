/* 
 * File:   servo.h
 * Author: Nam
 *
 * Created on February 27, 2017, 1:44 PM
 */

#ifndef SERVO_H
#define	SERVO_H

void initServo(int timer, int po, int pi);
void servoInterruptService(long time);

#endif	/* SERVO_H */

