#include "StdTypes.h"
#include "MemMap.h"
#include "Utils.h"
#include "UART.h"
#include "DIO_Interface.h"
#include "Sensors.h"
#include "UART_Services.h"
#include "EnvInterface.h"

/*Include OS header file*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "SharedDef.h"
//////////////////////////////////////////////////////////////////////

void T_EnviroInterface(void* ptr)
{
	UART_SendString((u8*)"T_EnviroInterface....\r\n");
	u16 temp =0;
	u8 LightnessRatio=0;

		
	while(1)
	{
		LightnessRatio=LDR_RatioRead();
		temp=LM35_GetTemp();
		if(temp != SHApp.CTemp )
		{
			SHApp.CTemp=temp;
			
			if (SHApp.SysState==SS_MAIN)
			{
				xEventGroupSetBits(egEvents,(E_CTEMP_UPDATE|E_SET_SYSTEM_MANGMENT));
				xSemaphoreGive(bsCheck);
			}
			else if (SHApp.SysState==SS_ALARM)
			{
				xEventGroupSetBits(egEvents,E_SET_SYSTEM_MANGMENT);
				xSemaphoreGive(bsCheck);

			}
		}
		if(LightnessRatio != SHApp.Ldr_Intensity )
		{
			SHApp.Ldr_Intensity=LightnessRatio;				
			if (SHApp.SysState==SS_MAIN)
			{
				xEventGroupSetBits(egEvents,E_LIGHT_INTENSITY_UPDATE|E_SET_SYSTEM_MANGMENT);
				xSemaphoreGive(bsCheck);
			}		
		}
		vTaskDelay(50);
			
	}
		
}
