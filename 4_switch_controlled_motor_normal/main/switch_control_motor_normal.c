#include "sra_board.h"

static const char* TAG_SWITCH_CONTROL_MOTOR = "switch_control";

void drive_task(void *arg)
{
	/*
		Function to operate motors a/c to the sensor input
	*/
	/*	SWITCH_1 4
		SWITCH_2 2
		SWITCH_3 17
		SWITCH_4 16
	*/
	
	// Asks ESP to take input at gpio pins{SWITCH_1, SWITCH_2, SWITCH_3, SWITCH_4} 
	enable_switches();   
	
	/*
		enables 1 TB6612FNG motor driver for controlling motors, in normal mode.
	*/
	enable_motor_driver(a, NORMAL_MODE);  // Enable motor driver A in Normal Mode


// 	ESP_LOG: 
//    ESP_LOGE - error (lowest)
//    ESP_LOGW - warning
//    ESP_LOGI - info
//    ESP_LOGD - debug
//    ESP_LOGV - verbose (highest)

	
	//infinite loop to take input from the user
	while(1)
	{	//loop through the switches and take input from the sensor.
		
		if(read_switch(SWITCH_1))
		{
			//esp_err_t set_motor_speed(int motor_id, int direction, float duty_cycle)  . . moves the related motor.
			set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, 80);
			
			//logs the information in the ESP log.
			ESP_LOGI(TAG_SWITCH_CONTROL_MOTOR, "MOTOR_A_0 FORWARD");	
		}
		else if(read_switch(SWITCH_2))
		{
			set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, 80);
			ESP_LOGI(TAG_SWITCH_CONTROL_MOTOR, "MOTOR_A_0 BACKWARD");
		}
		else
		{
			set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
		}

		if(read_switch(SWITCH_3))
		{
			set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, 80);
			ESP_LOGI(TAG_SWITCH_CONTROL_MOTOR, "MOTOR_A_1 FORWARD");
		}
		else if(read_switch(SWITCH_4))
		{
			set_motor_speed(MOTOR_A_1, MOTOR_BACKWARD, 80);
			ESP_LOGI(TAG_SWITCH_CONTROL_MOTOR, "MOTOR_A_1 BACKWARD");
		}
		else
		{
			set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
		}


	//Delay the task for a specific amount of ticks, the constant portTICK_PERIOD_MS can be configured in the menu config of esp32.
    vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}
void app_main()
{
	/**
	 * @brief Basic Function for task creation
	 * 
	 */
	
	//Create a new task and add it to the list of tasks that are ready to run. 
	//https://www.freertos.org/a00125.html
	xTaskCreate(&drive_task,"drive_task",4096,NULL,1,NULL); // 4096 ->The number of words to allocate for use as the task’s stack. Here 4096 * 4 bytes are allocated.
 }
