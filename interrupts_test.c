// interrupts_test.c

#include <LPC21xx.h>

#include "types.h"
#include "lcd.h"
#include "lcd_defines.h"
#include "delay.h"
#include "eint.h"
#include "kpm.h"
#include "rtc.h"
#include "hydration.h"
#include "reminder.h"

#define EINT0_CH   14
#define LED        16

volatile u8 edit_request = 0;


/*-------------------------------------------------------
                Function Declarations
-------------------------------------------------------*/

void eint0_isr(void) __irq;

void EditTime(void);
void EditRTC(void);
void EditGoal(void);
void EditReminderInterval(void);

u32 CalculateDay(u32 date, u32 month, u32 year);
u32 GetDaysInMonth(u32 month, u32 year);


/*-------------------------------------------------------
            Initialize External Interrupt
-------------------------------------------------------*/

void Init_EINT(void)
{
    /* P0.1 -> EINT0 */

    PINSEL0 &= ~(3 << 2);
    PINSEL0 |=  (3 << 2);

    /* Optional debug LED */

    IODIR1 |= (1 << LED);

    /* Clear pending interrupt */

    EXTINT = (1 << 0);

    /* Edge triggered */

    EXTMODE |= (1 << 0);

    /* Falling edge */

    EXTPOLAR &= ~(1 << 0);


    /* VIC configuration */

    VICIntSelect &= ~(1 << EINT0_CH);

    VICVectAddr0 = (unsigned)eint0_isr;

    VICVectCntl0 = (1 << 5) | EINT0_CH;

    VICIntEnable = (1 << EINT0_CH);
}


/*-------------------------------------------------------
                        ISR
-------------------------------------------------------*/

void eint0_isr(void) __irq
{
    edit_request = 1;

    EXTINT = (1 << 0);

    VICVectAddr = 0;
}


/*-------------------------------------------------------
                    MAIN EDIT MENU

        Window 1:

        1:RTC  2:GOAL
        3:INT  4:EXIT

        Only 4 exits Edit Mode.
-------------------------------------------------------*/

void EditTime(void)
{
    u8 key;

    u32 Counter = 0;

    u8 PreviousDisplay = 255;


    while(1)
    {
        /*------------------------------------------------
                    Display MAIN EDIT MENU
        ------------------------------------------------*/

        if(PreviousDisplay != 0)
        {
            CmdLCD(CLEAR_LCD);

            StrLCD("1:RTC  2:GOAL");

            CmdLCD(GOTO_LINE2_POS0);

            StrLCD("3:INT  4:EXIT");

            PreviousDisplay = 0;
        }


        /*------------------------------------------------
                    Check keypad
        ------------------------------------------------*/

        key = keyscan_nb();


        /*------------------------------------------------
                        RTC
        ------------------------------------------------*/

        if(key == '1')
        {
            EditRTC();

            CmdLCD(CLEAR_LCD);

            Counter = 0;

            PreviousDisplay = 255;
        }


        /*------------------------------------------------
                    DAILY GOAL
        ------------------------------------------------*/

        else if(key == '2')
        {
            EditGoal();

            CmdLCD(CLEAR_LCD);

            Counter = 0;

            PreviousDisplay = 255;
        }


        /*------------------------------------------------
                REMINDER INTERVAL
        ------------------------------------------------*/

        else if(key == '3')
        {
            EditReminderInterval();

            CmdLCD(CLEAR_LCD);

            Counter = 0;

            PreviousDisplay = 255;
        }


        /*------------------------------------------------
                        EXIT
        ------------------------------------------------*/

        else if(key == '4')
        {
            CmdLCD(CLEAR_LCD);

            StrLCD("Exiting...");

            delay_ms(700);

            CmdLCD(CLEAR_LCD);

            return;
        }


        delay_ms(20);

        Counter++;


        /*
            Main menu only has 4 options,
            so no window switching is required.
        */
    }
}


/*-------------------------------------------------------
                    RTC MENU

        WINDOW 1

        1:HOUR  2:MIN
        3:SEC   4:DATE

        WINDOW 2

        5:MONTH 6:YEAR
        7:DAY   8:EXIT

        Window changes every 5 seconds.

        All RTC parameters are handled
        inside this single RTC menu.
-------------------------------------------------------*/

void EditRTC(void)
{
    s32 hr;
    s32 min;
    s32 sec;

    s32 date;
    s32 month;
    s32 year;

    u32 value;
    u32 day;

    u8 key;

    u32 Counter = 0;

    u8 RTCWindow = 0;

    u8 PreviousWindow = 255;


    while(1)
    {
        /*------------------------------------------------
                    DISPLAY RTC WINDOW
        ------------------------------------------------*/

        if(RTCWindow != PreviousWindow)
        {
            CmdLCD(CLEAR_LCD);


            /*--------------------------------------------
                        RTC WINDOW 1
            --------------------------------------------*/

            if(RTCWindow == 0)
            {
                StrLCD("1:HOUR 2:MIN");

                CmdLCD(GOTO_LINE2_POS0);

                StrLCD("3:SEC  4:DATE");
            }


            /*--------------------------------------------
                        RTC WINDOW 2
            --------------------------------------------*/

            else
            {
                StrLCD("5:MONTH 6:YEAR");

                CmdLCD(GOTO_LINE2_POS0);

                StrLCD("7:DAY   8:EXIT");
            }


            PreviousWindow = RTCWindow;
        }


        /*------------------------------------------------
                    CHECK KEYPAD
        ------------------------------------------------*/

        key = keyscan_nb();


        /*------------------------------------------------
                    OPTION 1 - HOUR
        ------------------------------------------------*/

        if(key == '1')
        {
REENTER_HOUR:

            CmdLCD(CLEAR_LCD);

            StrLCD("Enter Hour:");

            CmdLCD(GOTO_LINE2_POS0);

            value = ReadNumLCD();


            if(value > 23)
            {
                CmdLCD(CLEAR_LCD);

                StrLCD("Invalid Hour");

                delay_ms(1000);

                goto REENTER_HOUR;
            }


            GetRTCTimeInfo(&hr, &min, &sec);

            hr = value;

            SetRTCTimeInfo(hr, min, sec);

            UpdateNextReminder();


            CmdLCD(CLEAR_LCD);

            StrLCD("Hour Updated");

            delay_ms(1000);


            /* Restart RTC menu */

            Counter = 0;

            RTCWindow = 0;

            PreviousWindow = 255;
        }


        /*------------------------------------------------
                    OPTION 2 - MINUTE
        ------------------------------------------------*/

        else if(key == '2')
        {
REENTER_MINUTE:

            CmdLCD(CLEAR_LCD);

            StrLCD("Enter Minute:");

            CmdLCD(GOTO_LINE2_POS0);

            value = ReadNumLCD();


            if(value > 59)
            {
                CmdLCD(CLEAR_LCD);

                StrLCD("Invalid Minute");

                delay_ms(1000);

                goto REENTER_MINUTE;
            }


            GetRTCTimeInfo(&hr, &min, &sec);

            min = value;

            SetRTCTimeInfo(hr, min, sec);

            UpdateNextReminder();


            CmdLCD(CLEAR_LCD);

            StrLCD("Minute Updated");

            delay_ms(1000);


            Counter = 0;

            RTCWindow = 0;

            PreviousWindow = 255;
        }


        /*------------------------------------------------
                    OPTION 3 - SECOND
        ------------------------------------------------*/

        else if(key == '3')
        {
REENTER_SECOND:

            CmdLCD(CLEAR_LCD);

            StrLCD("Enter Second:");

            CmdLCD(GOTO_LINE2_POS0);

            value = ReadNumLCD();


            if(value > 59)
            {
                CmdLCD(CLEAR_LCD);

                StrLCD("Invalid Second");

                delay_ms(1000);

                goto REENTER_SECOND;
            }


            GetRTCTimeInfo(&hr, &min, &sec);

            sec = value;

            SetRTCTimeInfo(hr, min, sec);

            UpdateNextReminder();


            CmdLCD(CLEAR_LCD);

            StrLCD("Second Updated");

            delay_ms(1000);


            Counter = 0;

            RTCWindow = 0;

            PreviousWindow = 255;
        }


        /*------------------------------------------------
                    OPTION 4 - DATE
        ------------------------------------------------*/

        else if(key == '4')
        {
REENTER_DATE:

            CmdLCD(CLEAR_LCD);

            StrLCD("Enter Date:");

            CmdLCD(GOTO_LINE2_POS0);

            value = ReadNumLCD();


            GetRTCDateInfo(&date, &month, &year);


            if(value < 1 ||
               value > GetDaysInMonth(month, year))
            {
                CmdLCD(CLEAR_LCD);

                StrLCD("Invalid Date");

                delay_ms(1000);

                goto REENTER_DATE;
            }


            date = value;


            SetRTCDateInfo(date, month, year);


            /* Calculate day automatically */

            day = CalculateDay(date, month, year);

            SetRTCDay(day);


            CmdLCD(CLEAR_LCD);

            StrLCD("Date Updated");

            delay_ms(1000);


            Counter = 0;

            RTCWindow = 0;

            PreviousWindow = 255;
        }


        /*------------------------------------------------
                    OPTION 5 - MONTH
        ------------------------------------------------*/

        else if(key == '5')
        {
REENTER_MONTH:

            CmdLCD(CLEAR_LCD);

            StrLCD("Enter Month:");

            CmdLCD(GOTO_LINE2_POS0);

            value = ReadNumLCD();


            if(value < 1 || value > 12)
            {
                CmdLCD(CLEAR_LCD);

                StrLCD("Invalid Month");

                delay_ms(1000);

                goto REENTER_MONTH;
            }


            GetRTCDateInfo(&date, &month, &year);


            month = value;


            /*
                Correct date if it is invalid
                for the selected month.
            */

            if(date > GetDaysInMonth(month, year))
            {
                date = GetDaysInMonth(month, year);
            }


            SetRTCDateInfo(date, month, year);


            /* Recalculate day */

            day = CalculateDay(date, month, year);

            SetRTCDay(day);


            CmdLCD(CLEAR_LCD);

            StrLCD("Month Updated");

            delay_ms(1000);


            Counter = 0;

            RTCWindow = 0;

            PreviousWindow = 255;
        }


        /*------------------------------------------------
                    OPTION 6 - YEAR
        ------------------------------------------------*/

        else if(key == '6')
        {
REENTER_YEAR:

            CmdLCD(CLEAR_LCD);

            StrLCD("Enter Year:");

            CmdLCD(GOTO_LINE2_POS0);

            value = ReadNumLCD();


            if(value < 2000 || value > 2099)
            {
                CmdLCD(CLEAR_LCD);

                StrLCD("Invalid Year");

                delay_ms(1000);

                goto REENTER_YEAR;
            }


            GetRTCDateInfo(&date, &month, &year);


            year = value;


            /*
                Check February again because
                leap year status may change.
            */

            if(date > GetDaysInMonth(month, year))
            {
                date = GetDaysInMonth(month, year);
            }


            SetRTCDateInfo(date, month, year);


            /* Recalculate day */

            day = CalculateDay(date, month, year);

            SetRTCDay(day);


            CmdLCD(CLEAR_LCD);

            StrLCD("Year Updated");

            delay_ms(1000);


            Counter = 0;

            RTCWindow = 0;

            PreviousWindow = 255;
        }


        /*------------------------------------------------
                    OPTION 7 - DAY
        ------------------------------------------------*/

        else if(key == '7')
        {
            /*
                Day of week is automatically calculated
                from Date + Month + Year.

                User does not manually enter it.
            */

            GetRTCDateInfo(&date, &month, &year);

            day = CalculateDay(date, month, year);

            SetRTCDay(day);


            CmdLCD(CLEAR_LCD);

            StrLCD("Day Updated");

            delay_ms(1000);


            Counter = 0;

            RTCWindow = 1;

            PreviousWindow = 255;
        }


        /*------------------------------------------------
                    OPTION 8 - EXIT RTC
        ------------------------------------------------*/

        else if(key == '8')
        {
            CmdLCD(CLEAR_LCD);

            return;
        }


        /*------------------------------------------------
                    20 ms DELAY
        ------------------------------------------------*/

        delay_ms(20);

        Counter++;


        /*------------------------------------------------
                    5 SECOND WINDOW CHANGE

                    150 x 20ms = 3000ms
        ------------------------------------------------*/

        if(Counter >= 150)
        {
            Counter = 0;


            if(RTCWindow == 0)
            {
                RTCWindow = 1;
            }
            else
            {
                RTCWindow = 0;
            }


            PreviousWindow = 255;
        }
    }
}


/*-------------------------------------------------------
                Get Days In Month
-------------------------------------------------------*/

u32 GetDaysInMonth(u32 month, u32 year)
{
    switch(month)
    {
        case 1:
            return 31;

        case 2:

            if((year % 400 == 0) ||
               ((year % 4 == 0) &&
                (year % 100 != 0)))
            {
                return 29;
            }
            else
            {
                return 28;
            }

        case 3:
            return 31;

        case 4:
            return 30;

        case 5:
            return 31;

        case 6:
            return 30;

        case 7:
            return 31;

        case 8:
            return 31;

        case 9:
            return 30;

        case 10:
            return 31;

        case 11:
            return 30;

        case 12:
            return 31;

        default:
            return 0;
    }
}


/*-------------------------------------------------------
                    Calculate Day

        0 = SUN
        1 = MON
        2 = TUE
        3 = WED
        4 = THU
        5 = FRI
        6 = SAT
-------------------------------------------------------*/

u32 CalculateDay(u32 date, u32 month, u32 year)
{
    u32 k;
    u32 y;
    u32 m;
    u32 dow;


    k = year;

    m = month;


    if(m < 3)
    {
        m = m + 12;

        k = k - 1;
    }


    y = k % 100;

    k = k / 100;


    /* Zeller's Congruence */

    dow = (date +
           ((13 * (m + 1)) / 5) +
           y +
           (y / 4) +
           (k / 4) +
           (5 * k)) % 7;


    /*
        Zeller:

        0 = Saturday
        1 = Sunday
        2 = Monday
        ...
        6 = Friday

        Convert to:

        0 = Sunday
        1 = Monday
        ...
        6 = Saturday
    */

    dow = (dow + 6) % 7;


    return dow;
}


/*-------------------------------------------------------
                    DAILY GOAL EDIT
-------------------------------------------------------*/

void EditGoal(void)
{
    u32 goal;


REENTER_GOAL:

    CmdLCD(CLEAR_LCD);

    StrLCD("Daily Goal");

    delay_ms(500);


    CmdLCD(CLEAR_LCD);

    StrLCD("Goal:");

    CmdLCD(GOTO_LINE2_POS0);

    goal = ReadNumLCD();

    delay_ms(500);


    if(goal < 1 || goal > 20)
    {
        CmdLCD(CLEAR_LCD);

        StrLCD("Invalid Goal");

        delay_ms(1000);

        goto REENTER_GOAL;
    }


    SetGoal(goal);


    CmdLCD(CLEAR_LCD);

    StrLCD("Goal Updated");

    delay_ms(1000);
}


/*-------------------------------------------------------
                REMINDER INTERVAL EDIT
-------------------------------------------------------*/

void EditReminderInterval(void)
{
    u32 interval;


REENTER_INTERVAL:

    CmdLCD(CLEAR_LCD);

    StrLCD("Interval(min)");

    CmdLCD(GOTO_LINE2_POS0);

    interval = ReadNumLCD();

    delay_ms(500);


    if(interval < 1 || interval > 1380)
    {
        CmdLCD(CLEAR_LCD);

        StrLCD("Invalid");

        delay_ms(1000);

        goto REENTER_INTERVAL;
    }


    SetReminderInterval(interval);


    CmdLCD(CLEAR_LCD);

    StrLCD("Interval=");

    U32LCD(interval);

    StrLCD(" Min");

    CmdLCD(GOTO_LINE2_POS0);

    StrLCD("UPDATED!");

    delay_ms(1000);
}
