//C Headers
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

//Components
#include "sra18.h"



void flicker_task(void *arg)
{
	/*
		Set the The LED Pins : GPIO 32, 33, 25, 26, 27, 14, 12 and 13 to OUTPUT
	*/
	gpio_set_direction(led_pack_1,GPIO_MODE_OUTPUT);
	gpio_set_direction(led_pack_2,GPIO_MODE_OUTPUT);
	gpio_set_direction(led_pack_3,GPIO_MODE_OUTPUT);	
	gpio_set_direction(led_pack_4,GPIO_MODE_OUTPUT);
	gpio_set_direction(led_pack_5,GPIO_MODE_OUTPUT);
	gpio_set_direction(led_pack_6,GPIO_MODE_OUTPUT);
	gpio_set_direction(led_pack_7,GPIO_MODE_OUTPUT);
	gpio_set_direction(led_pack_8,GPIO_MODE_OUTPUT);

	while(1)
	{

		gpio_set_level(led_pack_1,0);	//Set led_pack0 ON
		vTaskDelay(100 / 10);	//Wait for 100ms
		
		gpio_set_level(led_pack_1,1);	//Set led_pack0 OFF
		gpio_set_level(led_pack_2,0);	//Set led_pack1 ON
		vTaskDelay(100 / 10);	//Wait for 100ms

		gpio_set_level(led_pack_2,1);	//Set led_pack1 OFF
		gpio_set_level(led_pack_3,0);	//Set led_pack2 ON
		vTaskDelay(100 / 10);	//Wait for 100ms

		gpio_set_level(led_pack_3,1);	//Set led_pack2 OFF
		gpio_set_level(led_pack_4,0);	//Set led_pack3 ON
		vTaskDelay(100 / 10);	//Wait for 100ms

		gpio_set_level(led_pack_4,1);	//Set led_pack3 OFF
		gpio_set_level(led_pack_5,0);	//Set led_pack4 ON
		vTaskDelay(100 / 10);	//Wait for 100ms

		gpio_set_level(led_pack_5,1);	//Set led_pack4 OFF
		gpio_set_level(led_pack_6,0);	//Set led_pack5 ON
		vTaskDelay(100 / 10);	//Wait for 100ms

		gpio_set_level(led_pack_6,1);	//Set led_pack5 OFF
		gpio_set_level(led_pack_7,0);	//Set led_pack6 ON
		vTaskDelay(100 / 10);	//Wait for 100ms

		gpio_set_level(led_pack_7,1);	//Set led_pack6 OFF
		gpio_set_level(led_pack_8,0);	//Set led_pack7 ON
		vTaskDelay(100 / 10);	//Wait for 100ms

		gpio_set_level(led_pack_8,1);	//Set LED7 OFF
	}
	
}

void app_main()
{
	/*
		Basic Function for task creation
	*/

	xTaskCreate(&flicker_task,"flicker task",4096,NULL,1,NULL);
}
