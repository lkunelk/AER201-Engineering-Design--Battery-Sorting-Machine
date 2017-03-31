/* 
 * File:   eeprom.h
 * Author: Nam
 *
 * Created on February 24, 2017, 9:46 AM
 */

#ifndef EEPROM_H
#define	EEPROM_H

int* readPastRun(int run);
void saveNewRun(int* data);
void eepromWrite(int data, long address);
int eepromRead(long address);

#endif	/* EEPROM_H */

