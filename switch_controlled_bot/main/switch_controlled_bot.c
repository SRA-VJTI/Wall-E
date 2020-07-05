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

	enable_buttons();	//Enable the buttons connected to GPIO 2 and GPIO 5
	mcpwm_initialize();	//Initialize PWM to control speed of motors

	while(1)
	{

		/*
			Make the Bot go forward, at a PWM of 80 if button 1 is pressed
		*/
		if(pressed_switch(BUTTON_1))
		{
			
			printf("%s\n","BOT FORWARD");

			bot_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, 80, 80);	
		}

		/*
			Make the Bot go backward, at a PWM of 80 if button 1 is pressed
		*/
		else if(pressed_switch(BUTTON_2))
		{
			
			printf("%s\n","BOT BACKWARD");  

			bot_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, 80, 80);	//Make the Bot go backward, at a PWM of 80 if button 2 is pressed
		}

		/*
			If no buttons are pressed, stop the bot
		*/

	   	else 
		{
			bot_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
			printf("%s\n","BOT STOPPED");
		}

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
