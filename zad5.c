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

char button1 = 0, button2 = 0, button3 = 0;

// button operation
void check_button(int *game_status, int *current_player){
    __delay_ms(10);
    TRISAbits.TRISA7=1;
    __delay_ms(10);
    button3 = PORTAbits.RA7;

    __delay_ms(10);
    TRISA = 0x0000;
    __delay_ms(10);
    TRISD = 0xFFFF;
    __delay_ms(10);
    button1 = PORTDbits.RD6;
    __delay_ms(10);
    button2 = PORTDbits.RD7;
    __delay_ms(10);

    // start game
    if (*current_player == 0 && *game_status == 0){
        if(button3 == 0){
            *current_player = 2;
            *game_status = 1;
        }

        if(button1 == 0){
            *current_player = 1;
            *game_status = 1;
        }
    }
    // if game is started and time is stoped
    if(*current_player != 0 && *game_status == 0){
        __delay_ms(10);
        if(button2 == 0){
            __delay_ms(10);
            *game_status = 1;
        }
    }
    __delay_ms(10);

    // if game is started and time is started
    if(*current_player != 0 && *game_status == 1){
        // stop game
        if(button2 == 0){
            *game_status = 0;
        }
        // if current player is 1 change current player
        if(*current_player != 1 && button1 == 0){
            *current_player = 2;
        }
        // if current player is 2 change current player
        if(*current_player != 2 && button3 == 0){
            *current_player = 1;
        }
    }
    __delay_ms(10);
    TRISA = 0x0000;
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;
}
// set the time for the current player
int set_time(int time){
    __delay_ms(10);
    TRISAbits.TRISA7=1;
    __delay_ms(10);
    button3 = PORTAbits.RA7;

    __delay_ms(10);
    TRISA = 0x0000;
    __delay_ms(10);
    TRISD = 0xFFFF;
    __delay_ms(10);
    button1 = PORTDbits.RD6;
    __delay_ms(10);
    button2 = PORTDbits.RD7;
    __delay_ms(10);

// add time from the current player
    if (button3 == 0) {
        time += 10;
    }
// subtract time from the current player
    if (button1 == 0) {
        time -= 10;
    }
    __delay_ms(10);
    TRISA = 0x0000;
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;

    return time;
}

//void wait(int n, int game_status, int current_player){
//    for(int i = 0; i<n; i++){
//        __delay_ms(100);
//        check_button(game_status, current_player);
//    }
//}

//int check_time_mode(int *time_mode){
//    TRISA = 0x0000;
//    TRISD = 0xFFFF;
//
//    button2 = PORTDbits.RD7;
//
//    __delay_ms(10);
//
//
//    if(button2 == 0){
//        *time_mode = 1;
//    }
//    if(*time_mode > 2) {
//        *time_mode = 0;
//    }
//    TRISB = 0x7FFF;
//    TRISD = 0x0000;
//    TRISE = 0x0000;
//}

int main(void) {
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;

//    int time_mode = 0; // 0: nie ustawiaj, 1: ustawianie czasu dla gracza 1, 2: ustawianie czasu dla gracza 2   -je?li NIE jestemy w trakcie gry button2 to ustawiaj czas     -je?li jestemy w trakcie gry button2 to start/stop
    int current_player = 0; // 0: not set, 1: player1, 2: player2
    int game_status = 0; // 0: stop timer, 1: start timer

    LCD_init();

    unsigned int player1_time = 10; // player1 time in seconds
    unsigned int player2_time = 10; // player2 time in seconds

    char tekst[100];

    // set and show time while game is not started
    while(current_player == 0){
        LCD_setCursor(1, 0);
        __delay_ms(10);
        sprintf(tekst, "Gracz 1: %02d:%02d", player1_time / 60, player1_time % 60);
        LCD_print(tekst);

        LCD_setCursor(2, 0);
        __delay_ms(10);
        sprintf(tekst, "Gracz 2: %02d:%02d", player2_time / 60, player2_time % 60);
        LCD_print(tekst);

        check_button(&game_status, &current_player);
//        check_time_mode(&time_mode);
//        
//        if (time_mode == 0){
//            LCD_sendCommand(LCD_CLEAR);
//            __delay_ms(10);
//            LCD_print("time1");
//            
//        }
//
//        if (time_mode == 1){
//            LCD_sendCommand(LCD_CLEAR);
//            __delay_ms(10);
//            LCD_print("time1");
//            break;
//            player1_time = set_time(player1_time);
//            __delay_ms(10);
//        }
//        if (time_mode == 2){
//            LCD_sendCommand(LCD_CLEAR);
//            __delay_ms(10);
//            player2_time = set_time(player2_time);
//            __delay_ms(10);
//        }
        __delay_ms(10);
        // start game
        while(player1_time > 0 && player2_time > 0 && current_player != 0) {
            check_button(&game_status, &current_player);
            // show player1 time while his time in not over, hi is current player and game is started
            while (player1_time > 0 && current_player == 1 && game_status == 1) {
                __delay_ms(970);
//                wait(5, &game_status, &current_player);
                player1_time--;

                LCD_setCursor(1, 0);
                __delay_ms(10);
                sprintf(tekst, "Gracz 1: %02d:%02d", player1_time / 60, player1_time % 60);
                LCD_print(tekst);
                __delay_ms(10);

                check_button(&game_status, &current_player);
                __delay_ms(10);
            }
            // show player2 time while his time in not over, hi is current player and game is started
            while (player2_time > 0 && current_player == 2 && game_status == 1) {
                __delay_ms(970);
//                wait(5, &game_status, &current_player);
                player2_time--;

                LCD_setCursor(2, 0);
                __delay_ms(10);
                sprintf(tekst, "Gracz 2: %02d:%02d", player2_time / 60, player2_time % 60);
                LCD_print(tekst);
                __delay_ms(10);

                check_button(&game_status, &current_player);
                __delay_ms(10);
            }
            if (player2_time == 0 || player1_time == 0) {
                LCD_sendCommand(LCD_CLEAR);
                __delay_ms(10);
                LCD_setCursor(1, 0);
                __delay_ms(10);
                // show that player1 loss when his time is over
                if (player1_time == 0) {
                    LCD_print("Gracz 1 przegral");
                }
                // show that player2 loss when his time is over
                if (player2_time == 0) {
                    LCD_print("Gracz 2 przegral");
                }
                __delay_ms(1000);
                __delay_ms(1000);
                __delay_ms(1000);
                __delay_ms(1000);
                __delay_ms(1000);
                break;
            }
        }
    }
    return 0;
}
