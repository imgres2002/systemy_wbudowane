/*
 * File:   wyswietlacz.c
 * Author: local
 *
 * Created on April 10, 2024, 11:49 AM
 */
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

#define FCY 4000000UL
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

void check_button(int *opcja, int *start){
    prev6 = PORTDbits.RD6;
    prev7 = PORTDbits.RD7;
    __delay32(150000);
    current6 = PORTDbits.RD6;
    current7 = PORTDbits.RD7;
    __delay32(150000);

    if (current6 - prev6 == 1){
        if (*opcja == 0) {
            *opcja = 1;
        } else {
            *opcja = 0;
        }
    }

    if (current7 - prev7 == 1){
        if (*start == 0) {
            *start = 1;
        } else {
            *start = 0;
        }
    }
}

int main(void) {
    TRISD = 0x0000;
    TRISB = 0X7FFF;
    TRISE = 0X0000;
    int opcja = 0; // 0: moc, 1: czas
    int start = 0; // 0: stop, 1: start

    char current6 = 0, prev6 = 0, current7 = 0, prev7 = 0;

    LCD_init();
    LCD_setCursor(1, 1);
    int waty = 100;
    int czas = 60;// w sekundach
    char tekst_moc[20];
    char tekst_czas[20];
    char pom[20];

    sprintf(tekst_moc, "moc: %dw", waty);
    LCD_setCursor(0, 1);
    LCD_print(tekst_moc);

    __delay_ms(100);

    sprintf(tekst_czas, "czas: %ds", waty);
    LCD_setCursor(1, 1);
    LCD_print(tekst_czas);

    while (1)
        check_button(&opcja, &start);
        if (opcja == 0) {
            sprintf(tekst_moc, "moc: %dw", waty);
            sprintf(pom, "%s  <---", tekst_moc);
            LCD_setCursor(0, 1);
            LCD_print(pom);
            __delay_ms(1000);
            LCD_print(tekst_moc);
        } else {
            sprintf(tekst_czas, "czas: %ds", czas);
            sprintf(pom, "%s  <---", tekst_czas);
            LCD_setCursor(1, 1);
            LCD_print(pom);
            __delay_ms(1000);
            LCD_print(tekst_czas);
        }
        if (start == 1) {
            LCD_sendCommand(LCD_CLEAR);
            __delay_ms(100);
            char pozostaly_czas[20];
            while (start == 1) {
                LCD_setCursor(1, 1);
                sprintf(pozostaly_czas, "pozostaly czas: %ds", czas);
                LCD_print(pozostaly_czas);
                __delay_ms(9900);
                czas--;
                check_button(&opcja, &start);
            }
            czas = 60;
        }
    return 0;
}
