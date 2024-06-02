// CONFIG2
#pragma config POSCMOD = HS      // Primary Oscillator Select (HS Oscillator mode selected)
#pragma config OSCIOFNC = OFF    // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD    // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRIPLL    // Oscillator Select (Primary Oscillator with PLL module (HSPLL, ECPLL))
#pragma config IESO = OFF        // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)

// CONFIG1
#pragma config WDTPS = PS32768   // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128     // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON       // Watchdog Timer Window (Standard Watchdog Timer enabled, Windowed-mode is disabled)
#pragma config FWDTEN = OFF      // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx2        // Comm Channel Select (Emulator/debugger uses EMUC2/EMUD2)
#pragma config GWRP = OFF        // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF         // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF      // JTAG Port Enable (JTAG port is disabled)

#include <xc.h>
#include <libpic30.h>
#include <stdio.h>
#include <math.h>

#define BIT_VALUE(val, no_bit) ((val >> no_bit) & 1)

char button1 = 0, button2 = 0; // Variables for buttons

int check_button(int task) {
    button1 = PORTDbits.RD6;
    button2 = PORTDbits.RD7;
    __delay32(1500000);

    if (button1 == 1) {
        task++;
        if (task > 9) {
            task = 1;
        }
    }

    if (button2 == 1) {
        task--;
        if (task < 1) {
            task = 9;
        }
    }

    return task;
}

int IntToGray(unsigned char input) {
    return (input >> 1) ^ input;
}

unsigned int generate_random(int val) {
    return BIT_VALUE(val, 0) ^ BIT_VALUE(val, 1) ^ BIT_VALUE(val, 4) ^ BIT_VALUE(val, 5);
}

int main(void) {
    // Port access
    TRISA = 0x0000;     // Port set to output
    TRISD = 0xFFFF;     // Port set to input

    unsigned portValue = 0x0000;
    int task = 1;

    while (1) {
        LATA = portValue;

        while (task == 1) {
            portValue++;
            LATA = portValue;
            __delay32(1500000);
            task = check_button(task);
        }
        portValue = 0x0000;

        while (task == 2) {
            portValue--;
            LATA = portValue;
            __delay32(1500000);
            task = check_button(task);
        }
        portValue = 0x0000;

        while (task == 3) {
            for (int i = 0; i <= 255; i++) {
                LATA = IntToGray(i);
                __delay32(1500000);
                task = check_button(task);
                if (task != 3) {
                    break;
                }
            }
        }
        portValue = 0x0000;

        while (task == 4) {
            for (int i = 255; i >= 0; i--) {
                LATA = IntToGray(i);
                __delay32(1500000);
                task = check_button(task);
                if (task != 4) {
                    break;
                }
            }
        }
        portValue = 0x0000;

        while (task == 5) {
            // Set the first 4 bits
            for (int tens = 0; tens <= 9; tens++) {
                // Set the next 4 bits
                for (int units = 0; units <= 9; units++) {
                    LATA = 16 * tens + units;
                    __delay32(1500000);
                    task = check_button(task);
                    if (task != 5) {
                        break;
                    }
                }
            }
        }
        portValue = 0x0000;

        while (task == 6) {
            // Set the first 4 bits
            for (int tens = 9; tens >= 0; tens--) {
                // Set the next 4 bits
                for (int units = 9; units >= 0; units--) {
                    LATA = 16 * tens + units;
                    __delay32(1500000);
                    task = check_button(task);
                    if (task != 6) {
                        break;
                    }
                }
            }
        }
        portValue = 0x0000;

        while (task == 7) {
            int snake_body = 0;
            int direction = 1; // 1: right, 0: left

            while (task == 7) {
                // create sneak body
                for (int i = snake_body; i < snake_body + 3; i++) {
                    portValue += pow(2, i);
                    __delay32(1500000);
                    task = check_button(task);
                    if (task != 7) {
                        break;
                    }
                }
                // change movement direction if snake body go out of scope
                if (snake_body + 4 > 8) {
                    direction = 0;
                }
                // change movement direction if snake body go out of scope
                if (snake_body - 1 < 0) {
                    direction = 1;
                }
                // check movement direction
                if (direction == 1) {
                    // move sneak body right
                    snake_body++;
                } else {
                    // move sneak body left
                    snake_body--;
                }

                LATA = portValue;
                portValue = 0;
                __delay32(1500000);
            }
        }
        portValue = 0x0000;

        while (task == 8) {
            unsigned queue = 0;
            unsigned tail = 7;

            while (portValue < 255) {
                // goes through all the unlit diodes until you reach the tail
                for (int i = 0; i <= tail; i++) {
                    // set head element
                    portValue = queue + pow(2, i);
                    LATA = portValue;
                    __delay32(1500000);
                    task = check_button(task);
                    if (task != 8) {
                        break;
                    }
                }
                // set tail element
                queue += pow(2, tail);
                tail--;
            }
            portValue = 0x0000;
        }
        portValue = 0x0000;

        while (task == 9) {
            int val = 1;
            while (task == 9) {
                int random_number = generate_random(val);
                val = (val >> 1) | (random_number << 5);
                __delay32(1500000);
                LATA = val;
                task = check_button(task);
            }
        }
        portValue = 0x0000;
    }
    return 0;
}
