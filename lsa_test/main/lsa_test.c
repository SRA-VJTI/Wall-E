//Components
#include "sra_board.h"
//#define debug

//Declare the the channel array of LSA pins 

void sensor_task(void *arg)
{

	init_line_sensor();

	while(1)
	{	
		line_sensor_array line_sensor_readings;

		// Initializing readings to 0
		for(int i = 0; i < 4; i++)
    	{
    	    line_sensor_readings.adc_reading[i] = 0;
    	}
		
		
		//Reading Raw samples
    	for( int i = 0; i < NO_OF_SAMPLES; i++)
    	{
    	    for(int j = 0; j < 4; j++)
    	    {
    	        line_sensor_readings.adc_reading[j] = line_sensor_readings.adc_reading[j] + read_adc(line_sensor_pins[j]);
    	    }
    	}


		//Applying multisampling
    	for(int i = 0; i < 4; j++)
    	{
    	    line_sensor_readings.adc_reading[i] = line_sensor_readings.adc_reading[i] / NO_OF_SAMPLES;
			printf("RAW %d: %d\t",i,line_sensor_readings.adc_reading[i]);
    	}


		#ifdef debug
		uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading[j] , adc_chars);
		printf("Voltage %d: %d\t\t",j,voltage);
		//Turn on LSA according readings of adc 
		#endif		

		printf("\n");
		vTaskDelay(pdMS_TO_TICKS(100));
	}
	
}

void app_main()
{
	/*
		Basic Function for task creation
	*/
    xTaskCreate(&sensor_task,"LSA task",4096,NULL,1,NULL);
}
