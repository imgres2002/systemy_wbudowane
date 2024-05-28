// CONFIG2
#pragma config POSCMOD = NONE             // Primary Oscillator Select (primary oscillator disabled)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = FRC              // Oscillator Select (Fast RC Oscillator without Postscaler)
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
#include "xc.h"
#include <libpic30.h>
#include <stdio.h>

#define FCY 8000000UL
#define LCD_E       LATDbits.LATD4
#define LCD_RW      LATDbits.LATD5
#define LCD_RS      LATBbits.LATB15
#define LCD_DATA    LATE

#define LCD_CLEAR   0x01
#define LCD_HOME    0x02
#define LCD_ON      0x0C
#define LCD_OFF     0x08
#define LCD_CONFIG  0x38
#define LCD_CURSOR  0x80
#define LINE1       0x00
#define LINE2       0x40

void __delay_us(unsigned long us){
    __delay32(us*FCY/1000000);
}

void __delay_ms(unsigned long ms){
    __delay32(ms*FCY/1000);
}

void LCD_sendCommand(unsigned char command){
    LCD_RW = 0;
    LCD_RS = 0;
    LCD_E = 1;
    LCD_DATA = command;
    __delay_us(60);
    LCD_E = 0;

}

void LCD_sendData(unsigned char data){
    LCD_RW = 0;
    LCD_RS = 1;
    LCD_E = 1;
    LCD_DATA = data;
    __delay_us(60);
    LCD_E = 0;

}

void LCD_setCursor(unsigned char row, unsigned char col){
    unsigned char address;
    if(row==1){
        address = LCD_CURSOR + LINE1 + col;
    }
    if(row==2){
        address = LCD_CURSOR + LINE2 + col;
    }
    LCD_sendCommand(address);
}

void LCD_print(char* string) {
    while(*string){
        LCD_sendData(*string++);
    }
}

void LCD_init(){
    __delay_us(20);
    LCD_sendCommand(LCD_CONFIG);
    __delay_us(50);
    LCD_sendCommand(LCD_ON);
    __delay_us(50);
    LCD_sendCommand(LCD_CLEAR);
    __delay_ms(2);
}
char button1 = 0, button2 = 0, potentiometer = 0;
void check_button(int *opcja, int *start){
    TRISA = 0x0000;
    TRISD = 0xFFFF;

    button1 = PORTDbits.RD6;
    button2 = PORTDbits.RD7;
    __delay32(150000);

    if (button1 == 1){
        if (*opcja == 0) {
            *opcja = 1;
        } else {
            *opcja = 0;
        }
    }

    if (button2 == 1){
        if (*start == 0) {
            *start = 1;
        } else {
            *start = 0;
        }
    }
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;
}

int check_potentiometer(int opcja){
    TRISA = 0x0000;      //TRISA = 0b0000000000000000;
    TRISB = 0xFFFF;      //TRISB = 0b1111111111111111;

    AD1CON1 = 0x80E4;   //AD1CON1 = 0b1000000011100100;
    AD1CON2 = 0x0404;   //AD1CON2 = 0b0000010000000100;
    AD1CON3 = 0x0F00;   //AD1CON3 = 0b0000111100000000;
    AD1CHS = 0;
    AD1CSSL = 0x0020;   //AD1CSSL = 0b0000000000100000;

    int val = 0;
    int potentiometer_normalized = ADC1BUF0/1024;
    if(opcja == 0){
        val = potentiometer_normalized * 300;
    }
    if(opcja == 1){
        val = potentiometer_normalized * 600;
    }
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;
    return val;
}

int main(void) {
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;

    int opcja = 1; // 0: moc, 1: czas
    int start = 0; // 0: stop, 1: start

    LCD_init();

    int waty = 100;
    int czas = 60; // w sekundach
    char tekst[100];



    while (1) {
        check_button(&opcja, &start);

        if (opcja == 0) {
            waty = check_potentiometer(opcja);

            sprintf(tekst, "moc: %dw", waty);
            LCD_setCursor(2, 0);
            __delay_ms(100);
            LCD_print(tekst);
            __delay_ms(100);
        }
        if (opcja == 1) {
            czas = check_potentiometer(opcja);

            sprintf(tekst, "czas: %02d:%02ds", czas / 60, czas % 60);
            LCD_setCursor(1, 0);
            __delay_ms(100);
            LCD_print(tekst);
            __delay_ms(100);
        }
        if (start == 1) {
            LCD_sendCommand(LCD_CLEAR);
            __delay_ms(100);
            while (start == 1 && czas > 0) {
                LCD_setCursor(1, 0);
                sprintf(tekst, "pozosto: %02d:%02ds", czas / 60, czas % 60);
                LCD_print(tekst);
                __delay_ms(9900);
                czas--;
                check_button(&opcja, &start);
            }
            break;
        }
    }
    return 0;
}
