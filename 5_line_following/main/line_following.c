#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sra_board.h"
#include "tuning_http_server.h"
#include "esp_netif.h"
#include "lwip/ip4_addr.h"
#ifdef CONFIG_SAC_JUNCTION_MAZE
#include "sac_track_controller.h"
#endif

#define MODE NORMAL_MODE
#define BLACK_MARGIN 4095
#define WHITE_MARGIN 0
#define bound_LSA_LOW 0
#define bound_LSA_HIGH 1000
#define BLACK_BOUNDARY  820    // Boundary value to distinguish between black and white readings

/*
 * weights given to respective line sensor
 */
const int weights[5] = {-5, -3, 1, 3, 5};

/*
 * Motor value boundts
 */
int optimum_duty_cycle = 57;
int lower_duty_cycle = 45;
int higher_duty_cycle = 65;
float left_duty_cycle = 0, right_duty_cycle = 0;

/*
 * Line Following PID Variables
 */
float error=0, prev_error=0, difference, cumulative_error, correction;

/*
 * Union containing line sensor readings
 */
line_sensor_array line_sensor_readings;

void calculate_correction()
{
    error = error*10;  // we need the error correction in range 0-100 so that we can send it directly as duty cycle paramete
    difference = error - prev_error;
    cumulative_error += error;

    cumulative_error = bound(cumulative_error, -30, 30);

    correction = read_pid_const().kp*error + read_pid_const().ki*cumulative_error + read_pid_const().kd*difference;
    prev_error = error;
}

void calculate_error()
{
    int all_black_flag = 1; // assuming initially all black condition
    float weighted_sum = 0, sum = 0; 
    float pos = 0; int k = 0;

    for(int i = 0; i < 5; i++)
    {
        if(line_sensor_readings.adc_reading[i] > BLACK_BOUNDARY)
        {
            all_black_flag = 0;
        }
        if(line_sensor_readings.adc_reading[i] > BLACK_BOUNDARY)
        {
            k = 1;
        }
        if(line_sensor_readings.adc_reading[i] < BLACK_BOUNDARY)
        {
            k = 0;
        }
        weighted_sum += (float)(weights[i]) * k;
        sum = sum + k;
    }

    if(sum != 0) // sum can never be 0 but just for safety purposes
    {
        pos = (weighted_sum - 1) / sum; // This will give us the position wrt line. if +ve then bot is facing left and if -ve the bot is facing to right.
    }

    if(all_black_flag == 1)  // If all black then we check for previous error to assign current error.
    {
        if(prev_error > 0)
        {
            error = 2.5;
        }
        else
        {
            error = -2.5;
        }
    }
    else
    {
        error = pos;
    }
}

void line_follow_task(void* arg)
{
    motor_handle_t motor_a_0, motor_a_1;
    ESP_ERROR_CHECK(enable_motor_driver(&motor_a_0, MOTOR_A_0));
    ESP_ERROR_CHECK(enable_motor_driver(&motor_a_1, MOTOR_A_1));
    adc_handle_t line_sensor;
    ESP_ERROR_CHECK(enable_line_sensor(&line_sensor));
    ESP_ERROR_CHECK(enable_bar_graph());
#ifdef CONFIG_ENABLE_OLED
    // Initialising the OLED
    ESP_ERROR_CHECK(init_oled());
    vTaskDelay(100);

    // Clearing the screen
    lv_obj_clean(lv_scr_act());

#endif

    #ifdef CONFIG_SAC_JUNCTION_MAZE
    sac_track_t track = {0};
#ifdef CONFIG_SAC_MAZE_INVERT_LINE
    track.inverted = true;
#endif
#ifdef CONFIG_SAC_AUTO_POLARITY
    track.automatic = true;
#endif
    #endif

    while(true)
    {
        line_sensor_readings = read_line_sensor(line_sensor);
        for(int i = 0; i < 5; i++)
        {
            line_sensor_readings.adc_reading[i] = bound(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN);
            line_sensor_readings.adc_reading[i] = map(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN, bound_LSA_LOW, bound_LSA_HIGH);
            line_sensor_readings.adc_reading[i] = 1000 - (line_sensor_readings.adc_reading[i]);
        }

#ifdef CONFIG_SAC_JUNCTION_MAZE
        uint8_t line_bits = 0;
        for (int i = 0; i < 5; ++i) {
            bool on_line = line_sensor_readings.adc_reading[i] > BLACK_BOUNDARY;
#ifdef CONFIG_SAC_MAZE_REVERSE_SENSORS
            int bit = 4 - i;
#else
            int bit = i;
#endif
            if (on_line) line_bits |= (uint8_t)(1u << bit);
        }
        uint32_t now_ms = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
        sac_track_output_t track_output = sac_track_step(&track, line_bits, now_ms);
        line_bits = track_output.line_bits;
        maze_output_t command = track_output.motor;
        if (track_output.switched) {
            ESP_LOGI("maze", "Line polarity switched; inverted=%d", track.inverted);
        }
        if (command.reset_pid) {
            error = prev_error = cumulative_error = difference = correction = 0;
        }
        if (command.decision) ESP_LOGI("maze", "Decision: %c", command.decision);
        if (command.override) {
            ESP_ERROR_CHECK(set_motor_speed(motor_a_0,
                command.left < 0 ? MOTOR_BACKWARD : command.left > 0 ? MOTOR_FORWARD : MOTOR_STOP,
                command.left < 0 ? -command.left : command.left));
            ESP_ERROR_CHECK(set_motor_speed(motor_a_1,
                command.right < 0 ? MOTOR_BACKWARD : command.right > 0 ? MOTOR_FORWARD : MOTOR_STOP,
                command.right < 0 ? -command.right : command.right));
            vTaskDelay(pdMS_TO_TICKS(10) ? pdMS_TO_TICKS(10) : 1);
            continue;
        }
        /* Symmetric position estimate for maze mode, including inverted lines. */
        int count = 0, position = 0;
        for (int i = 0; i < 5; ++i) {
            if (line_bits & (1u << i)) { position += (i - 2) * 2; ++count; }
        }
        error = count ? (float)position / count : (prev_error > 0 ? 2.5f : -2.5f);
#else
        calculate_error();
#endif
        calculate_correction();

        left_duty_cycle = bound((optimum_duty_cycle + correction), lower_duty_cycle, higher_duty_cycle);
        right_duty_cycle = bound((optimum_duty_cycle - correction), lower_duty_cycle, higher_duty_cycle);

        set_motor_speed(motor_a_0, MOTOR_FORWARD, left_duty_cycle);
        set_motor_speed(motor_a_1, MOTOR_FORWARD, right_duty_cycle);


        //ESP_LOGI("debug","left_duty_cycle:  %f    ::  right_duty_cycle :  %f  :: error :  %f  correction  :  %f  \n",left_duty_cycle, right_duty_cycle, error, correction);
        ESP_LOGI("debug", "KP: %f ::  KI: %f  :: KD: %f", read_pid_const().kp, read_pid_const().ki, read_pid_const().kd);
#ifdef CONFIG_ENABLE_OLED
        // Displaying kp, ki, kd values on OLED 
        if (read_pid_const().val_changed)
        {
            esp_netif_ip_info_t ip_info;
            esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info);

            const char *ip_str_value = ip4addr_ntoa((const ip4_addr_t *)&ip_info.ip);
            display_pid_values(read_pid_const().kp, read_pid_const().ki, read_pid_const().kd, ip_str_value);

            reset_val_changed_pid_const();
        }
#endif

        vTaskDelay(pdMS_TO_TICKS(10) ? pdMS_TO_TICKS(10) : 1);
    }

    vTaskDelete(NULL);
}

void app_main()
{
    start_tuning_http_server();
    xTaskCreate(&line_follow_task, "line_follow_task", 4096, NULL, 1, NULL);
}
