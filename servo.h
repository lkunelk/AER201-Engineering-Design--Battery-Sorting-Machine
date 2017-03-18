/* 
 * File:   servo.h
 * Author: Nam
 *
 * Created on February 27, 2017, 1:44 PM
 */

#ifndef SERVO_H
#define	SERVO_H

void initServo(int* motor, int angle);
void setAngle(int* motor, int angle);
void servoInterruptService();
void stopServo(int timer);
long angleToPulse(int angle);

#endif	/* SERVO_H */

