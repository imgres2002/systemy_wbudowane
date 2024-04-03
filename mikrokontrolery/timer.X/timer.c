/*
 * File:   timer.c
 * Author: local
 *
 * Created on 3 kwietnia 2024, 11:46
 */
// CONFIG2
#pragma config POSCMOD = HS             // Primary Oscillator Select (HS Oscillator mode selected)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRIPLL           // Oscillator Select (Primary Oscillator with PLL module (HSPLL, ECPLL))
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON              // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator/debugger uses EMUC2/EMUD2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

#include "xc.h"
#include <libpic30.h>
unsigned portValue = 0;
char currentS6 = 0, prevS6 = 0, currentS7 = 0, prevS7 = 0, program = 0;

void __attribute__((interrupt, autp_psv)) _T1Interrupt(void){
    if (program == 0){
        portValue++;
    }
    if(program == 1){
        portValue--;
    }
    
    LATA = portValue;
    _T1IF = 0;
}


int main(void) {
    TRISA = 0X0000;
    TRISD = 0xFFFF;
    
    
    T1CON = 0X8010;
    
    PR1 = 0xFFFF;
    _T1IE = 1;
    _T1IP = 1;
    while(1){
        prevS6 = PORTDbits.RD6;
        prevS7 = PORTDbits.RD7;
        __delay32(1024);
        currentS6 = PORTDbits.RD6;
        currentS7 = PORTDbits.RD7;
        if(currentS6-prevS6 == -1){
            program = 1;
        }
        if(currentS7-prevS7 == -1){
            program = 0;
        }
    }
    return 0;
}
