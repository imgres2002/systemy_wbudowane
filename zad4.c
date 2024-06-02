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

char button1 = 0, button2 = 0, potentiometer = 0;
void check_button(int *mode, int *state){
    TRISA = 0x0000;
    TRISD = 0xFFFF;
    __delay_ms(10);

    button1 = PORTDbits.RD6;
    __delay_ms(10);
    button2 = PORTDbits.RD7;
    __delay32(150000);

    if (button1 == 0){
        if (*mode == 0) {
            *mode = 1;
        } else {
            *mode = 0;
        }
    }
    __delay_ms(10);
    if (button2 == 0){
        if (*state == 0) {
            *state = 1;
        } else {
            *state = 0;
        }
    }
    __delay_ms(10);
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;
}

int check_potentiometer(int mode){
    TRISA = 0x0000;      //TRISA = 0b0000000000000000;
    TRISB = 0xFFFF;      //TRISB = 0b1111111111111111;
    AD1CON1 = 0x80E4;   //AD1CON1 = 0b1000000011100100;
    AD1CON2 = 0x0404;   //AD1CON2 = 0b0000010000000100;
    AD1CON3 = 0x0F00;   //AD1CON3 = 0b0000111100000000;
    AD1CHS = 0;
    AD1CSSL = 0x0020;   //AD1CSSL = 0b0000000000100000;

    __delay_ms(10);
    if(mode == 0){
//        float pom = ADC1BUF0/ 1024 * 300;
//        potentiometer = (int)pom;
        potentiometer = ADC1BUF0;
    }
    __delay_ms(10);
    if(mode == 1){
        potentiometer = ADC1BUF0/2;
    }
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;

    return potentiometer;
}

int main(void) {
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;

    int mode = 0; // 0: power, 1: time
    int state = 0; // 0: stop, 1: start

    LCD_init();

    int power = 100;
    int time = 60; // time in seconds
    char text[16];



    while (1) {
        check_button(&mode, &state);
        // set power
        if (mode == 0) {
            power = check_potentiometer(mode);
            __delay_ms(10);

            sprintf(text, "moc: %dw", power);

            LCD_sendCommand(LCD_CLEAR);
            __delay_ms(10);
            LCD_setCursor(2, 0);
            __delay_ms(10);
            LCD_print(text);
            __delay_ms(10);
        }
        // set time
        if (mode == 1) {
            time = check_potentiometer(mode);
            __delay_ms(10);

            sprintf(text, "czas: %02d:%02ds", time / 60, time % 60);

            LCD_sendCommand(LCD_CLEAR);
            __delay_ms(10);
            LCD_setCursor(1, 0);
            __delay_ms(10);
            LCD_print(text);
            __delay_ms(10);
        }
        // start the microwave timer
        if (state == 1) {
            LCD_sendCommand(LCD_CLEAR);
            __delay_ms(10);
            while (state == 1 && time > 0) {
                sprintf(text, "pozostolo: %02d:%02ds", time / 60, time % 60);

                LCD_setCursor(1, 0);
                __delay_ms(10);
                LCD_print(text);
                __delay_ms(980);

                time--;

                __delay_ms(10);
                check_button(&mode, &state);
            }
            // show "koniec" after user stop timer or time is over
            LCD_sendCommand(LCD_CLEAR);
            __delay_ms(10);
            LCD_setCursor(1, 0);
            __delay_ms(10);

            LCD_print("koniec");
            __delay_ms(1000);
            __delay_ms(1000);
            __delay_ms(1000);
            __delay_ms(1000);

            break;
        }
    }
    return 0;
}
