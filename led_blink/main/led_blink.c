//C Headers
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

//Components
#include "sra_board.h"

void blink_task(void *arg)
{
	enable_bar_graph();
	while(1)
	{
		
		set_bar_graph(0xFF);
		vTaskDelay(100 / portTICK_PERIOD_MS);
		set_bar_graph(0x00);
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void app_main()
{
	/**
	 * @brief Basic Function for task creation
	 * 
	 */

	xTaskCreate(&blink_task,"blink task",4096,NULL,1,NULL);
}
