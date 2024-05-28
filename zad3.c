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
#include <string.h>

// Defninicja makr tak by kod byl czytelny, przejrzysty, deskryptywny i przyjazny
// uzytkownikowi
#define FCY         4000000UL   // czestotliwosc robocza oscylatora jako polowa
//czestotliwosci (FNOSC = FRC -> FCY = 4000000)
// Zdefiniowanie poszczegolnych pinow jako odpowiednie makra
#define LCD_E       LATDbits.LATD4
#define LCD_RW      LATDbits.LATD5
#define LCD_RS      LATBbits.LATB15
#define LCD_DATA    LATE

// Przypisanie wartosci poszcegolnych komend do wlasciwych makr
#define LCD_CLEAR   0x01    //0b00000001
#define LCD_HOME    0x02    //0b00000010
#define LCD_ON      0x0C    //0b00001100
#define LCD_OFF     0x08    //0b00001000
#define LCD_CONFIG  0x38    //0b00111000
#define LCD_CURSOR      0x80    //0b10000000
#define LINE1           0x00
#define LINE2           0x40
#define LCD_CUST_CHAR   0x40    //0b01000000
#define LCD_SHIFT_R     0x1D    //0b00011100
#define LCD_SHIFT_L     0x1B    //0b00011000

// Definicja funkcji delay w us i ms - operujacych na jednostkach czasu zamiast
// cykli pracy oscylatora

void __delay_us(unsigned long us){
    __delay32(us*FCY/1000000);
}

void __delay_ms(unsigned long ms){
    __delay32(ms*FCY/1000);
}

// Definicja funkcji wysylajacych komendy (RS = 0) i dane (RS = 1) za pomoca
// magistrali rownoleglej (LCD_DATA). Znaki i komendy maja 8 bitow!

void LCD_sendCommand(unsigned char command){
    LCD_RW = 0;     // Zapis
    LCD_RS = 0;     // Przesylanie komend
    LCD_E = 1;      // Otwarcie transmisji danych
    LCD_DATA = command;
    __delay_us(50); // Opoznienie konieczne dla zapisania danych.
    LCD_E = 0;      // Konieczne zablokowanie transmisji po przeslaniu komunikatu.
}

void LCD_sendData(unsigned char data){
    LCD_RW = 0;
    LCD_RS = 1;     // Przesylanie danych
    LCD_E = 1;
    LCD_DATA = data;
    __delay_us(50);
    LCD_E = 0;
}

// Funkcja print wyswietlajaca kolejne 8-bitowe znaki w petli while - * oznacza
// przypisanie nie wartosci zmiennej lecz jej adresu.

void LCD_print(unsigned char* string){
    while(*string){
        LCD_sendData(*string++);
    }
}

// Funkcja ustawiaj?ca kursor w wybranym miejscu

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

// Funkcja ZAPISUJACA znak (zmienna array) do PAMIECI CGRAM na wybranym slocie
// od 0 do 7

void LCD_saveCustChar(unsigned char slot, unsigned char *array) {
    unsigned char i;
    LCD_sendCommand(LCD_CUST_CHAR + (slot*8));
    for(i=0;i<8;i++){
        LCD_sendData(array[i]);
    }
}

// Funkcja inicjalizujaca wyswietlacz LCD. Wysyla niezbedne komendy jak LCD_CONFIG
// i LCD_ON

void LCD_init(){
    __delay_ms(20);
    LCD_sendCommand(LCD_CONFIG);
    __delay_us(50);     // opoznienia wynikaja ze specyfikacji wyswietlacza i czasu
    // przetwarzania poszczegolnych komend
    LCD_sendCommand(LCD_ON);
    __delay_us(50);
    LCD_sendCommand(LCD_CLEAR);
    __delay_ms(2);
}

// Zdefiniowanie znaku niestandardowego w postaci tablicy 8x5 (8 linii po 5 kropek)

unsigned char coin1[8] = {
        0b00111,
        0b01000,
        0b10111,
        0b10100,
        0b10100,
        0b10100,
        0b01000,
        0b00111
};

unsigned char coin2[8] = {
        0b11100,
        0b00010,
        0b00001,
        0b00001,
        0b00001,
        0b00001,
        0b00010,
        0b11100
};

unsigned char coin3[8] = {
        0b00011,
        0b00100,
        0b01011,
        0b01010,
        0b01010,
        0b01010,
        0b00100,
        0b00011
};

unsigned char coin4[8] = {
        0b11000,
        0b00100,
        0b00010,
        0b00010,
        0b00010,
        0b00010,
        0b00100,
        0b11000
};

unsigned char coin5[8] = {
        0b00001,
        0b00011,
        0b00011,
        0b00011,
        0b00011,
        0b00011,
        0b00011,
        0b00001
};

unsigned char coin6[8] = {
        0b10000,
        0b11000,
        0b11000,
        0b11000,
        0b11000,
        0b11000,
        0b11000,
        0b10000
};

unsigned char coin7[8] = {
        0b00001,
        0b00001,
        0b00001,
        0b00001,
        0b00001,
        0b00001,
        0b00001,
        0b00001
};

unsigned char coin8[8] = {
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b10000
};
char current6 = 0, current7 = 0; //variables for buttons
int ilosc_trybow = 4;
int check_button(int tryb){
    current6 = PORTDbits.RD6;
    current7 = PORTDbits.RD7;
    __delay32(1500000);

    if (current6 == 1){
        tryb++;
        if(tryb > ilosc_trybow){
            tryb=1;
        }
    }

    if (tryb == 1){
        tryb--;
        if(tryb < 1){
            tryb=ilosc_trybow;
        }
    }
    return tryb;
}

int main(void) {
//    TRISA = 0x0000;     // port set to output
//    TRISD = 0xFFFF;     // port set to input
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;

    int tryb = 4;

    char ad_text[] = "pomidor2zl";

    LCD_init();                     // Inicjalizacja wyswietlacza
    LCD_saveCustChar(0, coin1);
    LCD_saveCustChar(1, coin2);
    LCD_saveCustChar(2, coin3);
    LCD_saveCustChar(3, coin4);
    LCD_saveCustChar(4, coin5);
    LCD_saveCustChar(5, coin6);
    LCD_saveCustChar(6, coin7);
    LCD_saveCustChar(7, coin8);
    LCD_setCursor(1,1);             // Ustawienie kursora na poczatku drugiej linii
    LCD_print(ad_text);      // Wyswietlenie napisu
    __delay_ms(500);

    while (1){
//        miganie
        while(tryb == 0) {
            LCD_setCursor(1,0);             // Ustawienie kursora na poczatku drugiej linii
            LCD_print(ad_text);
            __delay_ms(1000);
            LCD_sendCommand(LCD_CLEAR);
            __delay_ms(1000);
            tryb = check_button(tryb);
        }
        // przewijanie w pionie
        while(tryb == 1) {
            LCD_setCursor(1,0);             // Ustawienie kursora na poczatku drugiej linii
            LCD_print(ad_text);
            __delay_ms(500);
            LCD_sendCommand(LCD_CLEAR);
            LCD_setCursor(2,0);             // Ustawienie kursora na poczatku drugiej linii
            LCD_print(ad_text);
            __delay_ms(500);
            tryb = check_button(tryb);
        }
        // przewijanie w prawo
        while(tryb == 2) {
            LCD_sendCommand(LCD_CLEAR);
            int length = strlen(ad_text);
            LCD_setCursor(1,-length);
            int  column_first_latter = -length;
            while (column_first_latter < length + 12){
                LCD_print(ad_text);
                __delay_ms(500);
                tryb = check_button(tryb);
                LCD_sendCommand(LCD_SHIFT_R);
                column_first_latter++;
            }
        }
        // przewijanie w lewo
        while(tryb == 3) {
            LCD_sendCommand(LCD_CLEAR);
            int length = strlen(ad_text);
            LCD_setCursor(1,0);
            int  column_first_latter = length + 12;
            while (column_first_latter > -length){
                LCD_print(ad_text);
                __delay_ms(500);
                tryb = check_button(tryb);
                LCD_sendCommand(LCD_SHIFT_L);
                column_first_latter--;
            }
        }
//        ze znakiem specjalnym
        while(tryb == 4) {
            LCD_sendCommand(LCD_CLEAR);
            __delay_ms(500);
            int length = strlen(ad_text);
            __delay_ms(500);
            LCD_setCursor(1,1);
            __delay_ms(500);
            LCD_print(ad_text);
            __delay_ms(500);
            int symbol_number = 0;
            while (symbol_number<=6){
                LCD_setCursor(1,length+1);
                LCD_sendData(symbol_number);
                LCD_setCursor(1,length+2);
                LCD_sendData(symbol_number+1);
                symbol_number+=2;
                __delay_ms(500);
                tryb = check_button(tryb);
            }
            symbol_number = 7;
            while (symbol_number>=0){
                LCD_setCursor(1,length+2);
                LCD_sendData(symbol_number);
                LCD_setCursor(1,length+1);
                LCD_sendData(symbol_number-1);
                symbol_number-=2;
                __delay_ms(500);
                tryb = check_button(tryb);
            }
        }
    }
    return 0;
}
