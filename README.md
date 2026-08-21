# 💧 AquaGuardian — Smart Water Drinking Reminder System

<p align="center">
  <img src="https://img.shields.io/badge/MCU-LPC2148-blue?style=for-the-badge">
  <img src="https://img.shields.io/badge/IDE-Keil%20µVision-orange?style=for-the-badge">
  <img src="https://img.shields.io/badge/Language-C-00599C?style=for-the-badge">
  <img src="https://img.shields.io/badge/Platform-Embedded%20Systems-success?style=for-the-badge">
</p>

<p align="center">
  <b>💧 A Smart Embedded System for Timely Hydration Reminders and Daily Water Tracking</b>
</p>

---

## 🌊 About the Project

**AquaGuardian** is an embedded-based **Smart Water Drinking Reminder System** developed using the **LPC2148 ARM7 microcontroller**.

The system helps users maintain regular water intake by generating periodic drinking reminders. It combines an **RTC, LCD, matrix keypad, external interrupt, push button, LEDs and buzzer** to provide an interactive hydration-monitoring system.

The user can configure:

* ⏰ RTC Time
* 📅 RTC Date
* 🎯 Daily Water Goal
* 🔔 Reminder Interval

The system tracks the amount of water consumed throughout the day and displays the hydration progress on the LCD.

---

# ✨ Key Features

| Feature                           | Description                                     |
| --------------------------------- | ----------------------------------------------- |
| ⏰ **RTC**                         | Maintains current time, date and day            |
| 🔔 **Water Reminder**             | Generates reminders at the configured interval  |
| 🥤 **Drink Detection**            | Push button acknowledges drinking water         |
| 📊 **Hydration Tracking**         | Tracks goal, consumed, remaining and percentage |
| 🎯 **Daily Goal**                 | User-configurable water consumption goal        |
| 🔴 **Red LED**                    | Indicates low hydration progress                |
| 🟡 **Yellow LED**                 | Indicates an active drinking reminder           |
| 🟢 **Green LED**                  | Indicates daily hydration goal achieved         |
| 🔊 **Buzzer**                     | Provides audible reminder alert                 |
| ⌨️ **4×4 Keypad**                 | Used for configuration and numeric input        |
| ⚡ **EINT0**                       | Provides quick access to configuration menu     |
| 🖥️ **16×2 LCD**                  | Displays system information and status          |
| 🥛 **Custom LCD Graphics**        | Displays filled and empty glass symbols         |
| ⏱️ **30-Second Reminder Timeout** | Automatically stops an unanswered reminder      |
| 📌 **Missed Reminder Counter**    | Tracks reminders that were not acknowledged     |

---

# 🧠 System Concept

```text
                    💧 AQUAGUARDIAN
                           │
                           ▼
                  ┌─────────────────┐
                  │    LPC2148      │
                  │     ARM7        │
                  └────────┬────────┘
                           │
          ┌────────────────┼────────────────┐
          │                │                │
          ▼                ▼                ▼
       ⏰ RTC            ⌨️ KEYPAD        ⚡ EINT0
          │                │                │
          │                │                │
          ▼                ▼                ▼
   Reminder Timing     User Input      Configuration
          │
          ▼
   ┌───────────────┐
   │   Reminder    │
   │    Manager    │
   └───────┬───────┘
           │
      ┌────┴─────┐
      │          │
      ▼          ▼
   🔊 Buzzer   🟡 LED
      │          │
      └────┬─────┘
           │
           ▼
      🥤 Drink Switch
           │
           ▼
      DrinkWater()
           │
           ▼
   ┌──────────────────┐
   │ Hydration Status │
   └────────┬─────────┘
            │
            ▼
       📊 LCD Display
```

---

# 🖥️ LCD User Interface

AquaGuardian uses a **16×2 LCD** as the primary user interface.

## 🏠 Home Screen — Time & Date

```text
┌────────────────┐
│ 11:45:23 FRI   │
│ 21/08/2026     │
└────────────────┘
```

Displays:

* Current time
* Day of the week
* Current date

---

## 💧 Home Screen — Hydration Status

```text
┌────────────────┐
│ GOAL:8  REM:5  │
│ CON:3   PER:37%│
└────────────────┘
```

Displays:

* 🎯 Daily goal
* 💧 Remaining glasses
* 🥤 Consumed glasses
* 📊 Hydration percentage

---

## 🔔 Home Screen — Next Reminder

```text
┌────────────────┐
│NEXT REMINDER AT│
│11:46 Missed:0  │
└────────────────┘
```

Displays the next scheduled reminder and the number of missed reminders.

---

# 🚨 Reminder Screen

When the reminder interval expires, AquaGuardian enters **Reminder Mode**.

```text
┌────────────────┐
│ DRINK WATER BE │
│ 🥛 🥛 🥛 ⬜ ⬜ │
└────────────────┘
```

The reminder includes:

* 🟡 Yellow LED
* 🔊 Buzzer
* 📢 Scrolling reminder message
* 🥛 Visual glass indicators

The reminder remains active for **30 seconds** if the user does not acknowledge it.

---

# 🥛 Water Consumption Tracking

When the user presses the **Drink Switch** during a reminder:

```text
Drink Switch
      │
      ▼
 Stop Reminder
      │
      ├── 🔊 Buzzer OFF
      ├── 🟡 Yellow LED OFF
      │
      ▼
 DrinkWater()
      │
      ├── Consumed++
      ├── Remaining--
      └── Percentage Updated
             │
             ▼
       Next Reminder
```

Example:

```text
Before Drinking

┌────────────────┐
│ GOAL:8  REM:6  │
│ CON:2   PER:25%│
└────────────────┘


After Drinking

┌────────────────┐
│ GOAL:8  REM:5  │
│ CON:3   PER:37%│
└────────────────┘
```

---

# ⏱️ Reminder Timeout

If the user does not press the Drink Switch:

```text
Reminder Starts
      │
      ▼
🟡 Yellow LED ON
🔊 Buzzer ON
      │
      ▼
   30 Seconds
      │
      ▼
Reminder Timeout
      │
      ├── 🟡 LED OFF
      ├── 🔊 Buzzer OFF
      ├── MissedReminders++
      │
      ▼
Next Reminder Scheduled
```

A missed reminder **does not increase the consumed-water count**.

---

# 🎯 Goal Completion

When:

```text
Consumed >= Goal
```

the system considers the daily hydration goal completed.

```text
┌────────────────┐
│  GOAL ACHIEVED │
│   GOOD JOB!    │
└────────────────┘
```

🟢 **Green LED ON**

The reminder task also stops generating further reminders after the goal is completed.

---

# ⚙️ Configuration Menu

The user can enter configuration mode using **EINT0**.

```text
┌────────────────┐
│1:RTC  2:GOAL   │
│3:INT  4:EXIT   │
└────────────────┘
```

### RTC Menu

```text
┌────────────────┐
│1:HOUR 2:MIN    │
│3:SEC  4:DATE   │
└────────────────┘
```

```text
┌────────────────┐
│5:MONTH 6:YEAR  │
│7:DAY   8:EXIT  │
└────────────────┘
```

### Available Configuration

```text
       CONFIGURATION
              │
       ┌──────┼──────┐
       │      │      │
       ▼      ▼      ▼
      RTC    GOAL   INTERVAL
       │      │      │
       ▼      ▼      ▼
    Time/   Daily   Reminder
    Date    Goal    Interval
```

---

# ⌨️ Keypad Interface

The project uses a **4×4 matrix keypad**.

```text
┌─────┬─────┬─────┬─────┐
│  7  │  8  │  9  │  B  │
├─────┼─────┼─────┼─────┤
│  4  │  5  │  6  │  /  │
├─────┼─────┼─────┼─────┤
│  1  │  2  │  3  │  -  │
├─────┼─────┼─────┼─────┤
│  C  │  0  │  E  │  +  │
└─────┴─────┴─────┴─────┘
```

During numeric input:

```text
0–9 → Enter number
B   → Backspace
C   → Clear
E   → Enter
```

Example:

```text
┌────────────────┐
│ Enter Interval:│
│      15        │
└────────────────┘
```

---

# 🔌 Hardware Used

| Component             | Purpose                   |
| --------------------- | ------------------------- |
| **LPC2148**           | Main ARM7 microcontroller |
| **16×2 LCD**          | User interface            |
| **4×4 Matrix Keypad** | Configuration/input       |
| **RTC**               | Time/date tracking        |
| **Push Button**       | Drink acknowledgement     |
| **Red LED**           | Low hydration indication  |
| **Yellow LED**        | Reminder indication       |
| **Green LED**         | Goal completion           |
| **Buzzer**            | Audible reminder          |
| **EINT0**             | Configuration interrupt   |

---

# 💻 Software

```text
Microcontroller : LPC2148 ARM7
IDE             : Keil µVision
Language        : Embedded C
Simulation      : Proteus
LCD             : 16×2 Character LCD
Keypad          : 4×4 Matrix Keypad
RTC             : LPC2148 Internal RTC
```

---

# 🧩 Software Architecture

```text
                 ┌────────────────────┐
                 │      main.c        │
                 │   System Control   │
                 └─────────┬──────────┘
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
        ▼                  ▼                  ▼
   ┌─────────┐       ┌───────────┐      ┌──────────┐
   │  rtc.c  │       │reminder.c │      │hydration.c│
   └─────────┘       └───────────┘      └──────────┘
        │                  │                  │
        └──────────────────┼──────────────────┘
                           │
                           ▼
                    ┌────────────┐
                    │ display.c  │
                    └─────┬──────┘
                          │
                          ▼
                       lcd.c
```

Supporting drivers:

```text
lcd.c
kpm.c
led.c
buzzer.c
delay.c
switch.c
eint.c
```

---

# 📁 Project Structure

```text
AquaGuardian/
│
├── 📄 README.md
│
├── 📁 Source/
│   ├── main.c
│   ├── interrupts_test.c
│   ├── rtc.c
│   ├── reminder.c
│   ├── hydration.c
│   ├── display.c
│   ├── lcd.c
│   ├── kpm.c
│   ├── led.c
│   ├── buzzer.c
│   ├── delay.c
│   └── switch.c
│
├── 📁 Include/
│   ├── types.h
│   ├── lcd.h
│   ├── lcd_defines.h
│   ├── rtc.h
│   ├── reminder.h
│   ├── hydration.h
│   ├── display.h
│   ├── kpm.h
│   ├── kpm_defines.h
│   ├── led.h
│   ├── buzzer.h
│   ├── delay.h
│   ├── switch.h
│   └── eint.h
│
└── 📁 Proteus/
    └── AquaGuardian simulation files
```

---

# 🔄 Complete System Flow

```text
                  POWER ON
                     │
                     ▼
              Initialize Modules
                     │
                     ▼
              💧 AQUAGUARDIAN
                 SYSTEM ON
                     │
                     ▼
                Home Screen
                     │
          ┌──────────┼──────────┐
          │          │          │
          ▼          ▼          ▼
       Time/Date  Hydration  Next Reminder
          │          │          │
          └──────────┼──────────┘
                     │
                     ▼
             Reminder Time?
                /         \
              NO           YES
              │             │
              │             ▼
              │       Reminder Active
              │             │
              │       ┌─────┴─────┐
              │       │           │
              │       ▼           ▼
              │    Drink SW    30 sec
              │       │        Timeout
              │       │           │
              │       ▼           ▼
              │   DrinkWater   Missed++
              │       │           │
              │       └─────┬─────┘
              │             │
              └─────────────┘
                     │
                     ▼
               Next Reminder
```

---

# 🟢 LED Status

```text
🔴 RED LED
   ↓
Low hydration progress
Percentage < 50%

🟡 YELLOW LED
   ↓
Reminder currently active

🟢 GREEN LED
   ↓
Daily hydration goal achieved
```

---

# 🧠 Embedded Concepts Demonstrated

This project brings together several important embedded-systems concepts:

* 🔹 ARM7 LPC2148 programming
* 🔹 GPIO configuration
* 🔹 LCD interfacing
* 🔹 Matrix keypad scanning
* 🔹 RTC programming
* 🔹 External interrupts
* 🔹 VIC interrupt configuration
* 🔹 Timer-independent software scheduling
* 🔹 State-based application flow
* 🔹 User input validation
* 🔹 Custom LCD characters
* 🔹 Buzzer control
* 🔹 LED status indication
* 🔹 Embedded C modular programming
* 🔹 Peripheral driver development

---

# 🛠️ Important Functions

### RTC

```c
RTC_Init()
SetRTCTimeInfo()
GetRTCTimeInfo()
SetRTCDateInfo()
GetRTCDateInfo()
SetRTCDay()
GetRTCDay()
```

### Hydration

```c
Hydration_Init()
DrinkWater()
ResetHydration()
SetGoal()
```

### Reminder

```c
Reminder_Init()
StartReminder()
StopReminder()
StopReminderTimeout()
Reminder_Task()
SetReminderInterval()
UpdateNextReminder()
```

### Display

```c
DisplayWelcome()
DisplayStatus()
DisplayReminder()
DisplayGoalComplete()
DisplayNextReminder()
DisplayHydrationStatus()
DisplayTimeDate()
DisplayTimeOnly()
ScrollReminderMessage()
```

### Keypad

```c
Init_KPM()
keyscan()
keyscan_nb()
ReadNumLCD()
```

---

# 🧪 Example Demonstration

Suppose:

```text
Daily Goal      = 8 glasses
Reminder        = Every 1 minute
Consumed        = 0
```

The system starts:

```text
┌────────────────┐
│ 11:45:00 FRI   │
│ 21/08/2026     │
└────────────────┘
```

After the reminder interval:

```text
┌────────────────┐
│ DRINK WATER BE │
│ 🥛 🥛 🥛 🥛 ...│
└────────────────┘

🔊 BUZZER ON
🟡 YELLOW LED ON
```

User presses the Drink Switch:

```text
Consumed = 1
Remaining = 7
Percentage = 12%
```

The next reminder is automatically scheduled.

If the user doesn't respond for 30 seconds:

```text
MissedReminders++
```

and the system returns to normal operation.

---
## Proteus Simulation

<img src="Welcome msg.png" width="500">
<img src="window-1.png" width="500">
<img src="window-2.png" width="500">
<img src="window-3.png" width="500">
<img src="reminder.png" width="500">

# 🚀 Future Improvements

Possible future enhancements include:

* 📱 Mobile application connectivity
* ☁️ IoT/cloud-based hydration monitoring
* 📈 Daily/weekly hydration statistics
* 🔋 Battery-backed RTC
* 👤 Multiple user profiles
* 🔔 Custom reminder tones
* 🌡️ Environmental sensor integration
* 📊 Graphical hydration history
* 🕐 Smarter reminder scheduling
* 💾 EEPROM-based user settings storage

---

# 🎓 Project Purpose

The main purpose of AquaGuardian is to demonstrate how multiple embedded peripherals can be integrated into a **single real-time interactive application**.

Instead of treating the RTC, LCD, keypad, interrupts, GPIO, LEDs and buzzer as isolated experiments, this project combines them into one practical embedded system.

> **A small embedded system with a simple goal: remind, track, and encourage better hydration. 💧**

---

# 👨‍💻 Project

**AquaGuardian — Smart Water Drinking Reminder System**
## 👨‍💻 Author

**BURRI SRIHARI**

### 🔧 Embedded Systems / ARM7 Project

**Platform:** LPC2148 ARM7
**Development Environment:** Keil µVision
**Programming Language:** Embedded C
**Application Area:** Embedded Systems / Healthcare / Smart Monitoring

---

<p align="center">
  <b>💧 Stay Hydrated • Stay Healthy • Stay Alert 💧</b>
</p>

<p align="center">
  ⭐ If you find this project useful, consider giving the repository a star!
</p>
