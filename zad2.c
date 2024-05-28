// CONFIG2
#pragma config POSCMOD = HS             // Primary Oscillator Select (HS Oscillator mode selected)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRIPLL           // Oscillator Select (Primary Oscillator with PLL module (HSPLL, ECPLL))
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)
// CONFIG1
#pragma config WDTPS = PS32768 // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128 // WDT Prescaler (1:128)
#pragma config WINDIS = ON // Watchdog Timer Window Mode disabled
#pragma config FWDTEN = OFF // Watchdog Timer disabled
#pragma config ICS = PGx2 // Emulator/debugger uses EMUC2/EMUD2
#pragma config GWRP = OFF // Writes to program memory allowed
#pragma config GCP = OFF // Code protection is disabled
#pragma config JTAGEN = OFF // JTAG port is disabled

#define FCY         4000000UL   // Cz?stotliwo?? robocza oscylatora jako po?owa

#include "xc.h"

void __delay_ms(unsigned long ms){
    __delay32(ms*FCY/1000);
}
char current6 = 0;
int check_button(int wlacz){
    current6 = PORTDbits.RD6;

    if (current6 == 1){
        if(wlacz == 1){
            wlacz = 0;
        } else {
            wlacz = 1;
        }
    }

    return wlacz;
}

unsigned portValue;

int main(void) {
    TRISA = 0x0000;      //TRISA = 0b0000000000000000;
    TRISB = 0xFFFF;      //TRISB = 0b1111111111111111;

    AD1CON1 = 0x80E4;   //AD1CON1 = 0b1000000011100100;
    AD1CON2 = 0x0404;   //AD1CON2 = 0b0000010000000100;
    AD1CON3 = 0x0F00;   //AD1CON3 = 0b0000111100000000;
    AD1CHS = 0;
    AD1CSSL = 0x0020;   //AD1CSSL = 0b0000000000100000;

    int potentiometer = 0;
    int wlacz = 1;


    while(1) {
        __delay32(1500000);
        potentiometer = ADC1BUF0;

        int time = 0;
        while(potentiometer > 512 && wlacz==1){
            while(time <= 5 && potentiometer > 512){
                potentiometer = ADC1BUF0;
                LATA = 0x0001;
                __delay_ms(1000);
                LATA = 0x0000;
                __delay_ms(1000);
                time++;
            }
            potentiometer = ADC1BUF0;
            __delay32(1500000);
            LATA = 0xffff;
            __delay32(1500000);
            wlacz = check_button(wlacz);
            __delay32(1500000);
        }
        LATA = 0x0000;
    }
    return 0;
}
