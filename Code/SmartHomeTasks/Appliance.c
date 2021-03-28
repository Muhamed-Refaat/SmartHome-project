#include "StdTypes.h"
#include "MemMap.h"
#include "Utils.h"
#include "DIO_Interface.h"
#include "UART.h"
#include "UART_Services.h"
#include "Appliance.h"

/*Include OS header file*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "SharedDef.h"
////////////////////////////////Light Adjustment function///////////////////////////////////////////////////////////
void OUTPUT_Light_ADjustment(void)
{
	if(SHApp.Light_Mode=='M')
	{
		if((SHApp.Light_Intensity>=0)&&(SHApp.Light_Intensity<10))
		{
			DIO_WritePin(LIGHT1,LOW);
			DIO_WritePin(LIGHT2,LOW);
			DIO_WritePin(LIGHT3,LOW);
			DIO_WritePin(LIGHT4,LOW);
			DIO_WritePin(LIGHT5,LOW);
		}
		else if((SHApp.Light_Intensity>=10)&&(SHApp.Light_Intensity<30))
		{
			DIO_WritePin(LIGHT1,HIGH);
			DIO_WritePin(LIGHT2,LOW);
			DIO_WritePin(LIGHT3,LOW);
			DIO_WritePin(LIGHT4,LOW);
			DIO_WritePin(LIGHT5,LOW);
		}
		else if((SHApp.Light_Intensity>=30)&&(SHApp.Light_Intensity<50))
		{
			DIO_WritePin(LIGHT1,HIGH);
			DIO_WritePin(LIGHT2,LOW);
			DIO_WritePin(LIGHT3,HIGH);
			DIO_WritePin(LIGHT4,LOW);
			DIO_WritePin(LIGHT5,LOW);
		}
		else if((SHApp.Light_Intensity>=50)&&(SHApp.Light_Intensity<70))
		{
			DIO_WritePin(LIGHT1,HIGH);
			DIO_WritePin(LIGHT2,LOW);
			DIO_WritePin(LIGHT3,HIGH);
			DIO_WritePin(LIGHT4,LOW);
			DIO_WritePin(LIGHT5,HIGH);
		}
		else if((SHApp.Light_Intensity>=70)&&(SHApp.Light_Intensity<90))
		{
			DIO_WritePin(LIGHT1,HIGH);
			DIO_WritePin(LIGHT2,HIGH);
			DIO_WritePin(LIGHT3,HIGH);
			DIO_WritePin(LIGHT4,LOW);
			DIO_WritePin(LIGHT5,HIGH);
		}
		else if((SHApp.Light_Intensity>=90)&&(SHApp.Light_Intensity<=100))
		{
			DIO_WritePin(LIGHT1,HIGH);
			DIO_WritePin(LIGHT2,HIGH);
			DIO_WritePin(LIGHT3,HIGH);
			DIO_WritePin(LIGHT4,HIGH);
			DIO_WritePin(LIGHT5,HIGH);
		}
	}
	else if(SHApp.Light_Mode=='A')
	{
		if((SHApp.Ldr_Intensity>=0)&&(SHApp.Ldr_Intensity<10))
		{
			DIO_WritePin(LIGHT1,LOW);
			DIO_WritePin(LIGHT2,LOW);
			DIO_WritePin(LIGHT3,LOW);
			DIO_WritePin(LIGHT4,LOW);
			DIO_WritePin(LIGHT5,LOW);
		}
		else if((SHApp.Ldr_Intensity>=10)&&(SHApp.Ldr_Intensity<30))
		{
			DIO_WritePin(LIGHT1,HIGH);
			DIO_WritePin(LIGHT2,LOW);
			DIO_WritePin(LIGHT3,LOW);
			DIO_WritePin(LIGHT4,LOW);
			DIO_WritePin(LIGHT5,LOW);
		}
		else if((SHApp.Ldr_Intensity>=30)&&(SHApp.Ldr_Intensity<50))
		{
			DIO_WritePin(LIGHT1,HIGH);
			DIO_WritePin(LIGHT2,LOW);
			DIO_WritePin(LIGHT3,HIGH);
			DIO_WritePin(LIGHT4,LOW);
			DIO_WritePin(LIGHT5,LOW);
		}
		else if((SHApp.Ldr_Intensity>=50)&&(SHApp.Ldr_Intensity<70))
		{
			DIO_WritePin(LIGHT1,HIGH);
			DIO_WritePin(LIGHT2,LOW);
			DIO_WritePin(LIGHT3,HIGH);
			DIO_WritePin(LIGHT4,LOW);
			DIO_WritePin(LIGHT5,HIGH);
		}
		else if((SHApp.Ldr_Intensity>=70)&&(SHApp.Ldr_Intensity<90))
		{
			DIO_WritePin(LIGHT1,HIGH);
			DIO_WritePin(LIGHT2,HIGH);
			DIO_WritePin(LIGHT3,HIGH);
			DIO_WritePin(LIGHT4,LOW);
			DIO_WritePin(LIGHT5,HIGH);
		}
		else if((SHApp.Ldr_Intensity>=90)&&(SHApp.Ldr_Intensity<=100))
		{
			DIO_WritePin(LIGHT1,HIGH);
			DIO_WritePin(LIGHT2,HIGH);
			DIO_WritePin(LIGHT3,HIGH);
			DIO_WritePin(LIGHT4,HIGH);
			DIO_WritePin(LIGHT5,HIGH);
		}
	}
}

///////////////////////////////Appliance task Implementation  /////////////////////////////////////////////////////////////

void T_Alarm_Appliance(void* ptr)
{
	UART_SendString((u8*)"T_Alarm_Appliance....\r\n");
    EventBits_t ebBits= 0;
	u8 warning_flag=WARNING_UNSENT;
    while (1)
    {
        ebBits= xEventGroupWaitBits(egEventsAppliance,( E_ALARM | E_STOPFAN| E_STARTFAN|E_STOPHEATER | E_STARTHEATER|E_OPEN_LOCK|E_CLOSE_LOCK  | E_ADJUSTLIHTING | E_DEVICE1_OFF | E_DEVICE1_ON | E_DEVICE2_OFF | E_DEVICE2_ON ),0,0,portMAX_DELAY);
		
        if (ebBits & (E_ALARM))                      
        {
			if (warning_flag==WARNING_UNSENT)
			{
				UART_SendString((u8*)"\r\nWarning Alarm....\r\nWarning Alarm....\r\n");
				warning_flag=WARNING_SENT;
			}
			DIO_WritePin(ALARM_BUZZER,HIGH);
			DIO_WritePin(ALARM_LED,HIGH);
            vTaskDelay(ALARM_Delay);
            
            DIO_WritePin(ALARM_BUZZER,LOW);
			DIO_WritePin(ALARM_LED,LOW);
            vTaskDelay(ALARM_Delay);
        }
		else 
		{
			 warning_flag=WARNING_UNSENT;
		}
		
        if(ebBits & (E_OPEN_LOCK))
        {
                DIO_WritePin(DOOR_LOCK,HIGH);
				xEventGroupClearBits(egEventsAppliance,(E_OPEN_LOCK));
		}
        else if(ebBits&(E_CLOSE_LOCK))
        {
               DIO_WritePin(DOOR_LOCK,LOW);
			   xEventGroupClearBits(egEventsAppliance,(E_CLOSE_LOCK));
        }
           
        if(ebBits & (E_STARTFAN))
        {
           DIO_WritePin(AC_FAN,HIGH);
		   xEventGroupClearBits(egEventsAppliance,(E_STARTFAN));
        }
		else if(ebBits & (E_STOPFAN))
		{
			DIO_WritePin(AC_FAN,LOW);
		    xEventGroupClearBits(egEventsAppliance,(E_STOPFAN));
		}

        if(ebBits & (E_STARTHEATER))
        {
                DIO_WritePin(HEATER,HIGH);
                xEventGroupClearBits(egEventsAppliance,(E_STARTHEATER));
         }
			
			
        else if(ebBits & (E_STOPHEATER))
        {
                DIO_WritePin(HEATER,LOW);
              xEventGroupClearBits(egEventsAppliance,(E_STOPHEATER));
        }
		    	
        if(ebBits & (E_ADJUSTLIHTING))
        {
            OUTPUT_Light_ADjustment();
			xEventGroupClearBits(egEventsAppliance,(E_ADJUSTLIHTING));	
        }
		if(ebBits & (E_DEVICE1_ON))
		{
			DIO_WritePin(DEVICE1,HIGH);
			xEventGroupClearBits(egEventsAppliance,(E_DEVICE1_ON));
		}
		else if(ebBits & (E_DEVICE1_OFF))
		{
			DIO_WritePin(DEVICE1,LOW);
			xEventGroupClearBits(egEventsAppliance,(E_DEVICE1_OFF));
		}
		if(ebBits & (E_DEVICE2_ON))
		{
			DIO_WritePin(DEVICE2,HIGH);
			xEventGroupClearBits(egEventsAppliance,(E_DEVICE2_ON));
		}
		else if(ebBits & (E_DEVICE2_OFF))
		{
			DIO_WritePin(DEVICE2,LOW);
			xEventGroupClearBits(egEventsAppliance,(E_DEVICE2_OFF));
		}
    }
}