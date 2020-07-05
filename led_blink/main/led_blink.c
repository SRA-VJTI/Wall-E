//C Headers
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

//Components
#include "sra18.h"

void blink_task(void *arg)
{
	/*
		Set the The LED Pins : GPIO 0 and GPIO 5 to OUTPUT
	*/

	gpio_set_direction(LED_1,GPIO_MODE_OUTPUT);
	gpio_set_direction(LED_2,GPIO_MODE_OUTPUT);	

	while(1)
	{

		gpio_set_level(LED_1,0);	//Set LED1 ON
		gpio_set_level(LED_2,0);	//Set LED2 ON

		vTaskDelay(1000 / 10);	//Wait for 1000ms
		


		gpio_set_level(LED_1,1);	//Set LED1 OFF
		gpio_set_level(LED_2,1);	//Set LED2 OFF

		vTaskDelay(1000 / 10);	//Wait for 1000ms
	}
	
}

void app_main()
{
	/*
		Basic Function for task creation
	*/

	xTaskCreate(&blink_task,"blink task",4096,NULL,1,NULL);
}
