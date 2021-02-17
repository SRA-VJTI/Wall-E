// SRA's custom header file including additional functions
#include "sra_board.h"

// pointer to a character array
static const char* TAG_SWITCH_CONTROL_MOTOR = "switch_control";			

// main driver function
void drive_task(void *arg)
{
	/*
	 activate gpio pins SWITCH_1, SWITCH_2, SWITCH_3, SWITCH_4 .. 
	 * switches coorespond to following gpio pins
	 * SWITCH_1 : 4
	 * SWITCH_2 : 2
	 * SWITCH_3 : 17
	 * SWITCH_4 : 16
	more details can be found in https://github.com/SRA-VJTI/sra-board-component/blob/9f28700759ac816660c18859d65303d6540e8732/src/switches.c#L6-L34 
	 
        Enable switches.
	Enable motor driver A in Parallel Mode
	Enable motor driver B in Parallel Mode
	*/
	enable_switches();
	enable_motor_driver(a,PARALLEL_MODE); 
	enable_motor_driver(b,PARALLEL_MODE); 

	//infinite loop to get LSA readings continuously

	while(1)
	{
		// check input from LSA and activate the appropriate motor accordingly.


		/*
		Reading the input of switch 1,2,3,4

		SWITCH 1: Setting the speed of motor A in forward direction, duty cycle 80%
		SWITCH 2: Setting the speed of motor A in reverse direction, duty cycle 80%
		If both switches have zero input, stop

		SWITCH 3: Setting the speed of motor B in forward direction, duty cycle 80% 
`		SWITCH 4: Setting the speed of motor B in reverse direction, duty cycle 80%
		If both switches have zero input, stop.
		*/

		if(read_switch(SWITCH_1))   					        
		{
			set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, 80);	

			// Display logs - final LSA readings	        
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
     	*/

	/*
    xTaskCreate
	* creates a new task with name "drive_task" and allocates 4096 words of memory to it
	* for more information about xTaskCreate visit //https://www.freertos.org/a00125.html	
	 
        Task is created and added to the lists of tasks ready to run

	&drive_task:	Address of the function to execute
	drive_task :	Name of the task
	4096	   :	Memory allocated to the Task ie. 4096(4 bytes)
	NULL	   :	Paramter passed as input(if any)
	1	   	   :	Priority of the task
	NULL       :    Task handle
	*/	
    xTaskCreate(&drive_task,"drive_task",4096,NULL,1,NULL);                       			
}                                                                                 
