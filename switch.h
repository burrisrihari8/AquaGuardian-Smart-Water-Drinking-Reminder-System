#ifndef __SWITCH_H__
#define __SWITCH_H__

#include "types.h"

void Switch_Init(void);
u8 DrinkSwitchPressed(void);
void DrinkSwitch_Task(void);

#endif
