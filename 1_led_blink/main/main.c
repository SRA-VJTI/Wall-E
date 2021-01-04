#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sra_board.h"

void app_main()
{
    ESP_ERROR_CHECK(enable_bar_graph());

    while(1)
    {
        for (int i = 0; i < 5; i++)
        {
            ESP_ERROR_CHECK(set_bar_graph(0xFF));
            vTaskDelay(1000 / portTICK_RATE_MS);
            ESP_ERROR_CHECK(set_bar_graph(0x00));
            vTaskDelay(1000 / portTICK_RATE_MS);
        }

        uint8_t var = 0x01;

        while(1)
        {
            if (var == 0x00)
            {
                var = 0x01;
            }
            
            ESP_ERROR_CHECK(set_bar_graph(var));
            var = var << 1;
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}