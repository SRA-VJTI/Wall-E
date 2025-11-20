//C Headers
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

//Components
#include "sra_board.h"

void pwm_task(void *arg)
{
	motor_handle_t motor_a_0, motor_a_1;
	ESP_ERROR_CHECK(enable_motor_driver(&motor_a_0, MOTOR_A_0)); // Enable motor driver A0
	ESP_ERROR_CHECK(enable_motor_driver(&motor_a_1, MOTOR_A_1)); // Enable motor driver A1

	// Make the Motors go forward & backward alternatively, at different PWM from 60 to 100
	while (1)
	{
		for (int duty_cycle = 60; duty_cycle <= 100; duty_cycle++)
		{
			// setting motor speed of MOTOR A0 in forward direction with duty cycle
			set_motor_speed(motor_a_0, MOTOR_FORWARD, duty_cycle);

			// setting motor speed of MOTOR A1 in forward direction with duty cycle
			set_motor_speed(motor_a_1, MOTOR_FORWARD, duty_cycle);

			// adding delay of 100ms
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}

		// stopping the MOTOR A0
		set_motor_speed(motor_a_0, MOTOR_STOP, 0);

		// stopping the MOTOR A1
		set_motor_speed(motor_a_1, MOTOR_STOP, 0);

		// adding delay of 100ms
		vTaskDelay(100 / portTICK_PERIOD_MS);

		for (int duty_cycle = 60; duty_cycle <= 100; duty_cycle++)
		{
			// setting motor speed of MOTOR A0 in backward direction with duty cycle
			set_motor_speed(motor_a_0, MOTOR_BACKWARD, duty_cycle);

			// setting motor speed of MOTOR A1 in backward direction with duty cycle
			set_motor_speed(motor_a_1, MOTOR_BACKWARD, duty_cycle);

			// adding delay of 100ms
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}

		// stopping the MOTOR A0
		set_motor_speed(motor_a_0, MOTOR_STOP, 0);

		// stopping the MOTOR A1
		set_motor_speed(motor_a_1, MOTOR_STOP, 0);

		// adding delay of 100ms
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void app_main()
{
	// Basic Function for task creation
	xTaskCreate(&pwm_task, "pwm_task", 4096, NULL, 1, NULL);
}
