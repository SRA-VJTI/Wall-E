#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sra_board.h"

static const char *TAG = "mpu_test";

void mpu_task(void *arg)
{
    float euler_angle[2], mpu_offset[2] = {0.0f, 0.0f};

    while (1)
    {
        if (enable_mpu6050() == ESP_OK)
        {
            while (read_mpu6050(euler_angle, mpu_offset) == ESP_OK)
                ESP_LOGI(TAG, "Roll: %0.2f | Pitch: %0.2f", euler_angle[0], euler_angle[1]);
        }
        
        ESP_LOGE(TAG, "MPU Initialisation Failed / Connection Broke!");
    }
}

void app_main()
{
    xTaskCreate(mpu_task, "mpu_task", 4096, NULL, 1, NULL);
}
