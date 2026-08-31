// main.c

#include "lcd.h"
#include "lcd_defines.h"
#include "kpm.h"
#include "hydration.h"
#include "rtc.h"
#include "eint.h"
#include "delay.h"
#include "display.h"
#include "switch.h"
#include "reminder.h"
#include "led.h"
#include "buzzer.h"

extern volatile u8 edit_request;

int main()
{
		u8 HomeScreen = 0;
		u8 ScrollCounter = 0;

		s32 ScreenStartSecond = 0;
		s32 hour, min, sec;

    RTC_Init();
    InitLCD();
    BuildCustomChars();
    Init_KPM();
    Init_EINT();
    Switch_Init();
    Buzzer_Init();
    LED_Init();
    Hydration_Init();

    /*--------------------------------
             Welcome Screen
    --------------------------------*/
    DisplayWelcome();


    /*--------------------------------
             Initial RTC
    --------------------------------*/
    SetRTCTimeInfo(11,45,0);

    SetRTCDateInfo(21,8,2026);
		
		SetRTCDay(5);

    /*--------------------------------
             Reminder Init
    --------------------------------*/
    Reminder_Init();


    /*--------------------------------
        First Home Screen Immediately
    --------------------------------*/
    HomeScreen = 0;

    DisplayTimeDate();

    GetRTCTimeInfo(&hour,&min,&sec);

    ScreenStartSecond = sec;


    /*================================
                MAIN LOOP
    =================================*/
    while(1)
    {
        /*--------------------------------
             Configuration Interrupt
        --------------------------------*/
        if(edit_request)
        {
            edit_request = 0;

            EditTime();

            /*
             * After configuration,
             * return directly to home screen
             */
            CmdLCD(CLEAR_LCD);

            HomeScreen = 0;

            DisplayTimeDate();

            GetRTCTimeInfo(&hour,&min,&sec);

            ScreenStartSecond = sec;
        }


        /*--------------------------------
             Check New Day
        --------------------------------*/
        CheckNewDay();


        /*--------------------------------
             Drink Switch
        --------------------------------*/
        DrinkSwitch_Task();


        /*--------------------------------
             Reminder Task
        --------------------------------*/
        Reminder_Task();


        /*================================
              REMINDER MODE
        =================================*/
        if(ReminderActive)
        {
            /*
             * Home screens stop during
             * reminder
             */
            HomeScreen = 0;

            ScrollCounter++;

            /*
             * Control scrolling speed
             */
            if(ScrollCounter >= 4)
            {
                ScrollCounter = 0;

                ScrollReminderMessage();
            }
        }


        /*================================
               NORMAL HOME MODE
        =================================*/
        else
        {
            ScrollCounter = 0;


            /*--------------------------------
               Check RTC second
            --------------------------------*/
            GetRTCTimeInfo(&hour,&min,&sec);


            /*--------------------------------
               Switch screen after 5 seconds
            --------------------------------*/

            if(((sec - ScreenStartSecond + 60) % 60) >= 5)
            {
                /*
                 * Save current second as
                 * starting point of new screen
                 */
                ScreenStartSecond = sec;


                /*
                 * Move to next screen
                 */
                HomeScreen++;


                /*
                 * After Window 3,
                 * return to Window 1
                 */
                if(HomeScreen > 2)
                {
                    HomeScreen = 0;
                }


                /*--------------------------------
                     WINDOW 1

                     RTC TIME + DAY
                     FULL DATE
                --------------------------------*/
                if(HomeScreen == 0)
                {
                    DisplayTimeDate();
                }


                /*--------------------------------
                     WINDOW 2

                     GOAL
                     REMAINING
                     CONSUMED
                     PERCENTAGE
                --------------------------------*/
                else if(HomeScreen == 1)
                {
                    DisplayHydrationStatus();
                }


                /*--------------------------------
                     WINDOW 3

                     NEXT REMINDER
                --------------------------------*/
                else if(HomeScreen == 2)
                {
                    DisplayNextReminder();
                }
            }


            /*--------------------------------
               Window 1 RTC Update

               Update clock every second
               without clearing LCD
            --------------------------------*/
            if(HomeScreen == 0)
            {
                if(IsSecondChanged())
                {
                    DisplayTimeOnly();
                }
            }
        }


        /*--------------------------------
             Small Main Loop Delay
        --------------------------------*/
        delay_ms(20);
    }
}
