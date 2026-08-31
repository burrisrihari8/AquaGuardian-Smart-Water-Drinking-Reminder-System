// switch.c

#include <LPC214x.h>
#include "types.h"
#include "switch.h"
#include "hydration.h"
#include "delay.h"
#include "reminder.h"
#include "display.h"

//--------------------------------------------------
// Drink Switch Pin
//--------------------------------------------------
#define DRINK_SW    2      // P0.2 (Active LOW)

//--------------------------------------------------
// Initialize Drink Switch
//--------------------------------------------------
void Switch_Init(void)
{
    /* Configure P0.2 as GPIO */
    PINSEL0 &= ~(3 << (DRINK_SW * 2));

    /* Configure P0.2 as Input */
    IODIR0 &= ~(1 << DRINK_SW);
}

//--------------------------------------------------
// Check Drink Switch
//--------------------------------------------------
u8 DrinkSwitchPressed(void)
{
    /* Active LOW */
    if((IOPIN0 & (1 << DRINK_SW)) == 0)
        return 1;

    return 0;
}

//--------------------------------------------------
// Drink Switch Task
//--------------------------------------------------
void DrinkSwitch_Task(void)
{
    if(DrinkSwitchPressed())
    {
        delay_ms(20);

        if(DrinkSwitchPressed())
        {
            if(IsReminderActive())
            {
                StopReminder();
            }
            else
            {
                DrinkWater();
								if(GoalCompleted)
								{
									DisplayGoalComplete();

									delay_ms(2000);

									DisplayStatus();
								}
            }

            while(DrinkSwitchPressed());

            delay_ms(20);
        }
    }
}
