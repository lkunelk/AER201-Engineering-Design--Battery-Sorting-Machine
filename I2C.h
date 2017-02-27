/* 
 * File:   I2C.h
 * Author: Nam
 *
 * Created on February 25, 2017, 4:15 PM
 */

#ifndef I2C_H
#define	I2C_H

void I2C_Master_Init(const unsigned long c);
void I2C_Master_Write(unsigned d);
unsigned char I2C_Master_Read(unsigned char a);

#endif	/* I2C_H */

