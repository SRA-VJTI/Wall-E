//Components
#include "sra_board.h"
//#define debug

// C Headers
#include <stdio.h>
#include <math.h>
#include <time.h>

//Limiting Variables
#define MAX_PITCH_CORRECTION 90
#define MAX_PITCH_CUMULATIVE_ERROR 850

#define MAX_PWM 100
#define MIN_PWM 60

//Self Balancing Tuning Parameters
float pitch_kP = 15.1;	//5.85;
float pitch_kI = 0.075; //95;
float pitch_kD = 9;

float setpoint = 0;
float initial_acce_angle = 0;
float forward_angle = 0;

float forward_offset = 2.51;
float forward_buffer = 3.1;

//Error and correction values
float absolute_pitch_correction = 0, absolute_pitch_angle = 0, pitch_angle = 0, roll_angle = 0, pitch_error = 0, prevpitch_error = 0, pitchDifference = 0, pitch_cumulative_error = 0, pitch_correction = 0, integral_term = 0;

float motor_pwm = 0;

int constrain(int val, int lower_limit, int higher_limit)
{
	if (val < lower_limit)
	{
		val = lower_limit;
	}
	else if (val > higher_limit)
	{
		val = higher_limit;
	}

	return val;
}

// Calculate the motor inputs according to angle of the MPU
void calculate_pitch_error()
{
	pitch_error = pitch_angle;
	pitchDifference = (pitch_error - prevpitch_error);
	pitch_cumulative_error += pitch_error;

	if (pitch_cumulative_error > MAX_PITCH_CUMULATIVE_ERROR)
	{
		pitch_cumulative_error = MAX_PITCH_CUMULATIVE_ERROR;
	}
	else if (pitch_cumulative_error < -MAX_PITCH_CUMULATIVE_ERROR)
	{
		pitch_cumulative_error = -MAX_PITCH_CUMULATIVE_ERROR;
	}

	pitch_correction = pitch_kP * pitch_error + pitch_kI * pitch_cumulative_error + pitch_kD * pitchDifference;
	prevpitch_error = pitch_error;

	absolute_pitch_correction = fabs(pitch_correction);
	absolute_pitch_correction = constrain(absolute_pitch_correction, 0, MAX_PITCH_CORRECTION);
}

//The main task to balance the robot
void balance_task(void *arg)
{
	float euler_angle[2];
	if (i2c_master_init() == ESP_OK && enable_mpu6050() == ESP_OK)
	{

		while (1)
		{
			initial_acce_angle = setpoint;

			if (read_mpu6050(euler_angle) == ESP_OK)
			{

				calculate_pitch_error();
				enable_motor_driver(a, NORMAL_MODE);

				//constrain PWM values between max and min
				motor_pwm = constrain((absolute_pitch_correction), MIN_PWM, MAX_PWM);

				if (pitch_error > 1)
				{
					set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, motor_pwm);
					set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, motor_pwm);
				}

				else if (pitch_error < -1)
				{
					set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, motor_pwm);
					set_motor_speed(MOTOR_A_1, MOTOR_BACKWARD, motor_pwm);
				}
				else
				{
					set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
					set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
				}
			}
		} 
	}

} 

void app_main()
{
	xTaskCreate(&balance_task, "balance task", 100000, NULL, 1, NULL);
}
