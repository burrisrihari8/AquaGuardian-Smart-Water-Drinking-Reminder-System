#ifndef __REMINDER_H__
#define __REMINDER_H__

#include "types.h"

extern u32 ReminderInterval;     // Minutes
extern u8 ReminderActive;
extern u32 NextReminderMinute;

void Reminder_Init(void);
void Reminder_Task(void);
void StartReminder(void);
void StopReminder(void);
void SetReminderInterval(u32 minutes);
u8 IsReminderActive(void);
void DisplayReminder(void);
void UpdateNextReminder(void);
//void StopReminderTimeout(void)

extern u32 MissedReminders;

#endif
