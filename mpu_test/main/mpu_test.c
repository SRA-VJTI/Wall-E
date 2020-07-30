#include "sra18.h"
#include "mpu.h"

int setpoint = 0;
int initial_acce_angle = 0;
float pitch_angle = 0,roll_angle = 0;

void mpu_task(void *arg)
{
    //Create buffers to store raw readings
    uint8_t acce_rd[BUFF_SIZE];
    uint8_t gyro_rd[BUFF_SIZE];
    int16_t acce_raw_value[BUFF_SIZE/2];
    int16_t gyro_raw_value[BUFF_SIZE/2];

    i2c_master_init();  //Initialise the I2C interface
    start_mpu();        //Intialise the MPU 
    
    while(1)
    {

      //Calculate yaw and pitch angles
      calculate_angle(acce_rd,gyro_rd,acce_raw_value,gyro_raw_value,initial_acce_angle,&roll_angle,&pitch_angle);  //Function to calculate pitch angle based on intial accelerometer angle
      printf("ROLL ANGLE %f\t", roll_angle);
      printf("PITCH ANGLE %f\n",pitch_angle);
       
    }   

}

void app_main()
{
  xTaskCreate(mpu_task,"mpu_task",4096,NULL,1,NULL);
}

