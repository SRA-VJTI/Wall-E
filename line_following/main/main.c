#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sra_board.h"

#define MODE NORMAL_MODE/*
 * Line Following PID Constants
 */
#define kP 1.3
#define kI 0
#define kD 1.9

/*
 * weights given to respective line sensor
 */
const int weights[4] = {3,1,-1,-3};

/*
 * Motor value constraints
 */
int optimum_duty_cycle = 70;
int lower_duty_cycle = 60;
int higher_duty_cycle = 80;
float left_duty_cycle = 0, right_duty_cycle = 0;

/*
 * Line Following PID Variables
 */
float error=0, prev_error=0, difference, cumulative_error, correction;

/*
 * Union containing line sensor readings
 */
line_sensor_array line_sensor_readings;

// #########################################################################

int constrain(int val, int lower_limit, int higher_limit)
{
    if(val < lower_limit)
    {
        val = lower_limit;
    }
    else if(val > higher_limit)
    {
        val = higher_limit;
    }
    return val;
}

int map(int val, int input_lower_limit, int input_higher_limit, int output_lower_limit, int output_higher_limit)
{
    return (output_lower_limit + ((output_higher_limit - output_lower_limit)/(input_higher_limit - input_lower_limit))*(val-input_lower_limit));
}

void calculate_correction()
{
    error = error*10;
    difference = error - prev_error;
    cumulative_error += error;

    if(cumulative_error > 30)
    {
        cumulative_error = 30;
    }
    
    else if(cumulative_error < -30)
    {
        cumulative_error = -30;
    }

    correction = kP*error + kI*cumulative_error + kD*difference;
    prev_error = error;
}


void calculate_error()
{
    int all_black_flag = 0; // assuming initially all black condition
    int weighted_sum = 0, sum = 0; 
    float pos = 0;

    for(int i = 0; i < 4; i++)
    {
        if(line_sensor_readings.adc_reading[i] > 400)
        {
            all_black_flag = 0;
        }
        weighted_sum = weights[i] * line_sensor_readings.adc_reading[i];
        sum = sum + line_sensor_readings.adc_reading[i];
    }

    if(sum != 0) // sum can never be 0 but just for safety purposes
    {
        pos = weighted_sum / sum; // This will give us the position wrt line. if +ve then bot is facing left and if -ve the bot is facing to right.
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
    ESP_ERROR_CHECK(enable_motor_driver_a(MODE));
    ESP_ERROR_CHECK(init_line_sensor());

    while(true)
    {
        line_sensor_readings = read_line_sensor();
        for(int i = 0; i < 4; i++)
        {
            line_sensor_readings.adc_reading[i] = constrain(line_sensor_readings.adc_reading[i], 400, 2200);
            line_sensor_readings.adc_reading[i] = map(line_sensor_readings.adc_reading[i], 400, 2200, 0, 1000);
        }

        calculate_error();
        calculate_correction();

        left_duty_cycle = constrain((optimum_duty_cycle - correction), lower_duty_cycle, higher_duty_cycle);
        right_duty_cycle = constrain((optimum_duty_cycle + correction), lower_duty_cycle, higher_duty_cycle);

        set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, left_duty_cycle);
        set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, right_duty_cycle);
    }
}



void app_main()
{
    xTaskCreate(&line_follow_task, "line_follow_task", 100000, NULL, 1, NULL);
}