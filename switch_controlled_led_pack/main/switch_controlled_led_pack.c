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

	enable_buttons();	//Enable the buttons connected to GPIO for switches
	gpio_set_direction(led_pack_5,GPIO_MODE_OUTPUT);
	gpio_set_direction(led_pack_6,GPIO_MODE_OUTPUT);
	gpio_set_direction(led_pack_7,GPIO_MODE_OUTPUT);
	gpio_set_direction(led_pack_8,GPIO_MODE_OUTPUT);
	

	while(1)
	{

		/*
			Make the led_pack 5th led to blink
		*/
		if(pressed_switch(BUTTON_1))
		{
			
			printf("%s\n","Blinking led");

			gpio_set_level(led_pack_5,0);	//Set led ON
			
			vTaskDelay(100 / 10);			//Wait for 100ms
			
			gpio_set_level(led_pack_5,1);	//Set led OFF

		}

		/*
			Make the led_pack 6th led to blink	
		*/

		else if(pressed_switch(BUTTON_2))
		{
			
			printf("%s\n","Blinking led");

			gpio_set_level(led_pack_6,0);	//Set led ON
			
			vTaskDelay(100 / 10);			//Wait for 100ms
			
			gpio_set_level(led_pack_6,1);	//Set led OFF

		}

		/*
			Make the led_pack 7th led to blink	
		*/

		else if(pressed_switch(BUTTON_3))
		{
			
			printf("%s\n","Blinking led");

			gpio_set_level(led_pack_7,0);	//Set led ON
			
			vTaskDelay(100 / 10);			//Wait for 100ms
			
			gpio_set_level(led_pack_7,1);	//Set led OFF

		}


		/*
			Make the led_pack 8th led to blink	
		*/

		else if(pressed_switch(BUTTON_4))
		{
			
			printf("%s\n","Blinking led");

			gpio_set_level(led_pack_8,0);	//Set led ON
			
			vTaskDelay(100 / 10);			//Wait for 100ms
			
			gpio_set_level(led_pack_8,1);	//Set led OFF

		}

		/*
			If no buttons are pressed, off all the leds
		*/

	   	else 
		{
			gpio_set_level(led_pack_5,1);   //Set led OFF
			gpio_set_level(led_pack_6,1);   //Set led OFF
			gpio_set_level(led_pack_7,1);   //Set led OFF
			gpio_set_level(led_pack_8,1);   //Set led OFF
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
