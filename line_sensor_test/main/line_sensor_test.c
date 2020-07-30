//C Headers
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>


//Components
#include "sra18.h"

//Declare the the channel array consisting of the ADC channel inputs

adc1_channel_t channel[4] = {ADC_CHANNEL_7,ADC_CHANNEL_6,ADC_CHANNEL_0,ADC_CHANNEL_3};

int adc_reading[4];

void sensor_task(void *arg)
{

	while(1)
	{
		for(int i =0;i<4;i++)
		{
			adc_reading[i] = adc1_get_raw(channel[i]);
			printf("RAW %d: %d\t",i,adc_reading[i]);
		}		
		printf("\n");
	}
	
}

void app_main()
{
	/*
		Basic Function for task creation
	*/

    xTaskCreate(&sensor_task,"blink task",4096,NULL,1,NULL);
}
