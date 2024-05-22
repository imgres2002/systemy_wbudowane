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

#define FCY         4000000UL   // Cz?stotliwo?? robocza oscylatora jako po?owa

#define LCD_E       LATDbits.LATD4
#define LCD_RW      LATDbits.LATD5
#define LCD_RS      LATBbits.LATB15
#define LCD_DATA    LATE

#define LCD_CLEAR   0x01    // Czyszczenie ekranu
#define LCD_HOME    0x02    // Powrót kursora do pocz?tku
#define LCD_ON      0x0C    // W??czenie wy?wietlacza
#define LCD_OFF     0x08    // Wy??czenie wy?wietlacza
#define LCD_CONFIG  0x38    // Konfiguracja wy?wietlacza
#define LCD_CURSOR  0x80    // Ustawienie kursora
#define LINE1       0x00    // Adres pocz?tku pierwszej linii
#define LINE2       0x40    // Adres pocz?tku drugiej linii
#define LCD_CUST_CHAR   0x40    // Zapisanie niestandardowego znaku
#define LCD_SHIFT_R     0x1D    // Przesuni?cie ekranu w prawo
#define LCD_SHIFT_L     0x1B    // Przesuni?cie ekranu w lewo

int max_time = 600; // maksymalny czas w sekundach

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
char button1 = 0, button2 = 0, button3 = 0;
int check_button(int start){
    button1 = PORTDbits.RD6;
    button2 = PORTDbits.RD7;
    __delay32(150000);

    if (button2 == 1){
        if (*start == 0) {
            *start = 1;
        } else {
            *start = 0;
        }
    }
    return start;
}

int set_time(int time){
    button1 = PORTDbits.RD6;
    button2 = PORTDbits.RD7;
    if (button1 == 1) {
        time += 10;
    }
    if (button1 == 1) {
        time -= 10;
    }
    return time;
}

int main(void) {
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;
//    TRISA = 0x0000;     // port set to output
//    TRISD = 0xFFFF;     // port set to input

    
    int ustaw_czas = 0; // 0: nie ustawiaj, 1: ustawianie czasu dla gracza 1, 2: ustawianie czasu dla gracza 2
    int start = 0; // 0: stop, 1: start

    LCD_init();
    LCD_setCursor(2,0);

    unsigned int player1_time = 30; // Czas gracza 1 w sekundach
    unsigned int player2_time = 30; // Czas gracza 2 w sekundach

    char tekst[100];


    while(1){
        LCD_setCursor(1, 0);
        sprintf(tekst, "Gracz 1: %02d:%02d", player1_time / 60, player1_time % 60);
        LCD_print(tekst);

        LCD_setCursor(2, 0);
        sprintf(tekst, "Gracz 2: %02d:%02d", player2_time / 60, player2_time % 60);
        LCD_print(tekst);
        if (button3 == 1) {
            ustaw_czas++;
            if (ustaw_czas > 2){
                ustaw_czas = 0;
            }
        }
        if (ustaw_czas == 1){
            player1_time = set_time(player1_time);
        }
        if (ustaw_czas == 2){
            player2_time = set_time(player2_time);
        }
        if (button1 == 1 || button2 == 1){
            start = 1;
        }
        while(player1_time > 0 && player2_time > 0 &&  start == 1) {

            while (player1_time > 0 && button1 == 0) {
                __delay_ms(900);
                player1_time--;

                LCD_setCursor(1, 0);
                sprintf(tekst, "Gracz 1: %02d:%02d", player1_time / 60, player1_time % 60);
                LCD_print(tekst);

                __delay_ms(100);
                button1 = PORTDbits.RD6;

            }

            if (player1_time == 0) {
                // Gracz 1 przegra? przez czas
                LCD_sendCommand(LCD_CLEAR);
                LCD_setCursor(2, 0);
                LCD_print("Gracz 1 przegral");
                break;
            }

            while (player2_time > 0 && button2 == 0) {
                __delay_ms(900);
                player2_time--;

                LCD_setCursor(2, 0);
                sprintf(tekst, "Gracz 2: %02d:%02d", player2_time / 60, player2_time % 60);
                LCD_print(tekst);

                button1 = PORTDbits.RD7;
                button2 = PORTDbits.RD7;
                __delay_ms(100);
            }

            if (player2_time == 0) {
                // Gracz 2 przegra? przez czas
                LCD_sendCommand(LCD_CLEAR);
                LCD_setCursor(2, 0);
                LCD_print("Gracz 2 przegral");
                break;
            }

        }
    }

    return 0;
}
