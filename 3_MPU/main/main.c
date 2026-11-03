#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "sra_board.h"

static const char *TAG1 = "mpu_task";
static const char *TAG2 = "display_task";
#define G_RAW_VALUE 16384.0f
QueueHandle_t accel_queue = NULL;

void mpu_task(void *arg)
{
    // Euler angle buffer includes two floats - Roll angle and Pitch angle
    float euler_angle[2], mpu_offset[2] = {0.0f, 0.0f};

#ifdef CONFIG_ENABLE_LED_MATRIX
    uint8_t acce_bytes[BUFF_SIZE];
    int16_t acce_raw_value[BUFF_SIZE / 2];
#endif

#ifdef CONFIG_ENABLE_OLED
    // Initialising the OLED
    ESP_ERROR_CHECK(init_oled());
    vTaskDelay(100);

    // Clearing the screen
    lv_obj_clean(lv_scr_act());
#endif

    /* Euler angles are obtained by reading values from the accelerometer and gyroscope
     * Calculating change in roll, pitch from raw accelerometer and gyroscope readings seperately
     * And then performing sensor fusion via complementary filter to obtain a stable reading
     * The changes in angles from both measurements are weighted and added to the complementary angle
     */
    while (read_mpu6050(euler_angle, mpu_offset) == ESP_OK)
    {
        // Logging information of roll and pitch angles
        ESP_LOGI(TAG1, "Roll: %0.2f | Pitch: %0.2f", euler_angle[0], euler_angle[1]);
        // mpu6050_read_acce(acce_bytes, sizeof(acce_bytes));
        
        #ifdef CONFIG_ENABLE_LED_MATRIX
        // Creating a queue to send data from mpu_task to display_task
        mpu6050_read_acce(acce_bytes, sizeof(acce_bytes));
        combine_msb_lsb_raw_data(acce_bytes, acce_raw_value);
        if (accel_queue != NULL)
        {
            xQueueSend(accel_queue, acce_raw_value, portMAX_DELAY);
        }
#endif

#ifdef CONFIG_ENABLE_OLED
        // Displaying pitch and roll on OLED
        display_mpu(euler_angle[1], euler_angle[0]);
#endif
    }
}

void display_task(void *arg)
{
    // Setting up the led matrix
    led_matrix_t matrix;
    ESP_ERROR_CHECK(led_matrix_init(&matrix));
    ESP_ERROR_CHECK(led_matrix_set_brightness(&matrix, 4));
    float dir_x, dir_y, normalizing_factor, normalized_dir_x, normalized_dir_y;
    uint8_t rows[LED_MATRIX_HEIGHT] = {0x00};
    uint8_t prev_rows[LED_MATRIX_HEIGHT] = {0x00};

    int16_t acce_raw_value[BUFF_SIZE / 2];
    while (1)
    {
        // Read only the accelerometer values used by the LED matrix.
        if (xQueueReceive(accel_queue, acce_raw_value, portMAX_DELAY) == pdTRUE)
        {
            // get g value from raw value
            dir_x = ((float)acce_raw_value[0]) / G_RAW_VALUE;
            dir_y = ((float)acce_raw_value[1]) / G_RAW_VALUE;

            // normalizes the value to get the direction vector
            normalizing_factor = sqrt(dir_x * dir_x + dir_y * dir_y);
            normalized_dir_x = dir_x / normalizing_factor;
            normalized_dir_y = dir_y / normalizing_factor;

            // loops through each pixel and calculates the dot product of the position vector of pixel from origin and the direction vector
            for (int i = 0; i < LED_MATRIX_HEIGHT; i++)
            {
                rows[i] = 0x00;
                for (int j = 0; j < LED_MATRIX_WIDTH; j++)
                {
                    float pos_x = 3.5f - i;
                    float pos_y = j - 3.5f;
                    float score = pos_x * normalized_dir_x + pos_y * normalized_dir_y;
                    // limits the dot product to not change the state of led on very small change
                    float threshold;
                    if (prev_rows[i] & (1 << j))
                    {
                        threshold = -0.30f;
                    }
                    else
                    {
                        threshold = 0.30f;
                    }
                    if (score > threshold)
                    {
                        rows[i] |= 1 << j;
                    }
                }
                // stores the previous state
                prev_rows[i] = rows[i];
            }
            ESP_ERROR_CHECK(led_matrix_draw_bitmap(&matrix, rows));
            ESP_ERROR_CHECK(led_matrix_show(&matrix));
            vTaskDelay(pdMS_TO_TICKS(20));
        }
        else
        {
            ESP_LOGE(TAG2, "%s", "Error reading values!");
        }
    }
}

void app_main()
{
    /*The MPU6050 comes up in sleep mode upon power-up
     * enable_mpu6050 disables the SLEEP_MODE of the MPU by accessing the POWER_MANAGEMENT_1 register
     * and checks if the MPU is initialised correctly
     */
    while (enable_mpu6050() != ESP_OK)
    {
        ESP_LOGE("main", "MPU initialization failed, retrying...");
        vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for 1 second before retrying
    }

#ifdef CONFIG_ENABLE_LED_MATRIX
    accel_queue = xQueueCreate(1, sizeof(int16_t) * (BUFF_SIZE / 2));
    configASSERT(accel_queue);
#endif

    xTaskCreate(mpu_task, "mpu_task", 4096, NULL, 2, NULL);

#ifdef CONFIG_ENABLE_LED_MATRIX
    xTaskCreate(display_task, "display_task", 4096, NULL, 1, NULL);
#endif
}