#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "types.h"

void DisplayWelcome(void);
void DisplayStatus(void);
void DisplayReminder(void);
void DisplayGoalComplete(void);
void DisplayNextReminder(void);

void DisplayGlasses(void);
void ResetReminderScroll(void);
void ScrollReminderMessage(void);

void DisplayTimeDate(void);
void DisplayTimeOnly(void);
u8 IsSecondChanged(void);

#endif
