//C Headers
#include <stdio.h>
#include <math.h>
#include <time.h>

//Components
#include "sra18.h"
#include "mpu.h"
#include "tuning.h"

//Limiting Variables
#define MAX_PITCH_CORRECTION        (90.0)
#define MAX_PITCH_CUMULATIVE_ERROR (850.0)

#define MAX_PWM (100.0)
#define MIN_PWM (60.0)

//Array to store channels of ADC
adc1_channel_t channel[4] = {ADC_CHANNEL_7, ADC_CHANNEL_6, ADC_CHANNEL_0, ADC_CHANNEL_3};

//Line Following Tuning Parameters
float yaw_kP = 5.1;
float yaw_kI = 0.0;
float yaw_kD = 1.5;

//Self Balancing Tuning Parameters
float pitch_kP = 15.1; //5.85;       
float pitch_kI = 0.075; //95;          
float pitch_kD = 9.0;

float setpoint = 0.0;
float initial_acce_angle = 0.0;

float forward_offset = 2.51;
float forward_buffer = 3.1;

//Error and correction values

float left_pwm = 0.0, right_pwm = 0.0;

//Pitch and Roll angles
float complimentary_angle[2] = {0.0, 0.0};

float pitch_angle = 0.0;
float roll_angle = 0.0;

/*
  Calculate the motor inputs according to angle of the MPU
*/
void calculate_motor_command(const float pitch_cmd, const float pitch, float *motor_cmd)
{
    static float pitch_error = 0.0;
    static float pitch_error_difference = 0.0;
    static float pitch_error_cummulative = 0.0;

    static float prevpitch_error = 0.0;

    static float pitch_correction = 0.0;
    static float absolute_pitch_correction = 0.0;

    pitch_error              = pitch_cmd - pitch;
    pitch_error_difference   = pitch_error - prevpitch_error;
    pitch_error_cummulative += pitch_error;

    // integral term bounding, prevent windup
    if(pitch_error_cummulative > MAX_PITCH_CUMULATIVE_ERROR)
    {
      pitch_error_cummulative = MAX_PITCH_CUMULATIVE_ERROR;
    }
    else if(pitch_error_cummulative < -MAX_PITCH_CUMULATIVE_ERROR)
    {
      pitch_error_cummulative = -MAX_PITCH_CUMULATIVE_ERROR;
    }
    
    // PID loop, I term for trim, D term for modulating rate of control action
    pitch_correction = pitch_kP * pitch_error + pitch_kI * pitch_error_cummulative + pitch_kD * pitch_error_difference;
    
    absolute_pitch_correction = absolute(pitch_correction);
    *motor_cmd = constrain(absolute_pitch_correction, 0, MAX_PITCH_CORRECTION);

    // t = (t - 1)
    prevpitch_error = pitch_error;
}

//Create an HTTP server to tune variables wirelessly 
void http_server(void *arg)
{
    printf("%s\n", "http task");
    struct netconn *conn, *newconn;
    err_t err;
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, NULL, 80);
    netconn_listen(conn);
    do {
     err = netconn_accept(conn, &newconn);
     if (err == ERR_OK) {
       http_server_netconn_serve(newconn,&setpoint,&pitch_kP,&pitch_kD,&pitch_kI,&yaw_kP,&yaw_kD,&yaw_kI, &forward_offset, &forward_buffer);
       netconn_delete(newconn);
     }
    } while(err == ERR_OK);
    netconn_close(conn);
    netconn_delete(conn);
}

//The main task to balance the robot
void balance_task(void *arg)
{
    //Create buffers to store raw readings
    uint8_t acce_rd[BUFF_SIZE];
    uint8_t gyro_rd[BUFF_SIZE];
    int16_t acce_raw_value[BUFF_SIZE/2];
    int16_t gyro_raw_value[BUFF_SIZE/2];

    i2c_master_init();  //Initialise the I2C interface
    start_mpu();        //Intialise the MPU 
    mcpwm_initialize(); 
    
    vTaskDelay(100/ 10);

    while (1) 
    {
      initial_acce_angle = setpoint;
      
      calculate_angle(acce_rd, gyro_rd, acce_raw_value, gyro_raw_value, initial_acce_angle, &roll_angle, &pitch_angle);  //Function to calculate pitch angle based on intial accelerometer angle
      
      float pitch_cmd = 0.0;
      float motor_cmd = 0.0;
      calculate_motor_command(pitch_cmd, pitch_angle, &motor_cmd);
      
      //constrain PWM values between max and min
      left_pwm  = constrain(motor_cmd, MIN_PWM, MAX_PWM);
      right_pwm = constrain(motor_cmd, MIN_PWM, MAX_PWM);

      if (pitch_error > 1)
      {
          bot_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, left_pwm, right_pwm);
      }

      else if (pitch_error < -1)
      {                     
          bot_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, left_pwm, right_pwm);
      }
      else
      { 
          bot_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
      }

    }  //End of while loop

} //End of task

void app_main()
{
    initialise_wifi();
    //wait_till_wifi_connects();
    
    xTaskCreate(&balance_task,"balance task",100000,NULL,1,NULL);
    xTaskCreate(&http_server, "http_server", 10000, NULL, 5, NULL);
}
