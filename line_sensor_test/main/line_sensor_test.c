//C Headers
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

//Components
#include "sra18.h"

static const char *TAG_LINE_SENSOR = "line_sensor_test";

//Declare the the channel array consisting of the ADC channel inputs
adc1_channel_t channel[4] = {ADC_CHANNEL_7,ADC_CHANNEL_6,ADC_CHANNEL_0,ADC_CHANNEL_3};

int adc_reading[4];

void sensor_task(void *arg)
{

	while(1)
	{
		logD(TAG_LINE_SENSOR, "RAW 1: %d\tRAW 2: %d\tRAW 3: %d\tRAW 4: %d", \
			adc_reading[1], adc_reading[2], adc_reading[3], adc_reading[4]);
	}
	
}

void app_main()
{
	/*
		Basic Function for task creation
	*/

    xTaskCreate(&sensor_task,"blink task",4096,NULL,1,NULL);
}
