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

#define FCY         4000000UL   // Częstotliwość robocza oscylatora jako połowa

#define LCD_E       LATDbits.LATD4
#define LCD_RW      LATDbits.LATD5
#define LCD_RS      LATBbits.LATB15
#define LCD_DATA    LATE

#define LCD_CLEAR   0x01    // Czyszczenie ekranu
#define LCD_HOME    0x02    // Powrót kursora do początku
#define LCD_ON      0x0C    // Włączenie wyświetlacza
#define LCD_OFF     0x08    // Wyłączenie wyświetlacza
#define LCD_CONFIG  0x38    // Konfiguracja wyświetlacza
#define LCD_CURSOR  0x80    // Ustawienie kursora
#define LINE1       0x00    // Adres początku pierwszej linii
#define LINE2       0x40    // Adres początku drugiej linii
#define LCD_CUST_CHAR   0x40    // Zapisanie niestandardowego znaku
#define LCD_SHIFT_R     0x1D    // Przesunięcie ekranu w prawo
#define LCD_SHIFT_L     0x1B    // Przesunięcie ekranu w lewo

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
    __delay_us(50);
    LCD_E = 0;
}

void LCD_sendData(unsigned char data){
    LCD_RW = 0;
    LCD_RS = 1;
    LCD_E = 1;
    LCD_DATA = data;
    __delay_us(50);
    LCD_E = 0;
}

void LCD_print(unsigned char* string){
    while(*string){
        LCD_sendData(*string++);
    }
}

void LCD_setCursor(unsigned char row, unsigned char col){
    unsigned char address;
    if (row == 1){
        address = LCD_CURSOR + LINE1 + col;
    }
    if (row == 2){
        address = LCD_CURSOR + LINE2 + col;
    }
    LCD_sendCommand(address);
}

void LCD_saveCustChar(unsigned char slot, unsigned char *array) {
    unsigned char i;
    LCD_sendCommand(LCD_CUST_CHAR + (slot*8));
    for(i=0;i<8;i++){
        LCD_sendData(array[i]);
    }
}

void LCD_init(){
    __delay_ms(20);
    LCD_sendCommand(LCD_CONFIG);
    __delay_us(50);
    LCD_sendCommand(LCD_ON);
    __delay_us(50);
    LCD_sendCommand(LCD_CLEAR);
    __delay_ms(2);
}

//void check_button(int *opcja, int *start){
//    prev6 = PORTDbits.RD6;
//    prev7 = PORTDbits.RD7;
//    __delay32(150000);
//    current6 = PORTDbits.RD6;
//    current7 = PORTDbits.RD7;
//    __delay32(150000);
//
//    if (current6 - prev6 == 1){
//        if (*opcja == 0) {
//            *opcja = 1;
//        } else {
//            *opcja = 0;
//        }
//    }
//
//    if (current7 - prev7 == 1){
//        if (*start == 0) {
//            *start = 1;
//        } else {
//            *start = 0;
//        }
//    }
//}

int main(void) {
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;
//    TRISA = 0x0000;     // port set to output
//    TRISD = 0xFFFF;     // port set to input

    char current6 = 0, prev6 = 0, current7 = 0, prev7 = 0;

    LCD_init();
    LCD_setCursor(2,0);

    unsigned int player1_time = 300; // Czas gracza 1 w sekundach (5 minut)
    unsigned int player2_time = 300; // Czas gracza 2 w sekundach (5 minut)

    char tekst[100];
    LCD_setCursor(1, 0);
    sprintf(tekst, "Gracz 1: %02d:%02d", player1_time / 60, player1_time % 60);
    LCD_print(tekst);

    LCD_setCursor(2, 0);
    sprintf(tekst, "Gracz 2: %02d:%02d", player2_time / 60, player2_time % 60);
    LCD_print(tekst);

    while(player1_time > 0 && player2_time > 0) {
        unsigned int current_player_time;

        while (player1_time > 0 || current6 - prev6 == 1) {
            __delay_ms(900);
            player1_time--;

            LCD_setCursor(1, 0);
            sprintf(tekst, "Gracz 1: %02d:%02d", player1_time / 60, player1_time % 60);
            LCD_print(tekst);

            prev6 = PORTDbits.RD6;
            current6 = PORTDbits.RD6;
            __delay_ms(100);
        }
        if (current_player_time == 0) {
            // Gracz 1 przegrał przez czas
            LCD_sendCommand(LCD_CLEAR);
            LCD_setCursor(2, 0);
            LCD_print("Gracz 1 przegral");
            break;
        }

        current_player_time = player2_time;
        while (player2_time > 0 || current7 - prev7 == 1) {
            __delay_ms(900);
            player2_time--;

            LCD_setCursor(2, 0);
            sprintf(tekst, "Gracz 2: %02d:%02d", player2_time / 60, player2_time % 60);
            LCD_print(tekst);

            prev7 = PORTDbits.RD7;
            current7 = PORTDbits.RD7;
            __delay_ms(100);
        }
        if (player2_time == 0) {
            // Gracz 2 przegrał przez czas
            LCD_sendCommand(LCD_CLEAR);
            LCD_setCursor(2, 0);
            LCD_print("Gracz 2 przegral");
            break;
        }
    }
    
    while (current7 - prev7 == 1){
        prev8 = PORTDbits.RD8;
        current8 = PORTDbits.RD8;
        __delay_ms(100);
    }

    return 0;
}
