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
        for (int i = 0; i < 5; i++)//blinks all the 8 leds 5 times
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
        // setting variable var to 0x01 

        while(1)//blinking the leds one by one from right hand side to left hand side
        {
            if (var == 0x00)
            //if variable var is  0x00(i.e, all leds are off)
            //for security check
            
            {
                var = 0x01;
             //setting var to  0x01(8th led is on)
            }            
            ESP_ERROR_CHECK(set_bar_graph(var));
            //setting the value of the function to that of the var 
            var = var << 1;
            //left shifting the values of var
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            //delay of 1s
        }
    }
}
