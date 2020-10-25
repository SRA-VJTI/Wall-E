//Components
#include "sra_board.h"
//#define debug

// C Headers
#include <stdio.h>
#include <math.h>
#include <time.h>

//Limiting Variables
#define MAX_PITCH_CORRECTION (90)
#define MAX_PITCH_CUMULATIVE_ERROR (850)

#define MAX_PWM (100)
#define MIN_PWM (60)

//Self Balancing Tuning Parameters
float pitch_kP = 15.1f;	
float pitch_kI = 0.075f; 
float pitch_kD = 9.0f;

float setpoint = 0.0f;
float initial_acce_angle = 0.0f;
float forward_angle = 0.0f;

float forward_offset = 2.51f;
float forward_buffer = 3.1f;

float pitch_angle = 0.0f, roll_angle = 0.0f;
float motor_pwm = 0.0f;

int bound(int val, int lower_limit, int higher_limit)
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
void calculate_motor_command(const float pitch_cmd, const float pitch_angle, float *motor_cmd)
{
	static float pitch_error = 0.0f;
	static float pitch_error_difference = 0.0f;
    static float pitch_error_cummulative = 0.0f;

	static float prevpitch_error = 0.0f;

    static float pitch_correction = 0.0f;
    static float absolute_pitch_correction = 0.0f;

	pitch_error = pitch_cmd - pitch_angle;
	pitch_error_difference = pitch_error - prevpitch_error;
	pitch_error_cummulative += pitch_error;

	// integral term bounding, prevent windup
	pitch_error_cummulative = bound(pitch_error_cummulative, -MAX_PITCH_CUMULATIVE_ERROR, MAX_PITCH_CUMULATIVE_ERROR)

	pitch_correction = pitch_kP * pitch_error + pitch_kI * pitch_error_cummulative + pitch_kD * pitch_error_difference;

	absolute_pitch_correction = fabs(pitch_correction);
	*motor_cmd = bound(absolute_pitch_correction, 0, MAX_PITCH_CORRECTION);

	prevpitch_error = pitch_error;
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

				pitch_angle = euler_angle[1];
				
				float pitch_cmd = 0.0f;
				float motor_cmd = 0.0f;
				calculate_motor_command(pitch_cmd, pitch_angle, &motor_cmd);

				//bound PWM values between max and min
				motor_pwm = bound((motor_cmd), MIN_PWM, MAX_PWM);

				enable_motor_driver(a, NORMAL_MODE);
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
