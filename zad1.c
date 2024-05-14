int check_button(unsigned zad){
    prev6 = PORTDbits.RD6;      //scanning for a change of buttons' state
    prev7 = PORTDbits.RD7;
    __delay32(150000);
    current6 = PORTDbits.RD6;
    current7 = PORTDbits.RD7;

    if (current6 - prev6 == 1){
        zad++;
        if(zad > 9){
            zad=0;
        }
    }

    if (current7 - prev7 == 1){
        zad--;
        if(zad < 1){
            zad=9;
        }
    }
    return zad;
}

int IntToGray(unsigned char input){
    return (input >> 1) ^ input;
}

unsigned int generate_random() {
    seed ^= (seed << 13);
    seed ^= (seed >> 17);
    seed ^= (seed << 5);
    return seed & 0x3F;
}

int main(void) {
    unsigned int seed = 1110011;
    unsigned portValue = 0x0000;
    char current6 = 0, prev6 = 0, current7 = 0, prev7 = 0; //variables for buttons
    unsigned zad = 1;

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

        while(zad == 1) {
            portValue++;
            LATA = portValue;
            __delay32(1500000);
            zad = check_button(zad);
        }

        while (zad == 2) {
            portValue++;
            LATA = portValue;
            __delay32(1500000);
            zad = check_button(zad);
        }

        while (zad == 3){
            for (int i = 0; i <= 255; i++) {
                LATA = IntToGray(i);
                __delay32(1500000);
                zad = check_button(zad);
            }
        }

        while (zad == 4){
            for (int i = 255; i >= 0; i--) {
                LATA = IntToGray(i);
                __delay32(1500000);
                zad = check_button(zad);
            }
        }

        while (zad == 5){
            int dziesietna = 0;
            for (int i = 0; i <= 9; i++) {
                dziesietna = 16 * i;
                for (int j = 0; j <= 9; j++) {
                    LATA = dziesietna + j;
                    __delay32(1500000);
                    zad = check_button(zad);
                }
            }
        }

        while (zad == 6){
            int dziesietna = 0;
            for (int i = 0; i >= 0; i--) {
                dziesietna = 16 * i;
                for (int j = 9; j >= 0; j--) {
                    LATA = dziesietna + j;
                    __delay32(1500000);
                    zad = check_button(zad);
                }
            }
        }

        while (zad == 7) {
            int wezyk = 0;
            int strona = 1;//1: w prawo, 0: w lewo
            while (zad==7) {
                for (int i = wezyk; i < wezyk + 3; i++) {
                    portValue += pow(2, i);
                    zad = check_button(zad);
                }
                if (wezyk + 4 > 8) {
                    strona = 0;
                }
                if (wezyk - 1 < 0) {
                    strona = 1;
                }
                if (strona == 1) {
                    wezyk++;
                }
                if (strona == 0) {
                    wezyk--;
                }
                portValue = 0;
                LATA = portValue;
                __delay32(1500000);
            }
        }

        while (zad==8){
            unsigned kolejka = 0;
            unsigned j = 7;
            while(portValue<255){
                for(int i=0;i<=j;i++){
                    portValue = kolejka + pow(2,i);
                    LATA = portValue;
                    __delay32(1500000);
                    zad = check_button(zad);
                }
                kolejka += pow(2, j);
                j--;
            }
            portValue=0;
        }

        while (zad==9){
            portValue = generate_random();
            LATA = portValue;
            __delay32(1500000);
            zad = check_button(zad);
        }

        portValue=0;
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