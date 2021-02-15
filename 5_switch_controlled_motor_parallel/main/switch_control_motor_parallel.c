#include "sra_board.h"

static const char* TAG_SWITCH_CONTROL_MOTOR = "switch_control";			

void drive_task(void *arg)
{
	enable_switches();
	enable_motor_driver(a,PARALLEL_MODE); // Enable motor driver A in Parallel Mode
	enable_motor_driver(b,PARALLEL_MODE); // Enable motor driver B in Parallel Mode

	while(1)
	{
		if(read_switch(SWITCH_1))   					                //Reading the input of switch 1
		{
			set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, 80);		        //Setting the speed of motor A in forward direction, duty cycle 80%
			ESP_LOGI(TAG_SWITCH_CONTROL_MOTOR, "MOTOR_A_0 FORWARD");    //Sending a message to esp log file	
		}
		else if(read_switch(SWITCH_2))                                  //Reading the input of switch 2
		{
			set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, 80);             //Setting the speed of motor A in reverse direction, duty cycle 80%
			ESP_LOGI(TAG_SWITCH_CONTROL_MOTOR, "MOTOR_A_0 BACKWARD");
		}
		else
		{
			set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);                  //If both switches have zero input, stop.
		}

		if(read_switch(SWITCH_3))
		{
			set_motor_speed(MOTOR_B_0, MOTOR_FORWARD, 80);
			ESP_LOGI(TAG_SWITCH_CONTROL_MOTOR, "MOTOR_B_0 FORWARD");
		}
		else if(read_switch(SWITCH_4))
		{
			set_motor_speed(MOTOR_B_0, MOTOR_BACKWARD, 80);
			ESP_LOGI(TAG_SWITCH_CONTROL_MOTOR, "MOTOR_B_0 BACKWARD");
		}
		else
		{
			set_motor_speed(MOTOR_B_0, MOTOR_STOP, 0);
		}
    vTaskDelay(100 / portTICK_PERIOD_MS);					//Delays the task by 1 Tick-Period
	}
}


void app_main()
{
	 /**
     * @brief Basic Function for task creation
     */
	//Task is created and added to the lists of tasks ready to run
    xTaskCreate(&drive_task,					//address of the function to execute
		"drive_task",							//name of the task
		4096,								    //Memory allocated to the Task ie. 4096(4 bytes)
		NULL,								    //Paramter passed as input(if any)
		1,								        //Priority of the task
		NULL);                       			//Task handle
}                                                                                 
