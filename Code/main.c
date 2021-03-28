

/*************************** Includes ******************************/
#include "DIO_Interface.h"
#include "LCD.h"
#include "UART.h"#include "Sensors.h"#include "DC_Motor.h"#include "Keypad.h"#include "EEPROM.h"#include "EEPROM_Services.h"#include "WD_Timer.h"#include "Timers.h"#include "UART_Services.h"#include "string.h"
/*Include OS header file*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "SysManagment.h"

TaskHandle_t th_T_EnviroInterface;
TaskHandle_t th_T_DisplayManagment;
TaskHandle_t th_T_Alarm_Appliance;
TaskHandle_t th_T_User_Input ;
TaskHandle_t th_T_SysManagment;

EventGroupHandle_t	egEvents = NULL;
EventGroupHandle_t	egEventsAppliance = NULL;
/*Semaphores*/
xSemaphoreHandle bsCheck;
#include "SharedDef.h"

SHApp_t	SHApp = {SS_PW_CHECK,"0000",0,0,0,0,0,0,0,'A','E'};
///////////////////////////////////////////////////////////////////////////////////
int main(void)
{
	/*Init prephiral*/
	System_Init();
	/*Create OS Objects*/
	bsCheck = xSemaphoreCreateBinary();
	egEvents = xEventGroupCreate();
	egEventsAppliance = xEventGroupCreate();
	/*Create tasks*/
	xTaskCreate(T_DisplayManagment,"T_DisplayManagment",100,NULL,	1,&th_T_DisplayManagment);
	xTaskCreate(T_Alarm_Appliance,"T_Alarm_Appliance",100,NULL,		4,&th_T_Alarm_Appliance);
	xTaskCreate(T_User_Input ,"T_User_Input ",100,NULL,				5,&th_T_User_Input );
	xTaskCreate(T_EnviroInterface,"T_EnviroInterface",100,NULL,		2,&th_T_EnviroInterface);
	xTaskCreate(T_SysManagment,"T_SysManagment",100,NULL,			3,&th_T_SysManagment);
	
	/*OS Start*/
	vTaskStartScheduler();
	return 0;
}

void System_Init(void)
{
	DIO_Init();
	LCD_Init();
	UART_Init();
	ADC_Init(VREF_AVCC,ADC_SCALER_64);
	
	
// 	EEPROM_write(ALARM_STATUS_ADDRESS,'E');
// 	EEPROM_write(LIGHT_MODE_ADDRESS,'A');
// 	EEPROM_write(ACTEMP_ADDRESS,18);
// 	EEPROM_write(LIGHT_INTENSITY_ADDRESS,50);
// 	EEPROM_writeString(PW_ADDRESS,(u8*)"0000");
	
	
	LCD_WriteString((u8*)"      WELCOME      ");
	LCD_SetCursor(1,0);
	LCD_WriteString((u8*)"  (SH Application) ");
	UART_SendString((u8*)"System starting....\r\n");
	SHApp.ACTemp=EEPROM_read(ACTEMP_ADDRESS) ;
	SHApp.Light_Intensity=EEPROM_read(LIGHT_INTENSITY_ADDRESS);
	SHApp.Alarm_Status=EEPROM_read(ALARM_STATUS_ADDRESS);
	SHApp.Light_Mode=EEPROM_read(LIGHT_MODE_ADDRESS);
	EEPROM_ReadString(PW_ADDRESS,(u8*)SHApp.PW,5);
	_delay_ms(1000);
	SHApp_DispScreen(SS_PW_CHECK);
}
