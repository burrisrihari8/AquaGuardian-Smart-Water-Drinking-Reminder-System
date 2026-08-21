#include <LPC214x.h>
#include "led.h"

/*----------------------------------
    Initialize LEDs
-----------------------------------*/
void LED_Init(void)
{

    /* Configure as Output */
    IODIR0 |= (1<<YELLOW_LED) |
              (1<<GREEN_LED)  |
              (1<<RED_LED);

    /* All LEDs OFF (Active HIGH) */
    IOCLR0 = (1<<YELLOW_LED) |
             (1<<GREEN_LED)  |
             (1<<RED_LED);
}

/*----------------------------------
        Yellow LED
-----------------------------------*/
void YellowLED_ON(void)
{
    IOSET0 = (1<<YELLOW_LED);
}

void YellowLED_OFF(void)
{
    IOCLR0 = (1<<YELLOW_LED);
}

void YellowLED_Toggle(void)
{
    IOPIN0 ^= (1<<YELLOW_LED);
}

/*----------------------------------
        Green LED
-----------------------------------*/
void GreenLED_ON(void)
{
    IOSET0 = (1<<GREEN_LED);
}

void GreenLED_OFF(void)
{
    IOCLR0 = (1<<GREEN_LED);
}

/*----------------------------------
        Red LED
-----------------------------------*/
void RedLED_ON(void)
{
    IOSET0 = (1<<RED_LED);
}

void RedLED_OFF(void)
{
    IOCLR0 = (1<<RED_LED);
}
