// SRA's custom header file including additional functions
#include "sra_board.h"

// pointer to a character array
static const char *TAG_SWITCH_CONTROL_MOTOR = "switch_control";

// main driver function
void drive_task(void *arg)
{
	/**
	 * activate gpio pins SWITCH_1, SWITCH_2, SWITCH_3, SWITCH_4 .. 
	 * swtiches coorespond to following gpio pins
	 * SWITCH_1 : 4
	 * SWITCH_2 : 2
	 * SWITCH_3 : 17
	 * SWITCH_4 : 16
	 * more details can be found in https://github.com/SRA-VJTI/sra-board-component/blob/9f28700759ac816660c18859d65303d6540e8732/src/switches.c#L6-L34 
	 */
	enable_switches();

	/** 
	 * activate motor drivers  TB6612FNG for controlling motors.
	 * The ESP has two TB6612FNG drivers which can run upto 2 motors each, the a refers to the first driver
	 * more details can be found in https://github.com/SRA-VJTI/sra-board-component/blob/9f28700759ac816660c18859d65303d6540e8732/src/motor_driver.c#L7-L57 
	 */
	enable_motor_driver(a, NORMAL_MODE); // Enable motor driver A in Normal Mode

	//infinite loop to get LSA readings continuously
	while (1)
	{
		// check input from LSA and activate the appropriate motor accordingly.

		/**
		 * read_switch() checks if LSA has given any input to the switch and if statement is true if input is given
		 * more information can be found at https://github.com/SRA-VJTI/sra-board-component/blob/9f28700759ac816660c18859d65303d6540e8732/src/switches.c#L36-L46
		 */
		if (read_switch(SWITCH_1))
		{
			/**
			 * runs the first motor A_0 for 80 duty cylces forward or backward according to LSA input
			 * more details can be found at https://github.com/SRA-VJTI/sra-board-component/blob/9f28700759ac816660c18859d65303d6540e8732/src/motor_driver.c#L195
			 */
			set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, 80);

			// Display logs - final LSA readings
			ESP_LOGI(TAG_SWITCH_CONTROL_MOTOR, "MOTOR_A_0 FORWARD");
		}
		else if (read_switch(SWITCH_2))
		{
			set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, 80);
			ESP_LOGI(TAG_SWITCH_CONTROL_MOTOR, "MOTOR_A_0 BACKWARD");
		}
		else
		{
			// if there is no input from  LSA for first motor then stop the motor, this stops the motor if the previous 80 duty cycles are not completed
			set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
		}

		if (read_switch(SWITCH_3))
		{
			/**
			 * runs the second motor A_1 for 80 duty cylces forward or backward according to LSA input
			 * more details can be found at https://github.com/SRA-VJTI/sra-board-component/blob/9f28700759ac816660c18859d65303d6540e8732/src/motor_driver.c#L195
			 */
			set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, 80);

			//Displaying Logs - final LSA readings
			ESP_LOGI(TAG_SWITCH_CONTROL_MOTOR, "MOTOR_A_1 FORWARD");
		}
		else if (read_switch(SWITCH_4))
		{
			set_motor_speed(MOTOR_A_1, MOTOR_BACKWARD, 80);
			ESP_LOGI(TAG_SWITCH_CONTROL_MOTOR, "MOTOR_A_1 BACKWARD");
		}
		else
		{
			//if there is no input from LSA for the second motor then stop the motor
			set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
		}
		/**
		 * Delay the task for a specific amount of freeRTOS ticks
		 * RTOS ticks are a time resolution unit in ESP32, by default 1 RTOS tick will occur every 1ms.
		 * the constant portTICK_PERIOD_MS has a default value of one tick period
		 */
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}
void app_main()
{
	/**
	 * @brief Basic Function for task creation
	 *
	 */
	/**
	 * creates a new task with name "drive_task" and allocates 4096 words of memory to it
	 * for more information about xTaskCreate visit //https://www.freertos.org/a00125.html	
	 */
	xTaskCreate(&drive_task, "drive_task", 4096, NULL, 1, NULL);
}
