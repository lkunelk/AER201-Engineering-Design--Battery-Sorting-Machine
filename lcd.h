/* 
 * File:   lcd.h
 * Author: Nam
 *
 * Created on February 20, 2017, 12:52 PM
 */

#ifndef LCD_H
#define	LCD_H

void lcdInst(char data);
void lcdNibble(char data);
void initLCD(void);
void lcdClear(void);
void lcdNewline(void);

#endif	/* LCD_H */