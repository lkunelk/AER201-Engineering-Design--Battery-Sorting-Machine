/*
 * File:   main.c
 * Author: Nam Nguyen
 * Created on Feb 18, 2017, 12:11 PM
 *
 * Main procedure for the Battery Sorting Machine
 * 
 * Goal of Machine:
 * - given 15 batteries or less (type: AA, 9V, C) seperate the batteries
 * autonomously into 4 different categories: Charged AA, Charged 9V, Charged C,
 * Discharged and place each category into a separate bin.
 * 
 * Structure of physical machine:
 * 
 * rotating cylinder
 * ---------------------------------------------------------
 * batteries are placed into a cylinder which picks up batteries
 * and drops them onto a conveyor belt running in the middle of cylinder. 
 * The cylinder is run by a DC motor
 * 
 * conveyor belt
 * -------------------------------------------------------------
 * transports batteries outside of cylinder. 
 * Uses a continuous servo motor
 * 
 * testing channel
 * -----------------------------------------------------------
 * the batteries then drop into a testing v-shaped channel.
 * The battery triggers a contact switch as it falls into place. The testing channel
 * consists of 2 voltage measuring pads, which are embedded with metal contacts at
 * different locations. This helps to differentiate between batteries depending 
 * on which contacts detect a voltage. One pad is static while the other can rotate
 * to clamp or release the battery. The dynamic pad is controlled using a servo.
 * 
 * redirecting arm
 * -----------------------------------------------------------
 * below the testing channel is the redirecting arm. It points towards the 4
 * different bins. Depending on what type the testing channel detected the arm
 * will point to the correct bin. The dynamic pad above will release the battery
 * which will fall onto the redirecting arm and make its way to the bin. The 
 * directing arm is controlled using a servo motor.
 * 
 * 
 */

#include <xc.h>
#include <stdio.h>
#include "configBits.h"
#include "keypad.h"
#include "lcd.h"
#include "iopin.h"
#include "eeprom.h"
#include "I2C.h"
#include "timer.h"
#include "servo.h"
#include "interface.h"

void pinSetup(void);
void sortBattery(void);
void pause(char* message);

long time = 0; //duration of sorting process [deci-seconds] (10 = 1 second)
int batteryDetected = 0; //1 == contact switch activated on machine
int terminate = 0; //flag to let program know if we neeed to terminate
int stoppingTime = 10000; //[centi-seconds]

int n_AA;
int n_C;
int n_9V;
int n_OTHER;

int    redirectAngle_AA = 138; //angles for the re-directing arm
int     redirectAngle_C = 166;
int    redirectAngle_9V = 118;
int redirectAngle_OTHER = 153;

int   padAngle_CLOSE = 70; //angles for voltage testing pads
int padAngle_NEUTRAL = 130;
int   padAngle_OPEN  = 180;

//pins
//RTC uses C3 and C4
int     cylinderMotor[2] = {C, 0}; //port C, pin 0
int       cylinderDir[2] = {D, 1};  //port D, pin 1
int     conveyorServo[2] = {C, 2}; //port C, pin 1
int          padServo[2] = {C, 1}; //port C, pin 2
int  redirectingServo[2] = {D, 0}; //port D, pin 0

//pins for measuring voltage and determining battery type
int AA_float[2] = {C,5}; //pin for helping differentiate AA from 9V, for some case
int  padPin1[2] = {C,6}; //digital port A, pin 1
int  padPin2[2] = {C,7}; //digital port A, pin 2
int  padPin3[2] = {A,0}; //analog 0 is the channel not the pin 
                     //(in this case channel 0 is pin 0),
                     //port value not needed only for reference

float V_LIM_AA = .198; //Voltage limits (above => charged, below => discharged)
float V_LIM_C  = 1.3254;
float V_LIM_9V = 1.863;

void debug();
void debug(){
    
    pinSetup();
    initLCD();
    
    int i = 0;
    while(1){
        lcdClear();
        printf("%d, %02x",i,eepromRead(i));
        i++;
        readKeypad();
    }
    
    while(1);
}

void main(){
    
    //debug();
    
    pinSetup();
    initLCD();
    
    while(1){
        showInterface();
        
        
        //start timer
        terminate = 0;
        time = 0; //reset time
        initTimer(1);
        startTimer(1,0);
        
        //start up motors
        digitalWrite(cylinderMotor, HIGH);
        initServo(conveyorServo,    130);
        initServo(padServo,         padAngle_NEUTRAL);
        initServo(redirectingServo, redirectAngle_AA);
        
        //spin cylinder and conveyor and if battery is detected sort it
        while(1){
            lcdClear();
            printf("running");
            
            int lastBatteryTime = time;// store when the last battery was seen
            
            int cylinderStart = time;
            int cylinderDur = 0;
            int cylinderForward = 70; //deci-seconds
            int cylinderBackward = 30; //deci-seconds
            
            while(!batteryDetected && !terminate){ //update screen and poll time
                //print time
                lcdHome();
                printf("time %02ld:%02ld",time/600,(time/10)%60);
                __delay_ms(77);
                
                //change direction of the cylinder motor
//                if(time - cylinderStart >= cylinderDur)
//                {
//                    int prev = digitalRead(cylinderDir);
//                    digitalWrite(cylinderDir, !prev);
//                    if(prev)cylinderDur = cylinderBackward;
//                    else    cylinderDur = cylinderForward;
//                    cylinderStart = time;
//                }
                
                if(time - lastBatteryTime > stoppingTime)
                {
                    terminate = 1;
                    break;
                }            
            }
            
            if(terminate)break; //exit the sorting process
            
            sortBattery();
            
            //reset flag after sorting
            lastBatteryTime = time;
            batteryDetected = 0;
        }
        
        //stop motor
        digitalWrite(cylinderMotor, LOW);
        
        //display results
        int run[7];
        run[0] = time>>8;
        run[1] = time;
        run[2] = n_AA;
        run[3] = n_9V;
        run[4] = n_C;
        run[5] = n_OTHER;
        
        saveNewRun(run);
        showRunTime(run);
        showRunStats(run);
        
    };//stop here
    
    
    while(1); //stop here
}

void sortBattery(){
    
    //stop cylinder and conveyor belt
    setAngle(conveyorServo, 90);
    digitalWrite(cylinderMotor, LOW);
    
    //wait for battery to fall in
    __delay_ms(1000);
    
    float Vcc = 5.00;
    float resolution = (1<<10) - 1;
    float V_max = 0;
    float V_float = 0;
    int pos_v_counter = 1;
    int signal = 0;
    
    //compress battery and measure voltage
    //pause("interrupt!!!\nclose?");
    for(float i = (float)padAngle_NEUTRAL; i > padAngle_CLOSE; i -= 0.1) {
        setAngle(padServo, i);
        __delay_us(100);
        digitalWrite(AA_float, HIGH); //set floating pin before reading
        float V = analogRead(padPin3[1]) / resolution * Vcc;
        
        if(V > 0.1)pos_v_counter++;
        
        if(V > V_max) {
            signal  = digitalRead(padPin1)<<1; //combine 2 reads
            signal |= digitalRead(padPin2)<<0;
            
            V_max = V;
            
            //read voltage when pin is floating
            digitalWrite(AA_float, LOW); //float pin to ground to differentiate AA from 9V
            __delay_ms(1); //let voltage changes occur
            V_float = analogRead(padPin3[1]) / resolution * Vcc; //voltage read
            
        }
    }
    
    int targetAngle;
    float V = V_max;
    
    lcdClear();
    printf("V: %.3f, N: %d\n",V,pos_v_counter);
//  __delay_ms(1000);
    
    //set the angle for directing arm
    //pause("set redirect angle?");
    switch(signal){
        case 0b00:
            if(V_float < 0.1){ //if below then it's AA battery
                if(V > V_LIM_AA){ targetAngle = redirectAngle_AA; n_AA+=1;}
                else            { targetAngle = redirectAngle_OTHER; n_OTHER+=1;}
                printf("AA lim: %f",V_LIM_AA);
                break;
            }
            //else it's a 9V, fall through from case 0
        case 0b10:
            if(V > V_LIM_9V) {targetAngle = redirectAngle_9V; n_9V+=1;}
            else             {targetAngle = redirectAngle_OTHER; n_OTHER+=1;}
            printf("9V lim: %f",V_LIM_9V);
            break;
        case 0b01:
            if(V > V_LIM_C) {targetAngle = redirectAngle_C; n_C+=1;}
            else            {targetAngle = redirectAngle_OTHER; n_OTHER+=1;}
            printf("C  lim: %f",V_LIM_C);
            break;
    }
    
    
    readKeypad();
    setAngle(redirectingServo, targetAngle);
    __delay_ms(500);
    
    //release battery
    //pause("release battery?");
    for(int i = padAngle_CLOSE; i < padAngle_OPEN; i++) {
        setAngle(padServo, i);
        __delay_ms(3);
    }
//    setAngle(padServo, padAngle_OPEN);
    __delay_ms(1000);
    
    //set gate to the resting state
    //pause("reset the pad?");
    setAngle(padServo, padAngle_NEUTRAL);
    
    //turn on the conveyor belt and cylinder
    //pause("conveyor & \ncylinder on?");
    setAngle(conveyorServo, 130);
    digitalWrite(cylinderMotor, HIGH);
}

void pinSetup(){
    //**OSCILLATOR**//
    //OSCCON = 0xF0; // Force internal oscillator operation at 8 MHz (pg. 7-39)
    //OSCTUNEbits.PLLEN = 1; // set  OSCTUNE<6> to enable PLL (4x  phase-lock-loop) thereby setting internal oscillator frequency to 32 MHz
    
    //set direction of pins
    TRISA = 0xFF; // Set Port A as all input
    TRISB = 0xFF; // input for keypads
    TRISC = 0b11000000;// set 7 6 as inputs the rest as outputs
    TRISD = 0x00; //All output mode for LCD
    TRISE = 0x00;  
    
    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;
    
    ADCON0 = 0x00; //turn off AD module for now
    //ADCON1 |= 0x10; //set an3 (pin RA3) as V+ voltage reference
    ADCON1 |= 0x0B; //set channel an0-an3 as analog 
                    //(map of channels (an#) to pins in iopin.c))
    //ADCON1 |= 0b11<<4; //use external voltage reference 
    //(an2 -> Vref-) and (an3 -> Vref+)
    
    ADCON2 = 0; //clear before setting
    ADCON2 |= 0b010; //set Tad = 1/ (Fosc/32), (must be Tas>=0.7us)
    ADCON2 |= 0b110<<3; //set conversion rate  16 * Tosc per bit
    ADCON2 |= 1<<7; //set right justified result
    
    //interrupts
    INT1IE = 1; // external interrupt for keypad
    INT0IE = 1; // external interrupt on B0 for battery sensing switch
    ei();
}

//24400 for 10ms
//30650 for 12.5ms

int angle = 90;
void interrupt service(void) {
    
    if( servoInterruptService() )return; //if interrupt was dealt with here return
    
    // 0.1 second timer
    // 12.5ms = 31100, 12.5ms * 8 = 100ms = 0.1s (prescaler = 8)
    // 0xffff - 31100 = 0x8683
    if(TMR1IF){
        startTimer(1,0x8683);
        time+=1;
        TMR1IF = 0; // clear flag
        return;
    }
    
    //Contact sensor - port B, pin 0 external interrupt
    if(INT0IF){ INT0IF = 0; //clear flag
        batteryDetected = 1;
        return;
    }
    
    //Keyboard - port B, pin 1 external interrupt
    if(INT1IF && INT1IE){INT1IF = 0;     //Clear flag bit
        keyPressedInterruptService();
        char key = (PORTB & 0xF0) >> 4; //read the keypress
        if(key == 12)terminate = 1;
//        if(key == 0)angle+=5;
//        if(key == 1)angle-=5;
//        setAngle(padServo, angle);
//        
//        lcdClear();
//        printf("angle: %d",angle);
        return;
    }
}

void pause(char* message){
    lcdClear();
    printf(message);
    readKeypad();
}