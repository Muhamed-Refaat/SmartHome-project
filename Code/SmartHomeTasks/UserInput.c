#include "StdTypes.h"
#include "MemMap.h"
#include "Utils.h"
#include "DIO_Interface.h"
#include "Keypad.h"
#include "UART.h"
#include "Sensors.h"
#include "UART_Services.h"
#include "UserInput.h"

/*Include OS header file*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "SharedDef.h"

void T_User_Input(void* ptr)
{
	u8 Device1=0,Device2=0;
	u8 input_buf[5];
	UART_SendString((u8*)"T_User_input....\r\n");
	u8 input_ch,uart_ch,keypad_ch,light_temp=0,AC_temp=0;
	while (1)
	{
		keypad_ch=Keypad_GetKey();
		uart_ch=UART_Receive_periodic();
		
		if		(uart_ch!=0)								input_ch=uart_ch;
		else if (keypad_ch!=DEFAULT_KEY)					input_ch=keypad_ch;
		else                                                input_ch=0;
		
		if (input_ch)
		{
			switch (SHApp.SysState)
			{
				case SS_PW_CHECK:
						if (input_ch>='0' && input_ch<='9')
						{
							if (SHApp.digitcounter==0)		memset(input_buf,0,5);
/* mod */					input_buf[SHApp.digitcounter]=input_ch;
							SHApp.digitcounter++;
							SHApp.digitvalue=input_ch;
							xEventGroupSetBits(egEvents,E_INSERT_DIGIT_DISP);
					
					
						}
						if (SHApp.digitcounter>=4) //Without OK
						{	
							
							strcpy((char *)SHApp.PW,(char*)input_buf);	
							xEventGroupSetBits(egEvents,E_PW_CHECK);
							SHApp.digitcounter=0;
							xSemaphoreGive(bsCheck);							
						}
						break;
				case SS_LOCK_STUCK:			vTaskDelay(1);
								break;			
				case SS_LOCK_CONFIG:
						if (SHApp.digitcounter<4 && input_ch>='0' && input_ch<='9')
						{
							if (SHApp.digitcounter==0)		memset(input_buf,0,5);
							/* mod */					input_buf[SHApp.digitcounter]=input_ch;
							SHApp.digitcounter++;
							SHApp.digitvalue=input_ch;
							xEventGroupSetBits(egEvents,E_INSERT_DIGIT_DISP);	
						}
						
						else if (input_ch=='O') //Without OK
						{
							
							strcpy((char *)SHApp.PW,(char*)input_buf);							
							SHApp.digitcounter=0;

						}
						else if (input_ch=='C')
						{
							SHApp.digitcounter=0;
						}
						
						if (input_ch=='O' || input_ch=='C')
						{
							xEventGroupSetBits(egEvents,E_PW_UPDATE);
							xSemaphoreGive(bsCheck);
						}
						

						break;
				
				
				case SS_MAIN:
						if (input_ch=='1')
						{
							SHApp.digitcounter=0;
							SHApp.SysState= SS_LIGHT_CONFIG;
							xEventGroupSetBits(egEvents,E_LIGHT_CONFIG_SCREEN);
						}
				
						else if (input_ch=='2')
						{
							SHApp.digitcounter=0;
							SHApp.SysState= SS_ACTEMP_CONFIG;
							xEventGroupSetBits(egEvents,E_ACTEMP_CONFIG_SCREEN);
						}
						else if (input_ch=='3')
						{
							SHApp.digitcounter=0;
							SHApp.SysState= SS_LOCK_CONFIG;
							xEventGroupSetBits(egEvents,E_LOCK_CONFIG_SCREEN);
						}
						else if (input_ch=='4')
						{
							if(SHApp.Alarm_Status=='E')		SHApp.Alarm_Status='D';
							else							SHApp.Alarm_Status='E';
							xEventGroupSetBits(egEvents,(E_ALARM_STATUS_UPDATE|E_ACTEMP_UPDATE|E_SET_SYSTEM_MANGMENT));
							xSemaphoreGive(bsCheck);
						}
				
						else if (input_ch=='5')
						{
							if(SHApp.Light_Mode=='A')			SHApp.Light_Mode='M';				
							else								SHApp.Light_Mode='A';				
					
							xEventGroupSetBits(egEvents,E_LIGHT_MODE_UPDATE|E_LIGHT_INTENSITY_UPDATE|E_SET_SYSTEM_MANGMENT);
							xSemaphoreGive(bsCheck);
						}
						else if (input_ch == '#')
						{
							xEventGroupSetBits(egEvents,E_RESET_SYSTEM);
							xSemaphoreGive(bsCheck);
						}
										
						else if (input_ch=='X')
						{
							if(Device1==0)			
							{
								xEventGroupSetBits(egEventsAppliance, (E_DEVICE1_ON));
								Device1=1;
							}
							else
							{
								xEventGroupSetBits(egEventsAppliance, (E_DEVICE1_OFF));
								Device1=0;
							}	
						}
						
						else if (input_ch=='Y')
						{
							if(Device2==0)
							{
								xEventGroupSetBits(egEventsAppliance, (E_DEVICE2_ON));
								Device2=1;
							}
							else
							{
								xEventGroupSetBits(egEventsAppliance, (E_DEVICE2_OFF));
								Device2=0;
							}
						}
						
						break;
				
				
				case SS_LIGHT_CONFIG:
						if (SHApp.digitcounter<3 &&(input_ch>='0' && input_ch<='9'))
						{
							if (SHApp.digitcounter==0)				light_temp=0;
							light_temp=(light_temp * 10) + (input_ch - '0');      //Local
							SHApp.digitvalue=input_ch;							 //Global
							SHApp.digitcounter++;
							xEventGroupSetBits(egEvents,E_INSERT_DIGIT_DISP);
						}
				
						else if(input_ch=='O')
						{
							if(light_temp>100)		light_temp=100;
							SHApp.Light_Intensity=light_temp;
							light_temp=0;
							SHApp.digitcounter=0;
							SHApp.Light_Mode='M';
							SHApp.SysState=SS_MAIN;
							xEventGroupSetBits(egEvents,(E_SET_SYSTEM_MANGMENT|E_MAIN_SCREEN | E_LIGHT_MODE_UPDATE | E_LIGHT_INTENSITY_UPDATE | E_CTEMP_UPDATE | E_ACTEMP_UPDATE | E_ALARM_STATUS_UPDATE));
							xSemaphoreGive(bsCheck);
						}
						else if (input_ch=='C')
						{
							light_temp=0;
							SHApp.digitcounter=0;
							SHApp.SysState=SS_MAIN;
							xEventGroupSetBits(egEvents, (E_MAIN_SCREEN | E_LIGHT_MODE_UPDATE | E_LIGHT_INTENSITY_UPDATE | E_CTEMP_UPDATE | E_ACTEMP_UPDATE | E_ALARM_STATUS_UPDATE));
					
						}
						break;
						
				
				case SS_ACTEMP_CONFIG:
						if (SHApp.digitcounter<2 &&(input_ch>='0' && input_ch<='9'))
						{
							if (SHApp.digitcounter==0)				AC_temp=0;
							AC_temp=(AC_temp * 10) + (input_ch - '0');
							SHApp.digitvalue=input_ch;
							SHApp.digitcounter++;
							xEventGroupSetBits(egEvents,E_INSERT_DIGIT_DISP);
						}
						
						else if(input_ch=='O')
						{
							SHApp.ACTemp=AC_temp;
							AC_temp=0;
							SHApp.digitcounter=0;
							SHApp.SysState= SS_MAIN;
							xEventGroupSetBits(egEvents,(E_SET_SYSTEM_MANGMENT|E_MAIN_SCREEN | E_LIGHT_MODE_UPDATE | E_LIGHT_INTENSITY_UPDATE | E_CTEMP_UPDATE | E_ACTEMP_UPDATE | E_ALARM_STATUS_UPDATE));
							xSemaphoreGive(bsCheck);
						}
						else if (input_ch=='C')
						{
							AC_temp=0;
							SHApp.digitcounter=0;
							SHApp.SysState = SS_MAIN;
							xEventGroupSetBits (egEvents,(E_MAIN_SCREEN | E_LIGHT_MODE_UPDATE | E_LIGHT_INTENSITY_UPDATE | E_CTEMP_UPDATE | E_ACTEMP_UPDATE | E_ALARM_STATUS_UPDATE));
							
						}
						break;


				case SS_ALARM:
						if (input_ch=='C')
						{
							SHApp.SysState=SS_MAIN;
							SHApp.Alarm_Status='D';
							xEventGroupSetBits( egEvents,(E_SET_SYSTEM_MANGMENT|E_MAIN_SCREEN | E_LIGHT_MODE_UPDATE | E_LIGHT_INTENSITY_UPDATE | E_CTEMP_UPDATE | E_ACTEMP_UPDATE | E_ALARM_STATUS_UPDATE));
							xSemaphoreGive(bsCheck);
						}
						break;
				
				
				default:
						break;
			}
			
		}
		vTaskDelay(10);
	}
	
}