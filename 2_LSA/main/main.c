#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sra_board.h"

#define BLACK_MARGIN 400
#define WHITE_MARGIN 2000
#define CONSTRAIN_LSA_LOW 0
#define CONSTRAIN_LSA_HIGH 1000

static const char* TAG = "LSA_READINGS";

void app_main(void)
{
    // enable line sensor
    ESP_ERROR_CHECK(enable_line_sensor());

    //Union containing line sensor readings
    line_sensor_array line_sensor_readings;

    while(1)
    {
        // get line sensor readings
        line_sensor_readings = read_line_sensor();
        for(int i = 0; i < 4; i++)
        {
            // constrain lsa readings between BLACK_MARGIN and WHITE_MARGIN
            line_sensor_readings.adc_reading[i] = bound(line_sensor_readings.adc_reading[i], BLACK_MARGIN, WHITE_MARGIN);
            // map readings from (BLACK_MARGIN, WHITE_MARGIN) to (CONSTRAIN_LSA_LOW, CONSTRAIN_LSA_HIGH)
            line_sensor_readings.adc_reading[i] = map(line_sensor_readings.adc_reading[i], BLACK_MARGIN, WHITE_MARGIN, CONSTRAIN_LSA_LOW, CONSTRAIN_LSA_HIGH);
        }

        // log final lsa readings
        ESP_LOGI(TAG, "LSA_1: %d \t LSA_2: %d \t LSA_3: %d \t LSA_4: %d",line_sensor_readings.adc_reading[0], line_sensor_readings.adc_reading[1], line_sensor_readings.adc_reading[2], line_sensor_readings.adc_reading[3] );

    }
}
