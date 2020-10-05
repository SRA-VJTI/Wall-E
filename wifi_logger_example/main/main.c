#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_logger.h"
#include "tuning.h"

void app_main(void)
{
    initialise_wifi();
    start_wifi_logger(); // start wifi logger process
    
    while(1)
    {
        ESP_LOGI("main", "%s", "test sys log");
        wifi_log_e("test", "%s %d %f", "hello world: ERROR", 23, 56.23); // write log over wifi with log level -> ERROR
        wifi_log_w("test", "%s %d %f", "hello world: WARN", 24, 90.341223242); // write log over wifi with log level -> WARN
        wifi_log_i("test", "%s %d %f", "hello world: INFO", 25, 12.763242); // write log over wifi with log level -> INFO
        wifi_log_d("test", "%s %d %f", "hello world: DEBUG", 26, 80.3242); // write log over wifi with log level -> DEBUG
        wifi_log_v("test", "%s %d %f", "hello world: VERBOSE", 27, 76.5242); // write log over wifi with log level -> VERBOSE
    
        vTaskDelay(100); // wait for 100 ms
    }
}
