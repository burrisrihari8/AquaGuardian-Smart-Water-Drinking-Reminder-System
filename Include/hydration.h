#ifndef __HYDRATION_H__
#define __HYDRATION_H__

#include "types.h"

// Global Variables
extern u32 Goal;
extern u32 Consumed;
extern u32 Remaining;
extern u32 Percentage;
extern u8 GoalCompleted;

// Function Prototypes
void Hydration_Init(void);
void DrinkWater(void);
void ResetHydration(void);
void CheckNewDay(void);
void DisplayHydration(void);
void SetGoal(u32 goal);
void DisplayHydrationStatus(void);
#endif
