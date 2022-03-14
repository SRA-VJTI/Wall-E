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
    // Euler angle buffer includes two floats - Roll angle and Pitch angle
    float euler_angle[2], mpu_offset[2] = {0.0f, 0.0f};

#ifdef CONFIG_ENABLE_OLED
    // Declaring the required OLED struct
    u8g2_t oled_config;
    // Initialising the OLED
    ESP_ERROR_CHECK(init_oled(&oled_config));
#endif

    while (1)
    {
        /*The MPU6050 comes up in sleep mode upon power-up
         * enable_mpu6050 disables the SLEEP_MODE of the MPU by accessing the POWER_MANAGEMENT_1 register
         * and checks if the MPU is initialised correctly
         */
        if (enable_mpu6050() == ESP_OK)
        {
            /* Euler angles are obtained by reading values from the accelerometer and gyroscope
             * Calculating change in roll, pitch from raw accelerometer and gyroscope readings seperately
             * And then performing sensor fusion via complementary filter to obtain a stable reading
             * The changes in angles from both measurements are weighted and added to the complementary angle
             */
            while (read_mpu6050(euler_angle, mpu_offset) == ESP_OK)
            {
                // Logging information of roll and pitch angles
                ESP_LOGI(TAG, "Roll: %0.2f | Pitch: %0.2f", euler_angle[0], euler_angle[1]);
#ifdef CONFIG_ENABLE_OLED
                // Displaying pitch and roll on OLED
                display_mpu(euler_angle[1], euler_angle[0], &oled_config);
#endif
            }
        }
        // Logging Error
        ESP_LOGE(TAG, "MPU Initialisation Failed / Connection Broke!");
    }
}

void app_main()
{
    /* creating task to run mpu_task function
     * which runs at priority 1
     */
    xTaskCreate(mpu_task, "mpu_task", 4096, NULL, 1, NULL);
}
