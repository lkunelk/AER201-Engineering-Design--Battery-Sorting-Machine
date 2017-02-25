/* 
 * File:   timer.c
 * Author: Nam Nguyen
 *
 * Created on February 25, 2017, 12:25 PM
 * 
 * pic18 has 4 timer modules
 * timer0 
 * - can be configured to be 8or16 bit timer/counter
 * - 8 bit mode set T0CON<6> (6th bit)
 * - has 3 registers:
 *  - T0CON (control register)
 *  - TMR0L (lower data)
 *  - TMR0H (higher data)
 * timer0 operates as timer or counter depending on clock source
 * timer - internal instruction cycle clock
 * counter - RA4/T0CLK pin (set T0CON<5> to get counter mode)
 * - to start counter/timer set T0CON<7>
 * 
 * prescaler (clear T0CON<3> to assign prescaler)
 * - can increase number of pulses per increment of counter/timer in pow of 2
 * - max 256, min 2
 * - prescaler value determined by T0CON<2:0> -> 111 is 1/256 and 000 is 1/2
 * 
 * TMR0H is buffered version of real high bit
 * writing to it stores the value in a buffer
 * only when TMR0L is set TMR0H is set simultaneously
 * 
 * TMR0 interrupt generated when TMR0 register overflows 
 * from 0xff t0 0x00 (8bit)
 * from 0xffff to 0x0000 (16bit)
 * 
 * interrup is set by setting INTCON<5>
 * overflow sets flag INTCON<2>
 */

