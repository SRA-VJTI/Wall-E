//Components
#include "sra_board.h"
//#define debug

// C Headers
#include <stdio.h>
#include <math.h>

#include "tuning_websocket_server.h"

//Limiting Variables
#define MAX_PITCH_CORRECTION (90.0f)
#define MAX_PITCH_AREA (850.0f)
#define MAX_PITCH_RATE (850.0f)

#define MAX_PWM (80.0f)
#define MIN_PWM (60.0f)

/* Self Balancing Tuning Parameters
float forward_offset = 2.51f;
float forward_buffer = 3.1f;
*/

// Calculate the motor inputs according to angle of the MPU
void calculate_motor_command(const float pitch_error, float *motor_cmd)
{
	
	/** Error values **/
	// Stores pitch error of previous iteration
	static float prev_pitch_error = 0.0f;
	// Stores sum of product of errors with time interval obtained during each iteration
	static float pitch_area = 0.0f;
	// Stores difference between current error and previous iteration error
	float pitch_error_difference = 0.0f;

	/** Correction values **/
	// Variables for storing corrected values
	float pitch_correction = 0.0f, absolute_pitch_correction = 0.0f;
	// Helper variables for calculating integral and derivative correction
	float pitch_rate = 0.0f;

	// Variables storing correction values of different error terms
	float P_term = 0.0f, I_term = 0.0f, D_term = 0.0f;

	// Evaluated delta(error)
	pitch_error_difference = pitch_error - prev_pitch_error;

	// Evaluated area of the graph error vs time (cumulative error)
	pitch_area += (pitch_error);
	// evaluated delta(error)/delta(time) to calculate rate of change in error w.r.t time
	pitch_rate = pitch_error_difference;

	// Calculating p,i and d terms my multuplying corresponding proportional constants
	P_term = read_pid_const().kp * pitch_error;
	I_term = read_pid_const().ki * bound(pitch_area, -MAX_PITCH_AREA, MAX_PITCH_AREA);
	D_term = read_pid_const().kd * bound(pitch_rate, -MAX_PITCH_RATE, MAX_PITCH_RATE);

	pitch_correction = P_term + I_term + D_term;

    plot_graph(P_term, D_term, I_term, pitch_correction, pitch_error);
	/**
	 * Calculating absolute value of pitch_correction since duty cycle can't be negative. 
	 * Since it is a floating point variable, fabsf was used instead of abs
	*/
	absolute_pitch_correction = fabsf(pitch_correction);

	*motor_cmd = bound(absolute_pitch_correction, 0, MAX_PITCH_CORRECTION);
	prev_pitch_error = pitch_error;
}

//The main task to balance the robot
void balance_task(void *arg)
{
	/**
	 * euler_angles are the complementary pitch and roll angles obtained from mpu6050
	 * mpu_offsets are the initial accelerometer angles at rest position
	*/
	float euler_angle[2], mpu_offset[2] = {0.0f, 0.0f};

	float pitch_angle, pitch_error;

	/**
	 * 1. motor_cmd : Stores theoritically calculated correction values obtained with PID.
	 * 2. motor_pwm : Variable storing bounded data obtained from motor_cmd which will be used for
	                  giving actual correction velocity to the motors
	*/
	float motor_cmd, motor_pwm = 0.0f;

	// Pitch angle where you want to go - pitch_cmd, setpoint and mpu_offsets are linked to one another
	float pitch_cmd = 0.0f;
#ifdef CONFIG_ENABLE_OLED
	// Declaring the required OLED struct
    u8g2_t oled_config;

    // Initialising the OLED
    ESP_ERROR_CHECK(init_oled(&oled_config));
#endif

	// Ensure successful initialisation of MPU-6050
	if (enable_mpu6050() == ESP_OK)
	{
		// Function to enable Motor driver A in Normal Mode
		enable_motor_driver(a, NORMAL_MODE);
		while (1)
		{
			/**
			 * read_mpu6050(euler_angle, mpu_offset) : Checking for successful calculation of complementary pitch 
			 *											and roll angles based on intial accelerometer angle
			*/
			// Ensure required values are obtained from mpu6050
			if (read_mpu6050(euler_angle, mpu_offset) == ESP_OK)
			{
				// To read PID setpoint from tuning_http_server
				pitch_cmd = read_pid_const().setpoint;
				pitch_angle = euler_angle[1];
				pitch_error = pitch_cmd - pitch_angle;

				calculate_motor_command(pitch_error, &motor_cmd);

				//bound PWM values between max and min
				motor_pwm = bound((motor_cmd), MIN_PWM, MAX_PWM);

				// Bot tilts upwards
				if (pitch_error > 1)
				{
					// setting motor A0 with definite speed(duty cycle of motor driver PWM) in Backward direction
					set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, motor_pwm);
					// setting motor A1 with definite speed(duty cycle of motor driver PWM) in Backward direction
					set_motor_speed(MOTOR_A_1, MOTOR_BACKWARD, motor_pwm);
				}

				// Bot tilts downwards
				else if (pitch_error < -1)
				{
					// setting motor A0 with definite speed(duty cycle of motor driver PWM) in Forward direction
					set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, motor_pwm);
					// setting motor A1 with definite speed(duty cycle of motor driver PWM) in Forward direction
					set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, motor_pwm);
				}

				// Bot remains in desired region for vertical balance
				else
				{
					// stopping motor A0
					set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
					// stopping motor A1
					set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
				}

				//ESP_LOGI("debug","left_duty_cycle:  %f    ::  right_duty_cycle :  %f  :: error :  %f  correction  :  %f  \n",left_duty_cycle, right_duty_cycle, error, correction);
				ESP_LOGI("debug", "KP: %f ::  KI: %f  :: KD: %f :: Setpoint: %0.2f :: Roll: %0.2f | Pitch: %0.2f | PitchError: %0.2f", read_pid_const().kp, read_pid_const().ki, read_pid_const().kd, read_pid_const().setpoint, euler_angle[0], euler_angle[1], pitch_error);
				// ESP_LOGI("debug", "Pitch: %0.2f", pitch_angle);
#ifdef CONFIG_ENABLE_OLED
				// Diplaying kp, ki, kd values on OLED
				if (read_pid_const().val_changed)
				{
					display_pid_values(read_pid_const().kp, read_pid_const().ki, read_pid_const().kd, &oled_config);
					reset_val_changed_pid_const();
				}
#endif				
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
		}
	}

	// Remove the task from the RTOS kernel management
	vTaskDelete(NULL);
}

void app_main()
{
  // Starts tuning server for wireless control
	start_websocket_server();

	// xTaskCreate -> Create a new task and add it to the list of tasks that are ready to run
	xTaskCreate(&balance_task, "balance task", 4096, NULL, 1, NULL);
}