//C Headers
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

//Components
#include "sra18.h"

void drive_task(void *arg)
{

	mcpwm_initialize();	//Initialize PWM to control speed of motors

	while(1)
	{

		/*
			Make the Bot go forward & backward alternatively, at different PWM from 60 to 100
		*/
		
		for(int i = 60; i <= 100; i++)
		{
			bot_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, i, i);
			vTaskDelay(100 / 10);
		}

		bot_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
		vTaskDelay(100 / 10);
		
		for(int i = 60; i <= 100; i++)
		{
			bot_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, i, i);
			vTaskDelay(100 / 10);
		}

		bot_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
		vTaskDelay(100 / 10);	
	}
	
}

void app_main()
{
	/*
		Basic Function for task creation
	*/
    xTaskCreate(&drive_task,"drive_task",4096,NULL,1,NULL);
}
