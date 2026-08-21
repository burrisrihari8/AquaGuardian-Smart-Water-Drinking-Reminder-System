#include "reminder.h"
#include "rtc.h"
#include "display.h"
#include "hydration.h"
#include "led.h"
#include "buzzer.h"


u32 ReminderInterval = 1;       /* Default = 1 minute */

u32 NextReminderMinute = 0;

u8 ReminderActive = 0;


/*-------------------------------------------------------
                NEW VARIABLE

        Stores the RTC time when reminder starts.
-------------------------------------------------------*/

u32 ReminderStartSecond = 0;

u32 MissedReminders = 0;


/*-------------------------------------------------------
                    Initialize Reminder
-------------------------------------------------------*/

void Reminder_Init(void)
{
    s32 hr, min, sec;

    GetRTCTimeInfo(&hr, &min, &sec);

    NextReminderMinute =
        (hr * 60) + min + ReminderInterval;

    if(NextReminderMinute >= 1440)
        NextReminderMinute -= 1440;

    ReminderActive = 0;

    ReminderStartSecond = 0;
}


/*-------------------------------------------------------
                    Start Reminder
-------------------------------------------------------*/

void StartReminder(void)
{
    s32 hr, min, sec;

    ReminderActive = 1;

    RedLED_OFF();
    GreenLED_OFF();

    YellowLED_ON();

    DisplayReminder();

    Buzzer_ON();


    /*-----------------------------------------------
            Store reminder starting time
    -----------------------------------------------*/

    GetRTCTimeInfo(&hr, &min, &sec);

    ReminderStartSecond =
        (hr * 3600) +
        (min * 60) +
        sec;
}


/*-------------------------------------------------------
                    Stop Reminder
             
        This function is used when the
        DRINK switch is pressed.

        Therefore DrinkWater() is called.
-------------------------------------------------------*/

void StopReminder(void)
{
    s32 hr, min, sec;

    ReminderActive = 0;

    YellowLED_OFF();

    Buzzer_OFF();


    /*-----------------------------------------------
                User acknowledged reminder
        Therefore count one glass of water.
    -----------------------------------------------*/

    DrinkWater();


    /*-----------------------------------------------
                Schedule next reminder
    -----------------------------------------------*/

    GetRTCTimeInfo(&hr, &min, &sec);

    NextReminderMinute =
        (hr * 60) +
        min +
        ReminderInterval;

    if(NextReminderMinute >= 1440)
        NextReminderMinute -= 1440;


    /*-----------------------------------------------
                    Return HOME
    -----------------------------------------------*/

    DisplayStatus();
}


/*-------------------------------------------------------
            AUTO STOP AFTER 30 SECONDS

        This function is used when the user
        does NOT press the Drink switch.

        Therefore DrinkWater() is NOT called.
-------------------------------------------------------*/

void StopReminderTimeout(void)
{
    s32 hr, min, sec;

    ReminderActive = 0;

    YellowLED_OFF();

    Buzzer_OFF();

		MissedReminders++;

    /*-----------------------------------------------
                Schedule next reminder
    -----------------------------------------------*/

    GetRTCTimeInfo(&hr, &min, &sec);

    NextReminderMinute =
        (hr * 60) +
        min +
        ReminderInterval;

    if(NextReminderMinute >= 1440)
        NextReminderMinute -= 1440;


    /*-----------------------------------------------
                    Return HOME
    -----------------------------------------------*/

    DisplayStatus();
}


/*-------------------------------------------------------
                    Reminder Task
-------------------------------------------------------*/

void Reminder_Task(void)
{
    s32 hr, min, sec;

    u32 CurrentMinute;

    u32 CurrentSecond;

    u32 ElapsedSecond;


    /*-----------------------------------------------
                    Goal completed?
    -----------------------------------------------*/

    if(GoalCompleted)
        return;


    /*-----------------------------------------------
                Reminder already active?
    -----------------------------------------------*/

    if(ReminderActive)
    {
        /*-------------------------------------------
                Check 30-second timeout
        -------------------------------------------*/

        GetRTCTimeInfo(&hr, &min, &sec);

        CurrentSecond =
            (hr * 3600) +
            (min * 60) +
            sec;


        /*-------------------------------------------
                Calculate elapsed seconds

                Handles midnight also.
        -------------------------------------------*/

        if(CurrentSecond >= ReminderStartSecond)
        {
            ElapsedSecond =
                CurrentSecond - ReminderStartSecond;
        }
        else
        {
            /* RTC crossed midnight */

            ElapsedSecond =
                (86400 - ReminderStartSecond) +
                CurrentSecond;
        }


        /*-------------------------------------------
                30 seconds completed
        -------------------------------------------*/

        if(ElapsedSecond >= 30)
        {
            StopReminderTimeout();
        }

        return;
    }


    /*-----------------------------------------------
                Check reminder time
    -----------------------------------------------*/

    GetRTCTimeInfo(&hr, &min, &sec);

    CurrentMinute =
        (hr * 60) + min;


    if(CurrentMinute == NextReminderMinute)
    {
        StartReminder();
    }
}


/*-------------------------------------------------------
                Change Reminder Interval
-------------------------------------------------------*/

void SetReminderInterval(u32 minutes)
{
    ReminderInterval = minutes;

    Reminder_Init();
}


/*-------------------------------------------------------
                Check Reminder Active
-------------------------------------------------------*/

u8 IsReminderActive(void)
{
    return ReminderActive;
}


/*-------------------------------------------------------
            Update Next Reminder

        Used when RTC time is edited.
-------------------------------------------------------*/

void UpdateNextReminder(void)
{
    s32 hr, min, sec;

    u32 CurrentMinutes;


    GetRTCTimeInfo(&hr, &min, &sec);

    CurrentMinutes =
        (hr * 60) + min;


    NextReminderMinute =
        CurrentMinutes + ReminderInterval;


    if(NextReminderMinute >= 1440)
        NextReminderMinute -= 1440;
}
