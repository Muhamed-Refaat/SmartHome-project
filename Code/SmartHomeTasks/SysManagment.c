#include "StdTypes.h"
#include "MemMap.h"
#include "Utils.h"
#include "UART.h"
#include "EEPROM.h"
#include "EEPROM_Services.h"
#include "UART_Services.h"
#include "SysManagment.h"

/*Include OS header file*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "SharedDef.h"

void T_SysManagment (void* ptr)
{
	EventBits_t ebBits =0;
	u8 ErrorCounter = 0;
	u8 LastPw[20];
	u8 Last_PwLength = 5;
	strncpy((char *)LastPw , (char *)SHApp.PW, Last_PwLength);
	u8 Last_Ldr_Intensity = SHApp.Ldr_Intensity;
	u8 Last_Light_Intensity = SHApp.Light_Intensity;
	u8 Last_Light_Mode = SHApp.Light_Mode;
	u8 Last_ACTemp = SHApp.ACTemp;
	u8 Last_Alarm_Status = SHApp.Alarm_Status;
	UART_SendString((u8*)"T_SysManagment....\r\n");

	while (1)
	{
	
		if(xSemaphoreTake(bsCheck, portMAX_DELAY))
		{					
			ebBits = xEventGroupWaitBits(egEvents, ( E_SET_SYSTEM_MANGMENT |E_PW_CHECK | E_RESET_SYSTEM | E_PW_UPDATE) ,1, 0, portMAX_DELAY); //E_ALARM
			if(SS_PW_CHECK == SHApp.SysState)
			{
				if(ebBits&E_PW_CHECK)
				{
					if(!strncmp((char *)SHApp.PW , (char *)LastPw , Last_PwLength))
					{
		
						/*3lshan al system y3mel check 3la al temp awel ma yft7 */
						if (SHApp.CTemp>65 && SHApp.Alarm_Status=='E')
						{
							SHApp.SysState=SS_ALARM;
							xEventGroupSetBits(egEvents,E_ALARM_SCREEN);
							xEventGroupSetBits(egEventsAppliance,E_ALARM);
						}
						else
						{
							SHApp.SysState = SS_MAIN;
							xEventGroupSetBits(egEvents, (E_MAIN_SCREEN | E_LIGHT_MODE_UPDATE | E_LIGHT_INTENSITY_UPDATE | E_CTEMP_UPDATE | E_ACTEMP_UPDATE | E_ALARM_STATUS_UPDATE));
							if (SHApp.CTemp > SHApp.ACTemp)			xEventGroupSetBits(egEventsAppliance,E_OPEN_LOCK|E_ADJUSTLIHTING|E_STARTFAN);
							else									xEventGroupSetBits(egEventsAppliance,E_OPEN_LOCK|E_ADJUSTLIHTING|E_STARTHEATER);
							
						}			
						ErrorCounter = 0;
						
					}
					else
					{
						// SHS->SystemState = SS_PW_MISS
						if(ErrorCounter >= NUM_OF_TRIALS)
						{
							SHApp.SysState=SS_LOCK_STUCK;
							xEventGroupSetBits(egEvents, E_DOOR_LOCKED_SCREEN);
							xEventGroupSetBits(egEventsAppliance,E_ALARM);
							vTaskDelay(10000);
							ErrorCounter=0;	
							SHApp.SysState=SS_PW_CHECK;
							xEventGroupClearBits(egEventsAppliance, E_ALARM);
							xEventGroupSetBits  (egEvents, E_CHECK_PW_SCREEN);
						}
						else
						{
							ErrorCounter++;
							xEventGroupSetBits(egEvents, E_WRONG_PW_SCREEN);
						}
					}
				}
			}
			
			else if(SS_LOCK_CONFIG == SHApp.SysState)
			{
				if(ebBits&E_PW_UPDATE)
				{
					// Last_PwLength = SHApp.CurrentPwLength;
					// EEPROM_Write(Last_PwLength);
					strncpy((char *)LastPw , (char *)SHApp.PW, Last_PwLength);
					EEPROM_writeString(PW_ADDRESS , LastPw); //4->16
					SHApp.SysState = SS_PW_CHECK;
					xEventGroupSetBits(egEvents, E_CHECK_PW_SCREEN);
					
				}
			}
			
			
			else if(SS_ALARM == SHApp.SysState)
			{
				if((SHApp.CTemp < FIRE_ALARM_TSHOLD) || SHApp.Alarm_Status == 'D')				/***************************************/
				{
					SHApp.SysState=SS_MAIN;
					xEventGroupSetBits( egEvents,(E_MAIN_SCREEN | E_LIGHT_MODE_UPDATE | E_LIGHT_INTENSITY_UPDATE | E_CTEMP_UPDATE | E_ACTEMP_UPDATE | E_ALARM_STATUS_UPDATE));
					xEventGroupClearBits(egEventsAppliance,E_ALARM);
					//xSemaphoreGive(bsCheck);
				}
			}
			/* enviro*/
			else if(SS_MAIN == SHApp.SysState)
			{
				if(SHApp.Alarm_Status == 'E' && SHApp.CTemp > FIRE_ALARM_TSHOLD )
				{
					SHApp.SysState = SS_ALARM;
					xEventGroupSetBits(egEvents, E_ALARM_SCREEN);
				   xEventGroupSetBits(egEventsAppliance,(E_ALARM|E_DEVICE1_OFF|E_DEVICE2_OFF));
				   

				}
				
				if(ebBits&E_RESET_SYSTEM)
				{
					SHApp.SysState = SS_PW_CHECK;
					xEventGroupSetBits(egEventsAppliance, E_CLOSE_LOCK);
					xEventGroupSetBits  (egEvents, E_CHECK_PW_SCREEN); 
				}
				if(Last_ACTemp != SHApp.ACTemp)
				{
					Last_ACTemp = SHApp.ACTemp;
					EEPROM_write(ACTEMP_ADDRESS , Last_ACTemp);
				}
				if(Last_Alarm_Status != SHApp.Alarm_Status)
				{
					if('D' == SHApp.Alarm_Status)
					{
						xEventGroupClearBits(egEventsAppliance , E_ALARM);
					}
					Last_Alarm_Status = SHApp.Alarm_Status;
					EEPROM_write(ALARM_STATUS_ADDRESS , Last_Alarm_Status);
				}
				/*Light*/
				
				if(Last_Light_Mode != SHApp.Light_Mode)
				{
					Last_Light_Mode = SHApp.Light_Mode;
					EEPROM_write(LIGHT_MODE_ADDRESS , Last_Light_Mode);
					xEventGroupSetBits(egEventsAppliance, E_ADJUSTLIHTING);
				}
				if(Last_Light_Intensity != SHApp.Light_Intensity)
				{
					Last_Light_Intensity = SHApp.Light_Intensity;
					EEPROM_write(LIGHT_INTENSITY_ADDRESS , SHApp.Light_Intensity);
					xEventGroupSetBits(egEventsAppliance, E_ADJUSTLIHTING);
				}
				if(Last_Ldr_Intensity != SHApp.Ldr_Intensity)
				{
					Last_Ldr_Intensity = SHApp.Ldr_Intensity;
					xEventGroupSetBits(egEventsAppliance, E_ADJUSTLIHTING);
				}
				
				/* Fan speed Range */
				if(SHApp.CTemp < SHApp.ACTemp-ACTEMP_RANGE)	// && flag == 1)
				{
					xEventGroupSetBits(egEventsAppliance, E_STARTHEATER);
					xEventGroupSetBits(egEventsAppliance, E_STOPFAN);
				}
				else if(SHApp.CTemp > SHApp.ACTemp+ACTEMP_RANGE)	//	&& flag == 0)
				{
					
					if (SHApp.CTemp>FIRE_ALARM_TSHOLD)
					{
						
						xEventGroupSetBits(egEventsAppliance, E_STOPHEATER|E_STOPFAN|E_OPEN_LOCK);
					
					}
					xEventGroupSetBits(egEventsAppliance, E_STOPHEATER);
					xEventGroupSetBits(egEventsAppliance, E_STARTFAN);
				}
				else
				{
					//SHApp.Fspeed = 0;
					xEventGroupSetBits(egEventsAppliance,E_STOPFAN|E_STOPHEATER);
				}
				/*Temp*/
			}
		}
	}
}
