//Components
#include "sra_board.h"
//#define debug

// C Headers
#include <stdio.h>
#include <math.h>

#include "tuning_http_server.h"

//Limiting Variables
#define MAX_PITCH_CORRECTION (90.0f)
#define MAX_PITCH_AREA (850.0f)
#define MAX_PITCH_RATE (850.0f)

#define MAX_PWM (100.0f)
#define MIN_PWM (60.0f)

/* Self Balancing Tuning Parameters
float forward_offset = 2.51f;
float forward_buffer = 3.1f;
*/

// Calculate the motor inputs according to angle of the MPU
void calculate_motor_command(const float pitch_error, float *motor_cmd)
{
	static bool is_first_iteration = true;
	static uint32_t timer = 0;
	float dt = 0.005f;

	static float prev_pitch_error = 0.0f;
	static float pitch_area = 0.0f;
	float pitch_error_difference = 0.0f; 

	float pitch_correction = 0.0f, absolute_pitch_correction = 0.0f;
	float pitch_rate = 0.0f, pitch_area = 0.0f;

	float P_term = 0.0f, I_term = 0.0f, D_term = 0.0f;

	if (is_first_iteration)
	{
		is_first_iteration = false;
		timer = esp_timer_get_time();
		return;
	}

	dt = (float)(esp_timer_get_time() - timer) / 1000000;
	timer = esp_timer_get_time();

	pitch_error_difference = pitch_error - prev_pitch_error;

	pitch_area += (pitch_error * dt);
	pitch_rate = pitch_error_difference / dt;

	/**
	 * Integral term is bounded to prevent windup
	 * Bounding also helps, when dt is jumpy when the schedulers are warming up the threads
	*/
	P_term = read_pid_const().kp * pitch_error;
	I_term = read_pid_const().ki * bound(pitch_area, -MAX_PITCH_AREA, MAX_PITCH_AREA);
	D_term = read_pid_const().kd * bound(pitch_rate, -MAX_PITCH_RATE, MAX_PITCH_RATE);

	pitch_correction = P_term + I_term + D_term;
	absolute_pitch_correction = fabsf(pitch_correction);

	*motor_cmd = bound(absolute_pitch_correction, 0, MAX_PITCH_CORRECTION);
	prev_pitch_error = pitch_error;
}

//The main task to balance the robot
void balance_task(void *arg)
{
	// mpu_offsets are the initial accelerometer angles at rest position
	float euler_angle[2], mpu_offset[2] = {0.0f, 0.0f};
	float pitch_angle, pitch_error;
	float motor_cmd, motor_pwm = 0.0f;

	// Pitch angle where you want to go - pitch_cmd, setpoint and mpu_offsets are linked to one another
	float pitch_cmd = 0.0f;

	if (enable_mpu6050() == ESP_OK)
	{
		enable_motor_driver(a, NORMAL_MODE);
		while (1)
		{
			if (read_mpu6050(euler_angle, mpu_offset) == ESP_OK)
			{
				pitch_cmd = read_pid_const().setpoint;
				pitch_angle = euler_angle[1];
				pitch_error = pitch_cmd - pitch_angle;

				calculate_motor_command(pitch_error, &motor_cmd);

				//bound PWM values between max and min
				motor_pwm = bound((motor_cmd), MIN_PWM, MAX_PWM);

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

	// remove this it is only for debugging
	while (1)
	{
		vTaskDelay(100 / portTICK_PERIOD_MS);
		ESP_LOGI("debug", "%f %f %f %f %f", read_pid_const().kp, read_pid_const().ki, read_pid_const().kd, read_pid_const().setpoint, read_pid_const().offset);
	}

	vTaskDelete(NULL);
}

void app_main()
{
	xTaskCreate(&balance_task, "balance task", 4096, NULL, 1, NULL);
	start_tuning_http_server();
}
