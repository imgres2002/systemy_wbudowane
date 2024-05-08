//#pragma config POSCMOD = NONE             // Primary Oscillator Select (primary oscillator disabled)
//#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
//#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
//#pragma config FNOSC = FRC              // Oscillator Select (Fast RC Oscillator without Postscaler)
//#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)
//// CONFIG1
//#pragma config WDTPS = PS32768 // Watchdog Timer Postscaler (1:32,768)
//#pragma config FWPSA = PR128 // WDT Prescaler (1:128)
//#pragma config WINDIS = ON // Watchdog Timer Window Mode disabled
//#pragma config FWDTEN = OFF // Watchdog Timer disabled
//#pragma config ICS = PGx2 // Emulator/debugger uses EMUC2/EMUD2
//#pragma config GWRP = OFF // Writes to program memory allowed
//#pragma config GCP = OFF // Code protection is disabled
//#pragma config JTAGEN = OFF // JTAG port is disabled
//#include "xc.h"
//#include <libpic30.h>
//
//#define FCY 4000000UL
//#define LCD_E       LATDbits.LATD4
//#define LCD_RW      LATDbits.LATD5
//#define LCD_RS      LATBbits.LATB15
//#define LCD_DATA    LATE
//
//#define LCD_CLEAR   0x01
//#define LCD_HOME    0x02
//#define LCD_ON      0x0C
//#define LCD_OFF     0x08
//#define LCD_CONFIG  0x38
//#define LCD_CURSOR  0x80
//#define LINE1       0x00
//#define LINE2       0x40
//
//void __delay_us(unsigned long us){
//    __delay32(us*FCY/1000000);
//}
//
//void __delay_ms(unsigned long ms){
//    __delay32(ms*FCY/1000);
//}
//
//void LCD_sendCommand(unsigned char command){
//    LCD_RW = 0;
//    LCD_RS = 0;
//    LCD_E = 1;
//    LCD_DATA = command;
//    __delay_us(60);
//    LCD_E = 0;
//    
//}
//
//void LCD_sendData(unsigned char data){
//    LCD_RW = 0;
//    LCD_RS = 1;
//    LCD_E = 1;
//    LCD_DATA = data;
//    __delay_us(60);
//    LCD_E = 0;
//    
//}
//
//void LCD_setCursor(unsigned char row, unsigned char col){
//    unsigned char address;
//    if(row==1){
//        address = LCD_CURSOR + LINE1 + col;
//    }
//    if(row==2){
//        address = LCD_CURSOR + LINE2 + col;
//    }
//    LCD_sendCommand(address);
//}
//
//void LCD_print(char* string) {
//    while(*string){
//        LCD_sendData(*string++);
//    }
//}
//
//void LCD_init(){
//    __delay_us(20);
//    LCD_sendCommand(LCD_CONFIG);
//    __delay_us(50);
//    LCD_sendCommand(LCD_ON);
//    __delay_us(50);
//    LCD_sendCommand(LCD_CLEAR);
//    __delay_ms(2);
//}

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
int main(void) {
    unsigned portValue = 0x0001;
    char current6 = 0, prev6 = 0, current7 = 0, prev7 = 0; //variables for buttons

    // Port access
    TRISA = 0x0000;     // port set to output
    TRISD = 0xFFFF;     // port set to input

    while(1)
    {
        LATA = portValue;
        prev6 = PORTDbits.RD6;      //scanning for a change of buttons' state
        prev7 = PORTDbits.RD7;
        __delay32(150000);
        current6 = PORTDbits.RD6;
        current7 = PORTDbits.RD7;

        if (current6 - prev6 == 1) //button up
        {
            char current6 = 0, prev6 = 0, current7 = 0, prev7 = 0; //variables for buttons
            while(current6==prev6 && current7 == prev7){
                portValue++;
                LATA = portValue;
                __delay32(150000);
                prev6 = PORTDbits.RD6;      //scanning for a change of buttons' state
                prev7 = PORTDbits.RD7;
                __delay32(150000);
                current6 = PORTDbits.RD6;
                current7 = PORTDbits.RD7;
            }
            
        }

        if (current7 - prev7 == 1)  //button down
        {
            portValue--;
        }

    }

    return 0;
}

//int main(void) {
//    unsigned portValue = 0x0001;
//    char current6 = 0, prev6 = 0, current7 = 0, prev7 = 0; //variables for buttons
//    int zad = 0;
//
//    // Port access
//    TRISA = 0x0000;     // port set to output
//    TRISD = 0xFFFF;     // port set to input
//    
//    while(1)
//    {
//        
//        LATA = portValue;
//        prev6 = PORTDbits.RD6;      //scanning for a change of buttons' state
//        prev7 = PORTDbits.RD7;
//        __delay32(150000);
//        current6 = PORTDbits.RD6;
//        current7 = PORTDbits.RD7;
//        if (current6 - prev6 == 1) //button up
//        {   
//            zad++;
//            if(zad > 9){
//                zad=0;
//            }
//            
//        }
//        if (current6 - prev6 == 1) //button up
//        {
//            zad--;
//            if(zad < 0){
//                zad=9;
//            }
//        }
//        if (zad == 1) {
//            while(1){
//                portValue++;
//                LATA = portValue;
//            }
//        }
//        
//        if (zad == 2) {
//            while(current6==prev6 && current7 == prev7){
//                portValue--;
//            }
//        }
//    }
//
//    return 0;
//}
