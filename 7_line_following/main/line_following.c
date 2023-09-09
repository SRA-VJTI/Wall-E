#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sra_board.h"
#include "tuning_http_server.h"

//#define debug

// C Headers
#include <stdio.h>
#include <math.h>
#include <wifi_handler.h>

//Limiting Variables
#define MAX_PITCH_CORRECTION (90.0f)
#define MAX_PITCH_AREA (850.0f)
#define MAX_PITCH_RATE (850.0f)

#define MAX_PWM (80.0f)
#define MIN_PWM (60.0f)

static const gpio_num_t enc_read_pinA = GPIO_NUM_33;
static const gpio_num_t enc_read_pinB = GPIO_NUM_32;

const int setpoint = 10;

/* Self Balancing Tuning Parameters
float forward_offset = 2.51f;
float forward_buffer = 3.1f;
*/

// Calculate the motor inputs according to angle of the MPU
int calculate_motor_command(const float ang_err, float *motor_cmd)
{
	
	int target = setpoint;
	int dir = 0;
	/** Error values **/
	// Stores pitch error of previous iteration
	// static float prev_ang_err = 0.0f;
	// Stores sum of product of errors with time interval obtained during each iteration
	static float ang_area = 0.0f;
	// Stores difference between current error and previous iteration error
	float ang_err_diff = 0.0f;

	/** Correction values **/
	// Variables for storing corrected values
	float ang_correction = 0.0f, absolute_pitch_correction = 0.0f;
	// Helper variables for calculating integral and derivative correction
	float ang_diff_rate = 0.0f;

	// Variables storing correction values of different error terms
	float P_term = 0.0f, I_term = 0.0f, D_term = 0.0f;

	// Evaluated delta(error)
	ang_err_diff = ang_err - target;

	// Evaluated area of the graph error vs time (cumulative error)
	ang_area += (ang_err);
	// evaluated delta(error)/delta(time) to calculate rate of change in error w.r.t time
	ang_diff_rate = ang_err_diff;

	// Calculating p,i and d terms my multuplying corresponding proportional constants
	P_term = read_pid_const().kp * ang_err;
	I_term = read_pid_const().ki * bound(ang_area, -MAX_PITCH_AREA, MAX_PITCH_AREA);
	D_term = read_pid_const().kd * bound(ang_diff_rate, -MAX_PITCH_RATE, MAX_PITCH_RATE);

	ang_correction = P_term + I_term + D_term;

	if (ang_correction > 0)
	{
		dir = 1;
	} else if (ang_correction < 0 ){
		dir = -1;
	} else{
		dir = 0;
	}

	absolute_pitch_correction = fabsf(ang_correction);

	*motor_cmd = bound(absolute_pitch_correction, 0, MAX_PITCH_CORRECTION);

	return dir;
	// prev_ang_err = ang_err;
}

int readEncoder() {
	int position = 0;
	int lastEncoded = 0;
	int encoded;

	encoded = (gpio_get_level(enc_read_pinA) << 1) | gpio_get_level(enc_read_pinB);
	int sum = (lastEncoded << 2) | encoded;
	
	if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
		position++;
	} else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
		position--;
	}
	// Hypothesis:
	// Wave 1 => 00 11 00 11 00 11 00 11 00 11, amplitude lasts for two timeframes
	// Wave 2 => 11 00 11 00 11 00 11 00 11 00, amplitude lasts for two timeframes

	// Use 'position' as the relative encoder value, and do whatever you need to do with it
	
	lastEncoded = encoded;

	return position;
}

//The main task to balance the robot
void balance_task(void *arg)
{


	/**
	 * Configuring GPIOs
	 */

	gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = 0,
        .pull_up_en = 1,
        .pin_bit_mask = ((1ULL<<enc_read_pinA) || (1ULL<<enc_read_pinB)),
    };
	

	int angle = 0;
	/**
	 * euler_angles are the complementary pitch and roll angles obtained from mpu6050
	 * mpu_offsets are the initial accelerometer angles at rest position
	*/
	// float euler_angle[2], mpu_offset[2] = {0.0f, 0.0f};

	/**
	 * 1. motor_cmd : Stores theoritically calculated correction values obtained with PID.
	 * 2. motor_pwm : Variable storing bounded data obtained from motor_cmd which will be used for
	                  giving actual correction velocity to the motors
	*/
	float motor_cmd, motor_pwm = 0.0f;

	// Pitch angle where you want to go - pitch_cmd, setpoint and mpu_offsets are linked to one another
	// float pitch_cmd = 0.0f;
// #ifdef CONFIG_ENABLE_OLED
//     // Initialising the OLED
//     ESP_ERROR_CHECK(init_oled());
// 	vTaskDelay(100);

//     // Clearing the screen
//     lv_obj_clean(lv_scr_act());
// #endif

		// Function to enable Motor driver A in Normal Mode
	enable_motor_driver(a, NORMAL_MODE);
	while (1)
	{
		angle = readEncoder();

		int dir = calculate_motor_command(angle, &motor_cmd);

		//bound PWM values between max and min
		motor_pwm = bound((motor_cmd), MIN_PWM, MAX_PWM);

		// Bot tilts downwards
		if ((dir) == 1)
		{
			// setting motor A0 with definite speed(duty cycle of motor driver PWM) in Forward direction
			set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, motor_pwm);
			// setting motor A1 with definite speed(duty cycle of motor driver PWM) in Forward direction
			set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, motor_pwm);
		}

		// Bot tilts Upwards
		if ((dir) == 0)
		{
			// setting motor A0 with definite speed(duty cycle of motor driver PWM) in Forward direction
			set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, motor_pwm);
			// setting motor A1 with definite speed(duty cycle of moto	r driver PWM) in Forward direction
			set_motor_speed(MOTOR_A_1, MOTOR_BACKWARD, motor_pwm);
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
		// ESP_LOGI("debug", "KP: %f ::  KI: %f  :: KD: %f :: Setpoint: %0.2f :: Roll: %0.2f | Pitch: %0.2f | PitchError: %0.2f", read_pid_const().kp, read_pid_const().ki, read_pid_const().kd, setpoint, euler_angle[0], euler_angle[1], ang_err);
		// ESP_LOGI("debug", "Pitch: %0.2f", pitch_angle);
// #ifdef CONFIG_ENABLE_OLED
// 		// Diplaying kp, ki, kd values on OLED
// 		if (read_pid_const().val_changed)
// 		{
// 			display_pid_values(read_pid_const().kp, read_pid_const().ki, read_pid_const().kd);
// 			reset_val_changed_pid_const();
// 		}
// #endif				
		vTaskDelay(10 / portTICK_PERIOD_MS);
		
	}


	// Remove the task from the RTOS kernel management
	vTaskDelete(NULL);
}

void app_main()
{
  // Starts tuning server for wireless control
	start_tuning_http_server();

	// xTaskCreate -> Create a new task and add it to the list of tasks that are ready to run
	xTaskCreate(&balance_task, "balance task", 4096, NULL, 1, NULL);
}