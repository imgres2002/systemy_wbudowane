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

char button1 = 1, button2 = 1, button3 = 1;
int check_time_status(int *time_status){
    TRISA = 0b01000000;
    TRISD = 0xFFFF;

    button2 = PORTDbits.RD7;

    __delay32(150000);


    if(button2 == 0){
        *time_status++;;
    }
    if(*time_status > 2) {
        *time_status = 0;
    }

    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;
}

void check_game_status(int *game_status, int *current_player){
    TRISA = 0b01000000;
    TRISD = 0xFFFF;

    button1 = PORTDbits.RD6;
    __delay_ms(10);
    button2 = PORTDbits.RD7;
    __delay_ms(10);
    button3 = PORTAbits.RA7;
    __delay_ms(10);
    

    if(*current_player != 0 && *game_status == 0){
        if(button2 == 0){
            *game_status = 1;
        }
    }
    if (*current_player == 0 && *game_status == 0){
        if(button1 == 0){
            *current_player = 1;
            __delay_ms(100);
            *game_status = 1;
        }
        if(button3 == 0){
            *current_player = 2;
            __delay_ms(100);
            *game_status = 1;
        }
    }
    __delay_ms(10);
    
    if(*game_status == 1){
        if(button2 == 0){
            *game_status = 0;
        }
        if(button1 == 0){
            *current_player = 2;
        }
        if(button3 == 0){
            *current_player = 1;
        }
    }
    __delay_ms(10);;
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;
}

int set_time(int time){
    TRISA = 0x0000;
    TRISD = 0xFFFF;
    __delay_ms(10);
    button1 = PORTDbits.RD6;
    __delay_ms(10);
    button3 = PORTAbits.RA7;
    __delay_ms(10);
    if (button1 == 0) {
        time += 10;
    }
    if (button3 == 0) {
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


    int time_status = 0; // 0: nie ustawiaj, 1: ustawianie czasu dla gracza 1, 2: ustawianie czasu dla gracza 2   -je?li NIE jestemy w trakcie gry button2 to ustawiaj czas     -je?li jestemy w trakcie gry button2 to start/stop
    int current_player = 0; // 0: nie ustawiono gracza, 1: gracz 1, 2:gracz2
    int game_status = 0; // 0: zatrzymaj zegar, 1: wznów zegar

    LCD_init();

    unsigned int player1_time = 30; // Czas gracza 1 w sekundach
    unsigned int player2_time = 30; // Czas gracza 2 w sekundach

    char tekst[100];


    while(current_player == 0){
        __delay_ms(10);
        LCD_setCursor(1, 0);
        __delay_ms(10);
        sprintf(tekst, "Gracz 1: %02d:%02d", player1_time / 60, player1_time % 60);
        LCD_print(tekst);
        __delay_ms(10);

        LCD_setCursor(2, 0);
        __delay_ms(10);
        sprintf(tekst, "Gracz 2: %02d:%02d", player2_time / 60, player2_time % 60);
        LCD_print(tekst);
        __delay_ms(10);

        check_game_status(&game_status, &current_player);
        if(game_status == 1){
            LCD_sendCommand(LCD_CLEAR);
            LCD_setCursor(1, 0);
            LCD_print("zle dziala");
             __delay_ms(1000);
             __delay_ms(1000);
             __delay_ms(1000);
             __delay_ms(1000);
             __delay_ms(1000);
        }
        __delay_ms(10);
        check_time_status(&time_status);
        __delay_ms(10);
        if (time_status == 1){
            player1_time = set_time(player1_time);
            __delay_ms(10);
        }
        if (time_status == 2){
            player2_time = set_time(player2_time);
            __delay_ms(10);
        }
        __delay_ms(10);
        while(player1_time > 0 && player2_time > 0 && game_status == 1 && current_player != 0) {
            while (player1_time > 0 && current_player == 1) {
                __delay_ms(970);
                player1_time--;
                
                LCD_setCursor(1, 0);
                __delay_ms(10);
                sprintf(tekst, "Gracz 1: %02d:%02d", player1_time / 60, player1_time % 60);
                LCD_print(tekst);
                __delay_ms(10);

                check_game_status(&game_status, &current_player);
                __delay_ms(10);
            }


            while (player2_time > 0 && current_player == 2) {
                __delay_ms(970);
                player2_time--;
                
                LCD_setCursor(2, 0);
                __delay_ms(10);
                sprintf(tekst, "Gracz 2: %02d:%02d", player2_time / 60, player2_time % 60);
                LCD_print(tekst);
                __delay_ms(10);

                check_game_status(&game_status, &current_player);
                __delay_ms(10);
            }

            if (player2_time == 0 || player1_time == 0) {
                LCD_sendCommand(LCD_CLEAR);
                __delay_ms(10);
                if (player1_time == 0) {
                    LCD_setCursor(1, 0);
                    __delay_ms(10);
                    LCD_print("Gracz 1 przegral");
                    __delay_ms(1000);
                    __delay_ms(1000);
                    __delay_ms(1000);
                    __delay_ms(1000);
                    __delay_ms(1000);
                }
                if (player2_time == 0) {
                    LCD_setCursor(2, 0);
                    __delay_ms(10);
                    LCD_print("Gracz 2 przegral");
                    __delay_ms(1000);
                    __delay_ms(1000);
                    __delay_ms(1000);
                    __delay_ms(1000);
                    __delay_ms(1000);
                }
                break;
            }
        }
    }
    return 0;
}
