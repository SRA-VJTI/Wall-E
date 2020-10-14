#include "sra_board.h"

static const char* TAG_SWITCH_CONTROL_MOTOR = "switch_control";

void drive_task(void *arg)
{

	
	
	enable_switches();
	enable_motor_driver_a(PARALLEL_MODE); // Enable motor driver A in Parallel Mode
	enable_motor_driver_b(PARALLEL_MODE); // Enable motor driver B in Parallel Mode
	while(1)
	{
		if(read_switch(SWITCH_1))
		{
			set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, 80);
			ESP_LOGI(TAG_SWITCH_CONTROL_MOTOR, "MOTOR_A_0 FORWARD");	
		}
		
		else if(read_switch(SWITCH_2))
		{
			set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, 80);
			ESP_LOGI(TAG_SWITCH_CONTROL_MOTOR, "MOTOR_A_1 BACKWARD");
		}
		else if(read_switch(SWITCH_3))
		{
			set_motor_speed(MOTOR_B_0, MOTOR_FORWARD, 80);
			ESP_LOGI(TAG_SWITCH_CONTROL_MOTOR, "MOTOR_B_0 FORWARD");
		}
		else if(read_switch(SWITCH_4))
		{
			set_motor_speed(MOTOR_B_0, MOTOR_BACKWARD, 80);
			ESP_LOGI(TAG_SWITCH_CONTROL_MOTOR, "MOTOR_B_1 BACKWARD");
		}
		else
		{
			set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
			set_motor_speed(MOTOR_B_0, MOTOR_STOP, 0);
			
		}
			
	}
	
	
	
}


void app_main()
{
	/*
		Basic Function for task creation
	*/
    
        xTaskCreate(&drive_task,"drive_task",4096,NULL,1,NULL);
    
   
}
