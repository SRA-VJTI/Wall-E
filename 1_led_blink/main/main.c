#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sra_board.h"

void app_main()
{
    ESP_ERROR_CHECK(enable_bar_graph());
    // enable_bar_graph() turns on the gpio pins, if it succeeds it returns ESP_OK else it returns ESP_FAIL
    // If the argument of ESP_ERROR_CHECK() is not equal ESP_OK, then an error message is printed on the console, and abort() is called. 
    

    while(1)
    {
        //blinks all the 8 leds 5 times
        for (int i = 0; i < 5; i++)
        {
            ESP_ERROR_CHECK(set_bar_graph(0xFF));
            //0xFF = 1111 1111(all leds are on)
            // setting values of all 8 leds to 1
            vTaskDelay(1000 / portTICK_RATE_MS);
            //delay of 1s
            ESP_ERROR_CHECK(set_bar_graph(0x00));
            //0x00 = 0000 0000(all leds are off)
            // setting values of all 8 leds to 0
            vTaskDelay(1000 / portTICK_RATE_MS);
            //delay of 1s
        }

        uint8_t var = 0x01;
        //0x01 = 0000 0001(only 8th led is on)
        // After left-shifting 0x01 8-times, we end up with 0x00. To recover the var variable, we initialise it with 0x01.

        while(1)
        {   
            // After left-shifting 0x01 8-times, we end up with 0x00. To recover the var variable, we initialise it with 0x01.
            if (var == 0x00)
            //if variable var is  0x00(i.e, all leds are off)
            //it checks whether the value of var has changed from 0x01 to 0x00.
            {
                var = 0x01;
           //setting var to  0x01(8th led is on)
            }            
            ESP_ERROR_CHECK(set_bar_graph(var));
            // var contains the data for the LEDs and it's been passed as an argument to set_bar_graph() function.
            var = var << 1;
            // It left shifts the values of var eg. from 0x01(means only the 8th led is on) it changes to 
            // 0x02 (means only the 7th LED is on). Similarly, the LED's are turned on one by one from the 8th to the 1st.
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            //delay of 1s
        }
    }
}
