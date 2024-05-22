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


#include <xc.h>
#include <libpic30.h>
#include <stdio.h>
#include <math.h>

#define BIT_VALUE(val,no_bit) (val>>no_bit)&1


unsigned int seed = 1110011;
char current6 = 0, current7 = 0; //variables for buttons
int zad = 1;

int check_button(int zad){
    current6 = PORTDbits.RD6;
    current7 = PORTDbits.RD7;
    __delay32(1500000);

    if (current6 == 1){
        zad++;
        if(zad > 9){
            zad=1;
        }
    }

    if (current7 == 1){
        zad--;
        if(zad < 1){
            zad=9;
        }
    }
    return zad;
}

int IntToGray(unsigned char input){
    return (input >> 1) ^ input;
}

unsigned int generate_random(int val) {
    return BIT_VALUE(val,0)^BIT_VALUE(val,1)^BIT_VALUE(val,4)^BIT_VALUE(val,5);
}

int main(void) {
    // Port access
    TRISA = 0x0000;     // port set to output
    TRISD = 0xFFFF;     // port set to input

    unsigned portValue = 0x0000;



    while(1)
    {
        LATA = portValue;

        while(zad == 1) {
            portValue++;
            LATA = portValue;
            __delay32(1500000);
            zad = check_button(zad);

        }
        portValue = 0x0000;

        while (zad == 2) {
            portValue--;
            LATA = portValue;
            __delay32(1500000);
            zad = check_button(zad);
        }
        portValue = 0x0000;

        while (zad == 3){
            for (int i = 0; i <= 255; i++) {
                LATA = IntToGray(i);
                __delay32(1500000);
                zad = check_button(zad);
                if (zad != 3){
                    break;
                }
            }
        }
        portValue = 0x0000;

        while (zad == 4){
            for (int i = 255; i >= 0; i--) {
                LATA = IntToGray(i);
                __delay32(1500000);
                zad = check_button(zad);
                if (zad != 4){
                    break;
                }
            }
        }
        portValue = 0x0000;

        while (zad == 5){
            int dziesietna = 0;
            for (int i = 0; i <= 9; i++) {
                dziesietna = 16 * i;
                for (int j = 0; j <= 9; j++) {
                    LATA = dziesietna + j;
                    __delay32(1500000);
                    zad = check_button(zad);
                    if (zad != 5){
                        break;
                    }
                }
            }
        }
        portValue = 0x0000;

        while (zad == 6){
            int dziesietna = 0;
            for (int i = 9; i >= 0; i--) {
                dziesietna = 16 * i;
                for (int j = 9; j >= 0; j--) {
                    LATA = dziesietna + j;
                    __delay32(1500000);
                    zad = check_button(zad);
                    if (zad != 6){
                        break;
                    }
                }
            }
        }
        portValue = 0x0000;

        while (zad == 7) {
            int wezyk = 0;
            int strona = 1;//1: w prawo, 0: w lewo
            while (zad==7) {
                for (int i = wezyk; i < wezyk + 3; i++) {
                    portValue += pow(2, i);
                    __delay32(1500000);
                    zad = check_button(zad);
                    if (zad != 7){
                        break;
                    }
                }
                if (wezyk + 4 > 8) {
                    strona = 0;
                }
                if (wezyk - 1 < 0) {
                    strona = 1;
                }
                if (strona == 1) {
                    wezyk++;
                }
                if (strona == 0) {
                    wezyk--;
                }
                LATA = portValue;
                portValue = 0;
                __delay32(1500000);
            }
        }
        portValue = 0x0000;

        while (zad==8){
            unsigned kolejka = 0;
            unsigned j = 7;
            while(portValue<255){
                for(int i=0;i<=j;i++){
                    portValue = kolejka + pow(2,i);
                    LATA = portValue;
                    __delay32(1500000);
                    zad = check_button(zad);
                    if (zad != 8){
                        break;
                    }
                }
                kolejka += pow(2, j);
                j--;
            }
            portValue=0;
        }
        portValue = 0x0000;

        while (zad==9){
            int val = 1;
            while (zad==9){
                int x = generate_random(val);
                val = (val >> 1) | (x << 5);
                __delay32(1500000);
                LATA = val;
                zad = check_button(zad);
            }
        }
        portValue = 0x0000;
    }
    return 0;
}
