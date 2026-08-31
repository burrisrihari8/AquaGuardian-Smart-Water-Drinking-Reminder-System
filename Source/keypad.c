//keypad.c

// 4x4 Matrix Keypad driver.
// This file contains functions to:
//     1. Initialize the keypad GPIO pins.
//     2. Detect whether any key is pressed.
//     3. Identify the pressed row.
//     4. Identify the pressed column.
//     5. Find the actual key value.
//     6. Read a complete number from the keypad.

// Keypad arrangement:
//
//        C0   C1   C2   C3
//     R0  7    8    9    B
//     R1  4    5    6    /
//     R2  1    2    3    -
//     R3  C    0    E    +

//==============================================================

#include <LPC21xx.h>			// Contains LPC21xx hardware register definitions.
#include "types.h"				// Provides user-defined data types.
#include "kpm_defines.h"	// Contains keypad-related definitions.
#include "defines.h"			// Contains bit manipulation macros.
#include "lcd_defines.h"	// LCD functions such as InitLCD(), U32LCD(), StrLCD()
#include "lcd.h"					// LCD command definitions such as CLEAR_LCD

/*==============================================================
                         KEYPAD LUT
  =============================================================*/

/*
   kpmLUT
 
   LUT = Look-Up Table.
 
   This 2-dimensional array stores the character corresponding
   to every row and column combination of the keypad.
 
   First index  -> Row number.
   Second index -> Column number.
 
   Example:

       kpmLUT[0][0] = '7'
       kpmLUT[1][1] = '5'
       kpmLUT[2][2] = '3'
       kpmLUT[3][3] = '+'
  
   Instead of using many if/else conditions, the row and
   column numbers directly select the required key.
*/
u8 kpmLUT[4][4] =
{
    /* Row 0 */
    {'7','8','9','B'},

    /* Row 1 */
    {'4','5','6','/'},

    /* Row 2 */
    {'1','2','3','-'},

    /* Row 3 */
    {'C','0','E','+'}
};

/*==============================================================
                          Init_KPM()
  =============================================================*/

/*
   Function: Init_KPM()
 
   Purpose:
   Initializes the row pins of the 4x4 keypad
   as output pins.
 
   In a matrix keypad:
 
       Rows    -> Output
       Columns -> Input
 
   ROW0 represents the starting row pin.
 
   The four row pins are:
 
       ROW0
       ROW0+1
       ROW0+2
       ROW0+3
*/
void Init_KPM(void)
{
    /*
       Configure four consecutive bits starting from ROW0 as output pins.
     
       WRITENIBBLE() writes a 4-bit value.
     
       Value 15 = binary 1111.
     
       Therefore all four row pins are configured as outputs.
    */
    WRITENIBBLE(IODIR1,ROW0,15);
}

/*==============================================================
                           colscan()
  =============================================================*/

/*
   Function: colscan()
 
   Purpose:
   Checks whether any key is currently pressed.
 
   Return value:
 
       0 -> Key is pressed.
       1 -> No key is pressed.
 
   The four column pins are read together as a nibble.
*/
u32 colscan(void)
{
    /*
       Read the four column pins.
     
       When no key is pressed, the column nibble is expected
       to be 1111 binary, which is decimal 15.
     
       If the value becomes less than 15, at least one
       column has gone LOW, indicating a key press.
    */
    if(READNIBBLE(IOPIN1,COL0) < 15)
    {
        /*
           At least one column is LOW.
         
           Therefore a key is pressed.
        */
        return 0;
    }
    else
    {
        /*
           All columns are HIGH.
         
           Therefore no key is pressed.
        */
        return 1;
    }
}

/*==============================================================
                           rowcheck()
  =============================================================*/

/*
   Function: rowcheck()
 
   Purpose:
   Finds which row contains the pressed key.
 
   Return value:
 
       0 -> Row 0
       1 -> Row 1
       2 -> Row 2
       3 -> Row 3
*/
u32 rowcheck(void)
{
    /*
       rno
     
       Row number used as a loop counter.
     
       It varies from 0 to 3.
    */
    u32 rno;


    /* Test each row one by one. */
    for(rno=0; rno<4; rno++)
    {
        /*
           Make only the current row LOW.
         
           Example:
         
           rno = 0
         
           1 << 0 = 0001
         
           ~0001 = ...1110
         
           The four row bits therefore become:
         
               1110
         
           This activates row 0.
         
           The next iteration activates row 1, and so on.
        */
        WRITENIBBLE(IOPIN1,ROW0,(~(1<<rno)));

        /*
           Check whether any column becomes LOW.
         
           If a column becomes LOW, the currently selected
           row contains the pressed key.
        */
        if(colscan() == 0)
        {
            /*
               Stop searching because the pressed row
               has been found.
            */
            break;
        }
    }

    /*
       Restore all four row outputs to LOW.
     
       This returns the row pins to their default state
       after row detection.
    */
    IOCLR1 = 15 << ROW0;

    /* Return the detected row number. */
    return rno;
}

/*==============================================================
                           colcheck()
  =============================================================*/

/*
   Function: colcheck()
 
   Purpose:
   Finds which column contains the pressed key.
 
   Return value:
 
       0 -> Column 0
       1 -> Column 1
       2 -> Column 2
       3 -> Column 3
*/
u32 colcheck(void)
{
    /*
       cno
      
       Column number used as a loop counter.
    */
    u32 cno;

    /* Check each column one by one. */
    for(cno=0; cno<4; cno++)
    {
        /*
           Read one column bit.
         
           COL0 is the starting column pin.
         
           cno + COL0 selects:
         
               cno = 0 -> COL0
               cno = 1 -> COL1
               cno = 2 -> COL2
               cno = 3 -> COL3
         
           A LOW value means that this column
           is connected to the currently active row.
        */
        if(READBIT(IOPIN1,(cno+COL0)) == 0)
        {
            /* Pressed column has been found. */
            break;
        }
    }

    /* Return the detected column number. */
    return cno;
}

/*==============================================================
                           keyscan()
  =============================================================*/

/*
   Function: keyscan()
 
   Purpose:
   Detects and returns the actual key pressed
   on the matrix keypad.
 
   The function performs these steps:
 
       1. Wait for key press.
       2. Find row.
       3. Find column.
       4. Use row and column with kpmLUT.
       5. Wait until key is released.
       6. Return key value.
*/
u32 keyscan(void)
{
    /*
       row
     
       Stores the detected row number.
    */
    u32 row;

    /*
       col
      
       Stores the detected column number.
    */
    u32 col;

    /*
       key
     
       Stores the actual character corresponding
       to the pressed row and column.
    */
    u32 key;

    /*----------------------------------------------------------
                       WAIT FOR KEY PRESS
      ----------------------------------------------------------*/

    /*
       colscan() returns:
     
           1 -> No key pressed.
           0 -> Key pressed.
     
       Therefore the loop continues while no key
       is pressed.
    */
    while(colscan());

    /*----------------------------------------------------------
                          FIND ROW
      ----------------------------------------------------------*/

    /* Determine which row contains the pressed key. */
    row = rowcheck();

    /*----------------------------------------------------------
                         FIND COLUMN
      ----------------------------------------------------------*/

    /* Determine which column contains the pressed key. */
    col = colcheck();

    /*----------------------------------------------------------
                         FIND KEY VALUE
      ----------------------------------------------------------*/

    /*
       Use the detected row and column as indexes
       into the keypad look-up table.
     
       Example:
     
       row = 1
       col = 2
     
       kpmLUT[1][2] = '6'
    */
    key = kpmLUT[row][col];

    /*----------------------------------------------------------
                       WAIT FOR KEY RELEASE
      ----------------------------------------------------------*/

    /*
       Wait until the user releases the key.
     
       colscan() returns 0 while a key is pressed.
     
       Therefore this loop waits until colscan() becomes 1.
 
       This prevents one long key press from being detected repeatedly.
    */
    while(!colscan());

    /* Return the detected key. */
    return key;
}

/*==============================================================
                           readnum()
  =============================================================*/

/*
   Function: readnum()
 
   Purpose:
   Reads multiple numeric digits from the keypad
   and combines them into one integer.
 
   Example:
  
   User presses:
  
       1 -> 2 -> 3 -> +
 
   Function returns:
 
       123
 
   The first non-numeric key terminates the input.
*/
u32 readnum(void)
{
    /*
       num
     
       Stores the final number entered by the user.
     
       Initially:
     
           num = 0
    */
    u32 num = 0;

    /*
       key
     
       Stores one key returned by keyscan().
    */
    u8 key;

    /* Continue reading keys until a non-numeric key is pressed. */
    while(1)
    {
        /* Wait for and read one keypad key. */
        key = keyscan();

        /*
           Check whether the pressed key is a digit.
         
           ASCII:
         
               '0' = 48
               '1' = 49
               ...
               '9' = 57
        */
        if(key >= '0' && key <= '9')
        {
            /*
               Add the newly entered digit to the number.
             
               Formula:
             
                   num = num * 10 + digit
             
               Example:
             
               Existing num = 12
               New key     = '3'
             
               digit = '3' - 48 = 3
             
               num = (12 * 10) + 3
                   = 123
            */
            num = (num * 10) + (key - 48);
        }
        else
        {
            /*
               A non-numeric key was pressed.
             
               Stop reading the number.
            */
            break;
        }
    }

    /* Return the complete number entered by the user. */
    return num;
}

/*
 * Function: ReadNumLCD()
 *
 * Purpose:
 * Reads a multi-digit number from the keypad and
 * displays the entered digits on the LCD.
 *
 * Key functions:
 *
 *     0 to 9 -> Enter digit
 *     B      -> Backspace
 *     C      -> Clear
 *     E      -> Enter / Accept
 *
 *
 * Example:
 *
 *     Press: 1 2 7 4 8
 *
 *     LCD:
 *     12748
 *
 *     Press B
 *
 *     LCD:
 *     1274
 *
 *     Press E
 *
 *     Function returns:
 *     1274
 */
u32 ReadNumLCD(void)
{
    /*
     * Stores the complete number.
     */
    u32 num = 0;

    /*
     * Stores one key received from keypad.
     */
    u8 key;

    /*
     * Stores number of digits currently entered.
     *
     * This is required for Backspace.
     */
    u8 digits = 0;

    /*
     * Stores the LCD position where the next
     * digit will be displayed.
     *
     * Starts from column 0.
     */
    u8 pos = 0;


    /*
     * Clear LCD before starting number entry.
     */
    CmdLCD(GOTO_LINE2_POS0);


    /*
     * Keep reading keys until ENTER is pressed.
     */
    while(1)
    {
        /*
         * Read one key from keypad.
         */
        key = keyscan();


        /*======================================================
                         NUMBER KEY
          ======================================================*/

        /*
         * Check whether the key is between 0 and 9.
         */
        if(key >= '0' && key <= '9')
        {
            /*
             * Add the digit to the number.
             *
             * Example:
             *
             * num = 12
             * key = '7'
             *
             * num = (12 * 10) + 7
             *     = 127
             */
            num = (num * 10) + (key - '0');


            /*
             * Display the pressed digit on LCD.
             */
            LCD_CharXY(1,pos,key);


            /*
             * Move to the next LCD position.
             */
            pos++;


            /*
             * Increase the number of entered digits.
             */
            digits++;
        }


        /*======================================================
                          BACKSPACE
          ======================================================*/

        else if(key == KEY_BACKSPACE)
        {
            /*
             * Backspace is possible only when
             * at least one digit has been entered.
             */
            if(digits > 0)
            {
                /*
                 * Remove the last digit mathematically.
                 *
                 * Example:
                 *
                 * 12748 / 10 = 1274
                 */
                num = num / 10;


                /*
                 * Decrease the digit count.
                 */
                digits--;


                /*
                 * Move LCD cursor back to the
                 * previous digit position.
                 */
                pos--;


                /*
                 * Replace the deleted digit with
                 * a blank space.
                 */
                LCD_CharXY(1,pos,' ');


                /*
                 * Move cursor back to the deleted
                 * position.
                 *
                 * This is useful if another digit
                 * is entered immediately.
                 */
                LCD_GotoXY(1,pos);
            }
        }


        /*======================================================
                            CLEAR
          ======================================================*/

        else if(key == KEY_CLEAR)
        {
            /*
             * Reset the stored number.
             */
            num = 0;


            /*
             * Reset the number of digits.
             */
            digits = 0;


            /*
             * Reset LCD position.
             */
            pos = 0;

						CmdLCD(GOTO_LINE2_POS0);
            /*
             * Clear the LCD.
             */
            StrLCD("                ");
        }


        /*======================================================
                            ENTER
          ======================================================*/

        else if(key == KEY_ENTER)
        {
            /*
             * ENTER means number entry is complete.
             *
             * Exit the loop.
             */
            break;
        }
    }


    /*
     * Return the final number.
     */
    return num;
}
u32 keyscan_nb(void)
{
    u32 row;
    u32 col;

    /* No key pressed */
    if(colscan())
        return 0;

    /* Key pressed */
    row = rowcheck();

    col = colcheck();

    /* Wait for key release */
    while(!colscan());

    return kpmLUT[row][col];
}
