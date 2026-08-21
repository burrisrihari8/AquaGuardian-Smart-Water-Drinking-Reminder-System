#include <LPC214x.h>

#include "types.h"
#include "lcd.h"
#include "lcd_defines.h"
#include "rtc.h"
#include "hydration.h"
#include "led.h"
#include "reminder.h"

//--------------------------------------------------
// Global Variables
//--------------------------------------------------

u32 Goal       = 8;
u32 Consumed   = 0;
u32 Remaining  = 8;
u32 Percentage = 0;
u8 GoalCompleted = 0;

//--------------------------------------------------
// Initialize Hydration Values
//--------------------------------------------------

void Hydration_Init(void)
{
    Goal          = 8;
    Consumed      = 0;
    Remaining     = Goal;
    Percentage    = 0;
    GoalCompleted = 0;

    GreenLED_OFF();
    RedLED_OFF();
    YellowLED_OFF();
}

//--------------------------------------------------
// Drink One Glass
//--------------------------------------------------

void DrinkWater(void)
{
    if(Consumed < Goal)
        Consumed++;

    Remaining = Goal - Consumed;

    Percentage = (Consumed * 100) / Goal;

    if(Consumed >= Goal)
    {
        GoalCompleted = 1;

        RedLED_OFF();
        YellowLED_OFF();
        GreenLED_ON();
    }
    else
    {
        GoalCompleted = 0;
        GreenLED_OFF();

        if(Percentage < 50)
        {
            RedLED_ON();
        }
        else
        {
            RedLED_OFF();
        }
    }
}

//--------------------------------------------------
// Reset Everyday
//--------------------------------------------------

void ResetHydration(void)
{
    Consumed = 0;
		GoalCompleted = 0;
    Remaining = Goal;
    Percentage = 0;
		GreenLED_OFF();
		RedLED_OFF();
		YellowLED_OFF();
}

//--------------------------------------------------
// Detect New Day
//--------------------------------------------------

void CheckNewDay(void)
{
    static u32 PreviousDate = 0;

    s32 date;
    s32 month;
    s32 year;

    GetRTCDateInfo(&date,&month,&year);

    if(date != PreviousDate)
    {
        PreviousDate = date;
				MissedReminders = 0;
        ResetHydration();
    }
}

//--------------------------------------------------
// Display on LCD
//--------------------------------------------------

void DisplayHydration(void)
{
    CmdLCD(GOTO_LINE2_POS0);

    StrLCD("G:");
    U32LCD(Goal);

    StrLCD(" R:");
    U32LCD(Remaining);

    StrLCD(" C:");
    U32LCD(Consumed);

    CharLCD(' ');

    U32LCD(Percentage);

    CharLCD('%');

    CharLCD(' ');
}

void SetGoal(u32 goal)
{
    if(goal == 0)
        return;

    Goal = goal;

    if(Consumed > Goal)
        Consumed = Goal;

    Remaining = Goal - Consumed;

    Percentage = (Consumed * 100) / Goal;
}

void DisplayHydrationStatus(void)
{
    CmdLCD(CLEAR_LCD);

    CmdLCD(GOTO_LINE1_POS0);

    StrLCD("GOAL:");
    U32LCD(Goal);

    StrLCD("  REM:");
    U32LCD(Remaining);

    CmdLCD(GOTO_LINE2_POS0);

    StrLCD("CON:");
    U32LCD(Consumed);

    StrLCD("  PER:");
    U32LCD(Percentage);
    CharLCD('%');
}
