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

	enable_buttons();	//Enable the buttons connected to GPIO 39, 36 35 and 34 respectively
	mcpwm_initialize();	//Initialize PWM to control speed of motors

	while(1)
	{

		/*
			Motors 1 and 2 spin clockwise, at a PWM of 80 if button 1 is pressed
		*/
		if(pressed_switch(BUTTON_1))
		{
			
			printf("%s\n","MOTORS 1 AND 2 CLOCKWISE");

			motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, 80);	
			motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_1, 80);	
		}

		/*
			Motors 1 and 2 spin anticlockwise, at a PWM of 80 if button 2 is pressed
		*/
		else if(pressed_switch(BUTTON_2))
		{
			
			printf("%s\n","MOTORS 1 AND 2 ANTICLOCKWISE");  

			motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, 80);	
			motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_1, 80);	
		}

        /*
			Motors 3 and 4 spin clockwise, at a PWM of 80 if button 3 is pressed
		*/
		else if(pressed_switch(BUTTON_3))
		{
			
			printf("%s\n","MOTORS 3 AND 4 CLOCKWISE");  

			motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_2, 80);	
			motor_forward(MCPWM_UNIT_1, MCPWM_TIMER_0, 80);	
		}

		/*
			Motors 3 and 4 spin anticlockwise, at a PWM of 80 if button 4 is pressed
		*/
		else if(pressed_switch(BUTTON_4))
		{
			
			printf("%s\n","MOTORS 3 AND 4 ANTICLOCKWISE");  

			motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_2, 80);	
			motor_backward(MCPWM_UNIT_1, MCPWM_TIMER_0, 80);	
		}
		
        /*
			If no buttons are pressed, stop the motors
		*/
	   	else 
		{
			motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
			motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_1);
			motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_2);
			motor_stop(MCPWM_UNIT_1, MCPWM_TIMER_0);
			printf("%s\n","MOTORS STOPPED");
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
