#ifndef LED_H
#define LED_H

#include "types.h"

/* Active HIGH LEDs */
#define YELLOW_LED 23
#define GREEN_LED  24
#define RED_LED    25

void LED_Init(void);

void YellowLED_ON(void);
void YellowLED_OFF(void);
void YellowLED_Toggle(void);

void GreenLED_ON(void);
void GreenLED_OFF(void);

void RedLED_ON(void);
void RedLED_OFF(void);

#endif
