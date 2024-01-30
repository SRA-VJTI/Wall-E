//C Headers
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

//Components
#include "sra_board.h"

// Select the mode needed by uncommenting its definition
#define MODE NORMAL_MODE
// #define MODE PARALLEL_MODE

void pwm_task(void *arg)
{

	if (MODE == NORMAL_MODE)
	{
		enable_motor_driver(a, NORMAL_MODE); // Enable motor driver A in Normal Mode

		// Make the Motors go forward & backward alternatively, at different PWM from 60 to 100
		while (1)
		{
			for (int duty_cycle = 60; duty_cycle <= 100; duty_cycle++)
			{
				// setting motor speed of MOTOR A0 in forward direction with duty cycle
				set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, duty_cycle);

				// setting motor speed of MOTOR A1 in forward direction with duty cycle
				set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, duty_cycle);

				// adding delay of 100ms
				vTaskDelay(100 / portTICK_PERIOD_MS);
			}

			// stopping the MOTOR A0
			set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);

			// stopping the MOTOR A1
			set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);

			// adding delay of 100ms
			vTaskDelay(100 / portTICK_PERIOD_MS);

			for (int duty_cycle = 60; duty_cycle <= 100; duty_cycle++)
			{
				// setting motor speed of MOTOR A0 in backward direction with duty cycle
				set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, duty_cycle);

				// setting motor speed of MOTOR A1 in backward direction with duty cycle
				set_motor_speed(MOTOR_A_1, MOTOR_BACKWARD, duty_cycle);

				// adding delay of 100ms
				vTaskDelay(100 / portTICK_PERIOD_MS);
			}

			// stopping the MOTOR A0
			set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);

			// stopping the MOTOR A1
			set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);

			// adding delay of 100ms
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}
	}

	else if (MODE == PARALLEL_MODE)
	{
		enable_motor_driver(a, PARALLEL_MODE); // Enable motor driver A in Parallel Mode

		// Make the Motors go forward & backward alternatively, at different PWM from 60 to 100
		while (1)
		{
			for (int duty_cycle = 60; duty_cycle <= 100; duty_cycle++)
			{
				// setting motor speed of motor A in forward direction with duty cycle
				set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, duty_cycle);

				// adding delay of 100ms
				vTaskDelay(100 / portTICK_PERIOD_MS);
			}

			// stopping the motor A
			set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);

			// adding delay of 100ms
			vTaskDelay(100 / portTICK_PERIOD_MS);

			for (int duty_cycle = 60; duty_cycle <= 100; duty_cycle++)
			{
				// setting motor speed of motor A in backward direction with duty cycle
				set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, duty_cycle);

				// adding delay of 100ms
				vTaskDelay(100 / 10);
			}

			// stopping the motor A
			set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);

			// adding delay of 100ms
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}
	}
}

void app_main()
{
	// Basic Function for task creation
	xTaskCreate(&pwm_task, "pwm_task", 4096, NULL, 1, NULL);
}
