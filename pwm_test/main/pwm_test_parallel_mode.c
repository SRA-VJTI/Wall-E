//C Headers
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

//Components
#include "motor_driver.h"

void drive_task(void *arg)
{

	enable_motor_driver_a(1); // Enable motor driver A in Parallel Mode
	enable_motor_driver_b(1); // Enable motor driver B in Parallel Mode

	while(1)
	{

		/*
			Make the Motors go forward & backward alternatively, at different PWM from 60 to 100
		*/
		
		for(int duty_cycle = 60; duty_cycle <= 100; duty_cycle++)
		{
			set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, duty_cycle);
			set_motor_speed(MOTOR_B_0, MOTOR_FORWARD, duty_cycle);
			vTaskDelay(100 / 10);
		}

		set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
		set_motor_speed(MOTOR_B_0, MOTOR_STOP, 0);
		vTaskDelay(100 / 10);
		
		for(int duty_cycle = 60; duty_cycle <= 100; duty_cycle++)
		{
			set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, duty_cycle);
			set_motor_speed(MOTOR_B_0, MOTOR_BACKWARD, duty_cycle);
			vTaskDelay(100 / 10);
		}

		set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
		set_motor_speed(MOTOR_B_0, MOTOR_STOP, 0);
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
