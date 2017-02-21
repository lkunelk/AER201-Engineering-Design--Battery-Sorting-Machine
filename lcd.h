/* 
 * File:   lcd.h
 * Author: Nam
 *
 * Created on February 20, 2017, 12:52 PM
 */

#ifndef LCD_H
#define	LCD_H

#define RS          LATDbits.LATD2          
#define E           LATDbits.LATD3
#define	LCD_PORT    LATD   //On LATD[4,7] to be specific
#define LCD_DELAY   600

void lcdInst(char data);
void lcdNibble(char data);
void initLCD(void);
void lcdClear(void);
void lcdNewline(void);

#endif	/* LCD_H */