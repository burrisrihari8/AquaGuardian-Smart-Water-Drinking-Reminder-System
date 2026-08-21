#include "display.h"
#include "lcd.h"
#include "lcd_defines.h"
#include "rtc.h"
#include "hydration.h"
#include "delay.h"
#include "reminder.h"

s32 hour,min,sec;
s32 date,month,year;

static int ReminderScrollPos = 16;


/*--------------------------------------------------
                Welcome Screen
--------------------------------------------------*/
void DisplayWelcome(void)
{
    CmdLCD(CLEAR_LCD);

    StrLCD("  AQUAGUARDIAN");

    CmdLCD(GOTO_LINE2_POS0);

    StrLCD("   SYSTEM ON");

    delay_ms(1000);

    CmdLCD(CLEAR_LCD);
}


/*--------------------------------------------------
                Status Display
--------------------------------------------------*/
void DisplayStatus(void)
{
    GetRTCTimeInfo(&hour,&min,&sec);
    GetRTCDateInfo(&date,&month,&year);

    DisplayRTCTime(hour,min,sec);

    CmdLCD(GOTO_LINE1_POS0 + 8);
    CharLCD(' ');

    DisplayRTCShortDate(date,month);

    DisplayHydration();
}


/*--------------------------------------------------
                Reminder Display
--------------------------------------------------*/
void DisplayReminder(void)
{
    CmdLCD(CLEAR_LCD);

    ResetReminderScroll();

    DisplayGlasses();
}


/*--------------------------------------------------
                Goal Completed
--------------------------------------------------*/
void DisplayGoalComplete(void)
{
    CmdLCD(CLEAR_LCD);

    CmdLCD(GOTO_LINE1_POS0);

    StrLCD(" GOAL ACHIEVED");

    CmdLCD(GOTO_LINE2_POS0);

    StrLCD("   GOOD JOB!");
}


/*--------------------------------------------------
                Next Reminder
--------------------------------------------------*/
void DisplayNextReminder(void)
{
    s32 hr,min;
    u32 totalMinutes;

    totalMinutes = NextReminderMinute;

    /* Handle next day */
    totalMinutes %= 1440;

    hr = totalMinutes / 60;
    min = totalMinutes % 60;

    CmdLCD(CLEAR_LCD);

    CmdLCD(GOTO_LINE1_POS0);

    StrLCD("NEXT REMINDER AT");

    CmdLCD(GOTO_LINE2_POS0);

    //StrLCD("AT ");

    CharLCD((hr / 10) + '0');
    CharLCD((hr % 10) + '0');

    CharLCD(':');

    CharLCD((min / 10) + '0');
    CharLCD((min % 10) + '0');
		
		StrLCD(" Missed:");

    U32LCD(MissedReminders);
}


/*--------------------------------------------------
                Display Glasses
--------------------------------------------------*/
void DisplayGlasses(void)
{
    u32 i;

    CmdLCD(GOTO_LINE2_POS0);

    for(i = 0; i < Goal && i < 16; i++)
    {
        if(i < Consumed)
        {
            CharLCD(1);      /* Filled glass */
        }
        else
        {
            CharLCD(0);      /* Empty glass */
        }
    }
}


/*--------------------------------------------------
                Reset Scroll Position
--------------------------------------------------*/
void ResetReminderScroll(void)
{
    ReminderScrollPos = 16;
}


/*--------------------------------------------------
                Scroll Reminder Message
--------------------------------------------------*/
void ScrollReminderMessage(void)
{
    char msg[] = "DRINK WATER BE HYDRATED";
    char disp[17];

    int len;
    int i;
    int index;

    len = 0;

    while(msg[len] != '\0')
    {
        len++;
    }


    /* Build 16-character window */
    for(i = 0; i < 16; i++)
    {
        index = i - ReminderScrollPos;

        if(index >= 0 && index < len)
        {
            disp[i] = msg[index];
        }
        else
        {
            disp[i] = ' ';
        }
    }

    disp[16] = '\0';


    /* Display scrolling message */
    CmdLCD(GOTO_LINE1_POS0);

    StrLCD(disp);

    ReminderScrollPos--;


    /* Restart scrolling */
    if(ReminderScrollPos < -len)
    {
        ReminderScrollPos = 16;
    }
}


/*--------------------------------------------------
                Window 1
          RTC Time + Day
              Full Date
--------------------------------------------------*/
void DisplayTimeDate(void)
{
    s32 day;

    GetRTCTimeInfo(&hour,&min,&sec);
    GetRTCDateInfo(&date,&month,&year);

    GetRTCDay(&day);

    CmdLCD(CLEAR_LCD);

    /* Line 1: Time + Day */
    CmdLCD(GOTO_LINE1_POS0);

    DisplayRTCTime(hour,min,sec);

    StrLCD(" ");

    DisplayRTCDay(day);


    /* Line 2: Full Date */
    CmdLCD(GOTO_LINE2_POS0);

    CharLCD((date / 10) + '0');
    CharLCD((date % 10) + '0');

    CharLCD('/');

    CharLCD((month / 10) + '0');
    CharLCD((month % 10) + '0');

    CharLCD('/');

    CharLCD((year / 1000) + '0');
    CharLCD(((year / 100) % 10) + '0');
    CharLCD(((year / 10) % 10) + '0');
    CharLCD((year % 10) + '0');
}

/*--------------------------------------------------
             Update Only RTC Time

        No CLEAR_LCD here
        Prevents flickering
--------------------------------------------------*/
void DisplayTimeOnly(void)
{
    s32 day;

    GetRTCTimeInfo(&hour,&min,&sec);
    GetRTCDay(&day);

    CmdLCD(GOTO_LINE1_POS0);

    DisplayRTCTime(hour,min,sec);

    StrLCD(" ");

    DisplayRTCDay(day);
}

/*--------------------------------------------------
          Detect RTC Second Change

          Returns 1 when second changes
          Returns 0 otherwise
--------------------------------------------------*/
u8 IsSecondChanged(void)
{
    static s32 LastSecond = -1;

    s32 h;
    s32 m;
    s32 s;

    GetRTCTimeInfo(&h,&m,&s);

    if(s != LastSecond)
    {
        LastSecond = s;

        return 1;
    }

    return 0;
}
