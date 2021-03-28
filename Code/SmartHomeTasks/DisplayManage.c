#include "StdTypes.h"
#include "MemMap.h"
#include "Utils.h"
#include "DIO_Interface.h"
#include "LCD.h"
#include "UART.h"
#include "UART_Services.h"
#include "DisplayManage.h"

/*Include OS header file*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "SharedDef.h"
////////////////////////////////////////////////////////////////////
void T_DisplayManagment(void* ptr)
{
	UART_SendString((u8*)"T_DisplayManagment....\r\n");
	EventBits_t			ebBits = 0;
	while (1)
	{
		ebBits = xEventGroupWaitBits(egEvents,(E_DISP_EVENTS),1,0,portMAX_DELAY); // Auto clear ,either or
		
		if(ebBits&E_CHECK_PW_SCREEN){
			SHApp_DispScreen(SS_PW_CHECK);
		}
		else if(ebBits&E_WRONG_PW_SCREEN){
			SHApp_DispScreen(SS_PW_MISS);
			vTaskDelay(500);
			SHApp_DispScreen(SS_PW_CHECK);				/***************************************/
		}
		else if(ebBits&E_DOOR_LOCKED_SCREEN){
			SHApp_DispScreen(SS_LOCK_STUCK);
		}
		else if(ebBits&E_MAIN_SCREEN){
			SHApp_DispScreen(SS_MAIN);
		}
		else if(ebBits&E_LIGHT_CONFIG_SCREEN){
			SHApp_DispScreen(SS_LIGHT_CONFIG);
		}
		else if(ebBits&E_ACTEMP_CONFIG_SCREEN){
			SHApp_DispScreen(SS_ACTEMP_CONFIG);
		}
		else if(ebBits&E_LOCK_CONFIG_SCREEN){
			SHApp_DispScreen(SS_LOCK_CONFIG);
		}
		else if(ebBits&E_ALARM_SCREEN){
			SHApp_DispScreen(SS_ALARM);
		}
		if(ebBits&E_LIGHT_MODE_UPDATE){
			LCD_WriteCharXY(DISP_LIGHT_MODE_L,DISP_LIGHT_MODE_C,SHApp.Light_Mode);
		}
		if(ebBits&E_LIGHT_INTENSITY_UPDATE && SHApp.Light_Mode=='A'){
			LCD_WriteStringXY(DISP_LIGHT_INTENSITY_L,DISP_LIGHT_INTENSITY_C, (u8 *)"   ");
			LCD_WriteNumXY(DISP_LIGHT_INTENSITY_L,DISP_LIGHT_INTENSITY_C,SHApp.Ldr_Intensity);
			
		}
		if(ebBits&E_LIGHT_INTENSITY_UPDATE && SHApp.Light_Mode=='M'){
			LCD_WriteStringXY(DISP_LIGHT_INTENSITY_L,DISP_LIGHT_INTENSITY_C, (u8 *)"   ");
			LCD_WriteNumXY(DISP_LIGHT_INTENSITY_L,DISP_LIGHT_INTENSITY_C,SHApp.Light_Intensity);
		}
		if(ebBits&E_CTEMP_UPDATE){
			LCD_WriteStringXY(DISP_CTEMP_L,DISP_CTEMP_C, (u8 *)"   ");
			LCD_WriteNumXY(DISP_CTEMP_L,DISP_CTEMP_C,SHApp.CTemp);
		}
		if(ebBits&E_ACTEMP_UPDATE){
			LCD_WriteNumXY(DISP_ACTEMP_L,DISP_ACTEMP_C,SHApp.ACTemp);
		}
		if(ebBits&E_ALARM_STATUS_UPDATE){
			LCD_WriteCharXY(DISP_ALARM_STATUS_L,DISP_ALARM_STATUS_C,SHApp.Alarm_Status);
		}
		if((ebBits&E_INSERT_DIGIT_DISP) && (SHApp.SysState==SS_PW_CHECK || SHApp.SysState==SS_PW_MISS||SHApp.SysState==SS_LOCK_CONFIG)){
			LCD_WriteCharXY(DISP_PW_L, (DISP_PW_C +(SHApp.digitcounter-1)*2),'*');					///
			if (SHApp.digitcounter==4)		vTaskDelay(500);
		}
		else if((ebBits&E_INSERT_DIGIT_DISP) && (SHApp.SysState==SS_LIGHT_CONFIG)){
			LCD_WriteCharXY(DISP_INSERT_LIGHT_INTISITY_L, (DISP_INSERT_LIGHT_INTISITY_C +SHApp.digitcounter),SHApp.digitvalue);
		}
		else if((ebBits&E_INSERT_DIGIT_DISP) && (SHApp.SysState==SS_ACTEMP_CONFIG)){
			LCD_WriteCharXY(DISP_INSERT_ACTEMP_L, (DISP_INSERT_ACTEMP_C +SHApp.digitcounter), SHApp.digitvalue);
		}
		
		
	}
}

/* Display Static Screens Function uncompleted*/

void SHApp_DispScreen(SystemState_t state){
	LCD_ClearScreen();
	switch (state)
	{
		case SS_PW_CHECK:
		LCD_WriteStringXY(0,0,DISP_PW_CHECK_L0);
		break;
		
		case SS_PW_MISS:
		LCD_WriteStringXY(1,0,DISP_PW_MISS_L0);
		break;
		
		case SS_LOCK_STUCK:
		LCD_WriteStringXY(0,0,DISP_LOCK_STUCK_L0);
		LCD_WriteStringXY(1,0,DISP_LOCK_STUCK_L1);
		LCD_WriteStringXY(3,0,DISP_LOCK_STUCK_L3);
		break;
		
		case SS_MAIN:
		LCD_WriteStringXY(0,0,DISP_MAIN_L0);
		LCD_WriteStringXY(1,0,DISP_MAIN_L1);
		LCD_WriteStringXY(2,0,DISP_MAIN_L2);
		LCD_WriteStringXY(3,0,DISP_MAIN_L3);
		break;
		
		case SS_LIGHT_CONFIG:
		LCD_WriteStringXY(1,0,DISP_LIGHT_INT_L1);
		LCD_WriteStringXY(2,0,DISP_CONFIRM_L2);
		break;
		
		case SS_ACTEMP_CONFIG:
		LCD_WriteStringXY(1,0,DISP_ACTEMP_L1);
		LCD_WriteStringXY(2,0,DISP_CONFIRM_L2);
		break;
		
		case SS_LOCK_CONFIG:
		LCD_WriteStringXY(0,0,DISP_PW_NEW_L0);
		LCD_WriteStringXY(3,0,DISP_PW_NEW_L3);
		break;
		
		case SS_ALARM:
		LCD_WriteStringXY(1,0,DISP_ALARM_L1);
		LCD_WriteStringXY(2,0,DISP_ALARM_L2);
		break;
	}
}
