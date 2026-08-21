#include <LPC214x.h>
#include "buzzer.h"

#define BUZZER 4      // P0.4

/*----------------------------------
        Initialize Buzzer
----------------------------------*/
void Buzzer_Init(void)
{
    /* Configure P0.4 as GPIO */
    PINSEL0 &= ~(3 << (BUZZER * 2));   // Clears bits 8 and 9

    /* Configure P0.4 as Output */
    IODIR0 |= (1 << BUZZER);

    /* Initially OFF */
    IOCLR0 = (1 << BUZZER);
}

/*----------------------------------
        ON
----------------------------------*/
void Buzzer_ON(void)
{
    IOSET0 = (1 << BUZZER);
}

/*----------------------------------
        OFF
----------------------------------*/
void Buzzer_OFF(void)
{
    IOCLR0 = (1 << BUZZER);
}
