#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;

    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}

#define BIT_VALUE(val,no_bit) (val>>no_bit)&1
int main()
{
    int IntToGray(unsigned char input)
    {
        return (input >> 1) ^ input;
    }

    int val = 10;
    for (int k=0;k<val;k++)
    {
        for(int i=7;i>=0;i--)
        {

            printf("%d", BIT_VALUE(k, i));
        }

        printf("\n");

        for(int i=7;i>=0;i--)
        {
            printf("%d", BIT_VALUE(IntToGray(k), i));
        }

        delay(1);
        system("cls");
    }

    return 0;
}
