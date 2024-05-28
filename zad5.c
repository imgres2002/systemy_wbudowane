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

#define FCY         8000000UL   // Częstotliwość robocza oscylatora jako połowa

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

int check_time_status(int *time_status){
    TRISA = 0x0000;
    TRISD = 0xFFFF;

    button2 = PORTDbits.RD7;

    __delay32(150000);

    time_status++;
    if(*time_status > 2) {
        *time_status = 0;
    }

    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;
}

int check_game_status(int *game_status, int *current_player){
    TRISA = 0x0000;
    TRISD = 0xFFFF;

    button1 = PORTDbits.RD6;
    button2 = PORTDbits.RD6;
    button3 = PORTDbits.RD8;
    __delay32(150000);

    if(*current_player == 0 || *game_status == 0){
        if(*current_player != 0){
            if(button2 == 1){
                *game_status = 1;
            }
        } else {
            if(button1 == 1){
                *game_status = 1;
                *current_player = 2;
            }
            if(button3 == 1){
                *game_status = 1;
                *current_player = 1;
            }
        }
    }
    if(*game_status == 1){
        if(button2 == 1){
            *game_status = 0;
        }
    }
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;
}

int set_time(int time){
    TRISA = 0x0000;
    TRISD = 0xFFFF;
    button1 = PORTDbits.RD6;
    button3 = PORTDbits.RD8;
    if (button1 == 1) {
        time += 10;
    }
    if (button3 == 1) {
        time -= 10;
    }
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;

    return time;
}

int main(void) {
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;


    int time_status = 0; // 0: nie ustawiaj, 1: ustawianie czasu dla gracza 1, 2: ustawianie czasu dla gracza 2   -jeśli NIE jestemy w trakcie gry button2 to ustawiaj czas     -jeśli jestemy w trakcie gry button2 to start/stop
    int current_player = 0; // 0: nie ustawiono gracza, 1: gracz 1, 2:gracz2
    int game_status = 0; // 0: zatrzymaj zegar, 1: wznów zegar

    LCD_init();

    unsigned int player1_time = 30; // Czas gracza 1 w sekundach
    unsigned int player2_time = 30; // Czas gracza 2 w sekundach

    char tekst[100];


    while(current_player == 0){
        LCD_setCursor(1, 0);
        sprintf(tekst, "Gracz 1: %02d:%02d", player1_time / 60, player1_time % 60);
        LCD_print(tekst);

        LCD_setCursor(2, 0);
        sprintf(tekst, "Gracz 2: %02d:%02d", player2_time / 60, player2_time % 60);
        LCD_print(tekst);

        check_game_status(&game_status, &current_player);
        check_time_status(&time_status);

        if (time_status == 1){
            player1_time = set_time(player1_time);
        }
        if (time_status == 2){
            player2_time = set_time(player2_time);
        }

        while(player1_time > 0 && player2_time > 0 && game_status == 1 && current_player != 0) {
            while (player1_time > 0 && current_player == 1) {
                __delay_ms(900);
                player1_time--;

                LCD_setCursor(1, 0);
                sprintf(tekst, "%02d:%02d", player1_time / 60, player1_time % 60);
                LCD_print(tekst);

                __delay_ms(100);
                check_game_status(&game_status, &current_player);
            }


            while (player2_time > 0 && current_player == 2) {
                __delay_ms(900);
                player2_time--;

                LCD_setCursor(2, 0);
                sprintf(tekst, "%02d:%02d", player2_time / 60, player2_time % 60);
                LCD_print(tekst);

                check_game_status(&game_status, &current_player);
                __delay_ms(100);
            }

            if (player2_time == 0 || player1_time == 0) {
//                LCD_sendCommand(LCD_CLEAR);
                if (player1_time == 0) {
                    LCD_setCursor(1, 0);
                    LCD_print("Gracz 1 przegral");
                }
                if (player2_time == 0) {
                    LCD_setCursor(2, 0);
                    LCD_print("Gracz 2 przegral");
                }
                break;
            }
        }
    }
    return 0;
}
