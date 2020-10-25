//Components
#include "sra_board.h"
//#define debug

// C Headers
#include <stdio.h>
#include <math.h>

//Limiting Variables
#define MAX_PITCH_CORRECTION (90.0f)
#define MAX_PITCH_AREA (850.0f)
#define MAX_PITCH_RATE (850.0f)

#define MAX_PWM (100.0f)
#define MIN_PWM (60.0f)

//Self Balancing Tuning Parameters
float pitch_kP = 15.1f;	
float pitch_kI = 0.075f; 
float pitch_kD = 9.0f;

float setpoint = 0.0f;

float forward_offset = 2.51f;
float forward_buffer = 3.1f;

float pitch_angle = 0.0f, roll_angle = 0.0f;
float motor_pwm = 0.0f;

// Calculate the motor inputs according to angle of the MPU
void calculate_motor_command(const float pitch_cmd, const float pitch_angle, float *motor_cmd)
{
	static float pitch_error = 0.0f;
	static float pitch_error_difference = 0.0f;
    static float pitch_error_cummulative = 0.0f;
	
	static bool is_first_command = true;
	static float dt = 1.0f;
	static unit32_t timer;

	static float prevpitch_error = 0.0f;
    static float pitch_correction = 0.0f;
    static float absolute_pitch_correction = 0.0f;

	static float P_term = 0.0f;
	static float I_term = 0.0f;
	static float D_term = 0.0f;

	static float pitch_rate = 0.0f;
	static float pitch_area = 0.0f;

	if (is_first_command)
	{
		is_first_command = false;
		timer = esp_timer_get_time();
	}
	dt = (float)(esp_timer_get_time() - timer)/1000000;
	timer = esp_timer_get_time();

	pitch_error = pitch_cmd - pitch_angle;
	pitch_error_difference = pitch_error - prevpitch_error;
	pitch_error_cummulative += pitch_error;

	pitch_area = pitch_error_cummulative*dt;
	pitch_rate = pitch_error_difference / dt;
	
	/**
	 * Intergral term is bounded to prevent windup
	 * Bounding also helps, when dt is jumpy when the schedulers are warming up the threads
	 */
	P_term = pitch_kP * pitch_error;
	I_term = pitch_kI * bound(pitch_area, -MAX_PITCH_AREA, MAX_PITCH_AREA); 
	D_term = pitch_kD * bound(pitch_rate, -MAX_PITCH_RATE, MAX_PITCH_RATE);

	pitch_correction = P_term + I_term + D_term;

	absolute_pitch_correction = fabsf(pitch_correction);
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
			PITCH_ANGLE_OFFSET = setpoint;

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
