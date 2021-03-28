#ifndef DISPLAY_MANAGE_H_
#define DISPLAY_MANAGE_H_

///////////////////////////////////////////////////////////////////////////////////////////

/* Display Macros  Static Screens */
#define CLR_LINE(LINE)				LCD_WriteStringXY(LINE,0,"                    ")

#define WELCOME_Scr					(u8*)"    Welcome Home    "


#define DISP_PW_CHECK_L0			(u8*)"   Enter Password   "
#define DISP_PW_MISS_L0				(u8*)"   !! Try Again !!  "
#define DISP_LOCK_STUCK_L0			(u8*)" !! SYSTEM LOCKD !! "
#define DISP_LOCK_STUCK_L1			(u8*)" !!   ALARMING   !! "
#define DISP_LOCK_STUCK_L3			(u8*)"WAIT FOR 10 SECONDS "

#define DISP_MAIN_L0				(u8*)"CT:    AC:   LI:   %"
#define DISP_MAIN_L1				(u8*)"   AS:      LM:     "
#define DISP_MAIN_L2				(u8*)" LA:1   TA:2   SS:3 "
#define DISP_MAIN_L3				(u8*)" AST:4  LMT:5	 SL:# "


#define DISP_LIGHT_INT_L1			(u8*)"Light Intensity:   %"
//#define DISP_CONFIRM_L2			(u8*)"  O:OK	C:Cancel  "

#define DISP_ACTEMP_L1				(u8*)"AC Temperture:    C "

#define DISP_CONFIRM_L2				(u8*)"  O:OK    C:Cancel  "

#define DISP_PW_NEW_L0				(u8*)" Enter NEW Password "
#define DISP_PW_NEW_L3				(u8*)"  O:SAVE	C:Cancel  "

#define DISP_ALARM_L1				(u8*)" !!   ALARMING   !! "
#define DISP_ALARM_L2				(u8*)"    C:STOP ALARM    "

/* Dynamic Screens */
#define DISP_LIGHT_INTENSITY_L					0
#define DISP_LIGHT_INTENSITY_C					16

#define DISP_CTEMP_L							0
#define DISP_CTEMP_C							3

#define DISP_ACTEMP_L							0
#define DISP_ACTEMP_C							10

#define DISP_ALARM_STATUS_L						1
#define DISP_ALARM_STATUS_C						6

#define DISP_LIGHT_MODE_L						1
#define DISP_LIGHT_MODE_C						16

#define DISP_PW_L								1
#define DISP_PW_C								6

#define DISP_INSERT_LIGHT_INTISITY_L			1
#define DISP_INSERT_LIGHT_INTISITY_C			15

#define DISP_INSERT_ACTEMP_L					1
#define DISP_INSERT_ACTEMP_C					14


#endif	// DISPLAY_MANAGE_H_