//C Headers
#include <stdio.h>
#include <math.h>
#include <time.h>

//Components
#include "sra18.h"
#include "mpu.h"
#include "tuning.h"


//Limiting Parameters
#define MAX_PITCH_CORRECTION 90
#define MAX_PITCH_CUMULATIVE_ERROR 850

#define MAX_PWM 100
#define MIN_PWM 60

#define MAX_PITCH_ERROR -2.5

static const char *TAG_BALANCE_PLUS_FOLLOW = "self_balance_with_line_follow";

//ADC Channels
adc1_channel_t channel[4] = {ADC_CHANNEL_7,ADC_CHANNEL_6,ADC_CHANNEL_0,ADC_CHANNEL_3};

//Line Following Tuning Parameters
float yaw_kP= 5.6;
float yaw_kI= 0;
float yaw_kD= 1.7;

//Self Balancing Tuning Parameters
float pitch_kP=  15.1;//5.85;       
float pitch_kI=  0.075;//95;          
float pitch_kD=  9;

float setpoint = -5.0	;//-3.5***-2.5
float initial_acce_angle = 0;
float forward_angle = 0;

float forward_offset = 0.25;//0.75***-2.251
float forward_buffer = 3;

//FOR BALANCING
bool balanced = false;
float absolute_pitch_angle = 0;

float pitch_angle=0,roll_angle=0,absolute_pitch_correction=0, pitch_error=0, prevpitch_error=0, pitchDifference=0, pitchCumulativeError=0, pitch_correction=0;

//FOR LINE FOLLOWING
float yaw_error=0, yaw_prev_error=0, yaw_difference=0, yaw_cumulative_error=0, yaw_correction=0;
int weights[4] = {3,1,-1,-3};

uint32_t adc_reading[4];
float sensor_value[4];

float left_pwm = 0, right_pwm = 0;

static void read_sensors()
{
  for(int i = 0; i < 4; i++)
    {
        adc_reading[i] = adc1_get_raw(channel[i]);
    }
}

static void calc_sensor_values()
{
    for(int i = 0; i < 4; i++)
    {
        sensor_value[i] = map(adc_reading[i], 1700, 4000, 0, 1000);
        sensor_value[i] = constrain(sensor_value[i],0,1000);
    }

}

static void calculate_yaw_error()
{
    int all_black_flag = 1;
    float weighted_sum = 0, sum = 0, pos = 0;
    
    for(int i = 0; i < 4; i++)
    {
        if(sensor_value[i] > 400)
        {
            all_black_flag = 0;
        }

        weighted_sum += (float)(sensor_value[i]) * (weights[i]);
        sum += sensor_value[i];
        
    }
    
    if(sum != 0)
    {
        pos = weighted_sum / sum;
    }

    if(all_black_flag == 1)
    {
        if(yaw_error > 0)
            pos = 2.5;
        else
            pos = -2.5;
    }

    yaw_error = pos;

}

static void calculate_yaw_correction()
{
    yaw_error *= 10;
    yaw_difference = (yaw_error - yaw_prev_error);
    yaw_cumulative_error += yaw_error;
    
    if(yaw_cumulative_error > 30)
    {
        yaw_cumulative_error = 30;
    }
    
    else if(yaw_cumulative_error < -30)
    {
        yaw_cumulative_error = -30;
    }

    yaw_correction = yaw_kP*yaw_error + yaw_kI*yaw_cumulative_error + yaw_kD*yaw_difference;
    yaw_prev_error = yaw_error;
}

void calculate_pitch_error()
{
    pitch_error = pitch_angle; 
    pitchDifference = (pitch_error - prevpitch_error);
    pitchCumulativeError += pitch_error;

    if(pitchCumulativeError>MAX_PITCH_CUMULATIVE_ERROR)
    {
      pitchCumulativeError = MAX_PITCH_CUMULATIVE_ERROR;
    }
    else if(pitchCumulativeError<-MAX_PITCH_CUMULATIVE_ERROR)
    {
      pitchCumulativeError = -MAX_PITCH_CUMULATIVE_ERROR;
    }
    
    pitch_correction = pitch_kP * pitch_error + pitchCumulativeError*pitch_kI + pitch_kD * pitchDifference;
    prevpitch_error = pitch_error;

    absolute_pitch_correction = absolute(pitch_correction);
    absolute_pitch_correction = constrain(absolute_pitch_correction,0,MAX_PITCH_CORRECTION);
}

//Create an HTTP server to tune variables wirelessly 
void http_server(void *arg)
{
    logI(TAG_BALANCE_PLUS_FOLLOW, "%s", "http task");
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

//Task for line following while self balancing
void balance_with_line_follow_task(void *arg)
{
    //Create buffers to store raw readings
    uint8_t acce_rd[BUFF_SIZE];
    uint8_t gyro_rd[BUFF_SIZE];
    int16_t acce_raw_value[BUFF_SIZE/2];
    int16_t gyro_raw_value[BUFF_SIZE/2];

    i2c_master_init();
    start_mpu();
    mcpwm_initialize();

    vTaskDelay(100/ 10);
    

    //SELF BALANCING AND LINE FOLLOWING
    while (1) 
    {

        calculate_angle(acce_rd,gyro_rd,acce_raw_value,gyro_raw_value,initial_acce_angle,&roll_angle,&pitch_angle);

        //Calulate PITCH and YAW error
        calculate_pitch_error();
        read_sensors();
        calc_sensor_values();
        calculate_yaw_error();
        calculate_yaw_correction();

        if(!balanced)
        {
            initial_acce_angle = setpoint;

            //constrain PWM values between max and min values
            right_pwm = constrain((absolute_pitch_correction),MIN_PWM,MAX_PWM);
            left_pwm = constrain((absolute_pitch_correction),MIN_PWM,MAX_PWM);

            // SET DIRECTION OF BOT FOR BALANCING
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
                initial_acce_angle = forward_angle;
                balanced = true;
            }
        }

        else
        {
            forward_angle = setpoint + forward_offset;

            //constrain PWM values between max and min values
            right_pwm = constrain((absolute_pitch_correction + yaw_correction), MIN_PWM, MAX_PWM);
            left_pwm = constrain((absolute_pitch_correction - yaw_correction), MIN_PWM, MAX_PWM);

            //Extra yaw correction during turns
            if(yaw_error>10)
            {
                right_pwm+=15;
                left_pwm-=15;   
            }
            else if(yaw_error<-10)
            {
                left_pwm+=15;
                right_pwm-=15;
            }

            // SET DIRECTION OF BOT FOR BALANCING
            if (pitch_error > 0)
            {   
                bot_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, left_pwm, right_pwm);
            }

            else if (pitch_error < 0)
            {
                bot_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, left_pwm, right_pwm);      
            }
            else
            { 
                bot_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
            }

            // Change intial_acce_angle back to setpoint if bot exceeds forward_angle buffer
            if(pitch_error>forward_buffer|| pitch_error < MAX_PITCH_ERROR)
            {
                initial_acce_angle = setpoint;
                balanced = false;
            }

        }

    }   //End of While Loop

}   //End of Task

void app_main()
{
    initialise_wifi();
    //wait_till_wifi_connects();

    xTaskCreate(&http_server,"server",10000,NULL,5,NULL);
    xTaskCreate(&balance_with_line_follow_task,"self_balancing with line_following",100000,NULL,1,NULL);
}
