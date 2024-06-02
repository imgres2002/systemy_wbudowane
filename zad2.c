// CONFIG2
#pragma config POSCMOD = HS        // Primary Oscillator Select (HS Oscillator mode selected)
#pragma config OSCIOFNC = OFF      // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD      // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRIPLL      // Oscillator Select (Primary Oscillator with PLL module (HSPLL, ECPLL))
#pragma config IESO = OFF          // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)

// CONFIG1
#pragma config WDTPS = PS32768     // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128       // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON         // Watchdog Timer Window Mode (Windowed-mode is disabled)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx2          // Comm Channel Select (Emulator/debugger uses EMUC2/EMUD2)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)

#define FCY 8000000UL              // Working frequency of the oscillator as half

#include "xc.h"

void __delay_ms(unsigned long ms) {
    __delay32(ms * FCY / 1000);
}

char button1 = 0;

int check_button(int state) {
    button1 = PORTDbits.RD6;

    if (button1 == 0) {
        if (state == 1) {
            state = 0;
        } else {
            state = 1;
        }
    }

    return state;
}

unsigned portValue;

int main(void) {
    TRISA = 0x0000;      // Set PORTA as output
    TRISB = 0xFFFF;      // Set PORTB as input

    AD1CON1 = 0x80E4;    // Configure ADC
    AD1CON2 = 0x0404;
    AD1CON3 = 0x0F00;
    AD1CHS = 0;
    AD1CSSL = 0x0020;

    int potentiometer = 0;
    int state = 1;       // 1: alarm turn on, 0: alarm turn off

    while (1) {
        __delay32(1500000);
        potentiometer = ADC1BUF0;
        state = check_button(state);

        int time = 0;
        while (potentiometer > 512 && state == 1) {
            // 1 LED flashing for 5 seconds
            while (time <= 5 && potentiometer > 512) {
                LATA = 0x0001;
                __delay_ms(1000);
                LATA = 0x0000;
                __delay_ms(1000);

                potentiometer = ADC1BUF0;

                time++;
            }
            potentiometer = ADC1BUF0;
            __delay32(1500000);

            LATA = 0xFFFF;
            __delay32(1500000);

            state = check_button(state);
            __delay32(1500000);
        }
        LATA = 0x0000;
    }
    return 0;
}
