    /*

Copyright (c) 2018, Society of Robotics and Automation, VJTI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "mpu.h"

static float counter = 0;
static uint32_t timer = 0;
static float dt = 0;
static float complimentary_angle[2] = {0, 0};
static float acce_angle[2];
static float gyro_angle[2];
static float gyro_rate[2];

//Initialise and power ON, MPU6050
esp_err_t mpu6050_init(i2c_port_t i2c_num)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( MPU6050_ADDR << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x6B, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

//Read accelerometer values
esp_err_t mpu6050_read_acce(i2c_port_t i2c_num, uint8_t* data_rd, size_t size)
{
    if (size == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( MPU6050_ADDR << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, ACCE_START_ADD, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( MPU6050_ADDR << 1 ) | READ_BIT, ACK_CHECK_EN);
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

//Read gyroscope values
esp_err_t mpu6050_read_gyro(i2c_port_t i2c_num, uint8_t* data_rd, size_t size)
{
    if (size == 0) 
    {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( MPU6050_ADDR << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, GYRO_START_ADD, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( MPU6050_ADDR << 1 ) | READ_BIT, ACK_CHECK_EN);
    if (size > 1) 
    {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

//Initialise the I2C bus and install driver to specified pins
void i2c_master_init()
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

//Display values stored in a buffer
void disp_buf(int16_t* buf, int len)
{
    int i;
    for(i = 0; i < len; i++) {
        printf("%d ", buf[i]);
    }
    printf("\n");
}

//Shift 8-bit values to 16-bit variable
void shift_buf(uint8_t* buf_1, int16_t* buf_2, int len)
{
    buf_2[0] = ((buf_1[0] << 8) + buf_1[1]);
    buf_2[1] = ((buf_1[2] << 8) + buf_1[3]);
    buf_2[2] = ((buf_1[4] << 8) + buf_1[5]);
}

//Returns time in miliseconds elapsed since start of system
uint32_t msec()
{
    uint32_t time;
    time = esp_timer_get_time();
    return time;
}

//Add complimentary filter on raw accelerometer and gyroscope values 
esp_err_t complimentary_filter(int16_t* acce_raw_value, int16_t* gyro_raw_value, float complimentary_angle[], int len, float initial_acce_angle)
{
    int i;
    counter++;
    if(counter == 1) 
    {
        acce_angle[0] = (atan2(acce_raw_value[1], acce_raw_value[2]) * RAD_TO_DEG);
        acce_angle[1] = (atan2(-(acce_raw_value[0]), acce_raw_value[2]) * RAD_TO_DEG);
        for( i = 0; i < 2; i++) 
        {
            complimentary_angle[i] =  acce_angle[i];
        }
        timer = esp_timer_get_time();
        return ESP_OK;
    }
    dt = (float) (msec() - timer) / 1000000;
    timer = msec();
    acce_angle[0] = (atan2(acce_raw_value[1], acce_raw_value[2]) * RAD_TO_DEG);
    acce_angle[1] = (atan2(-(acce_raw_value[0]), acce_raw_value[2]) * RAD_TO_DEG);
    acce_angle[1] = acce_angle[1] - initial_acce_angle;
    for( i = 0; i < 2; i++) 
    {
        // printf("gyro:%f\n",gyro_angle[i] );
        // printf("acce:%f\n",acce_angle[i] );
        gyro_rate[i] = gyro_raw_value[i]/131;
        gyro_angle[i] = gyro_rate[i] * dt;
        complimentary_angle[i] = (ALPHA * (complimentary_angle[i] + gyro_angle[i])) + ((1-ALPHA) * acce_angle[i]);    

    }
    return ESP_OK;
}

//Proceed only when MPU is initialise
void start_mpu()
{
    
    int ret = mpu6050_init(I2C_MASTER_NUM);
    
    //CHECK IF MPU IS ACTIVE
    while(ret != ESP_OK) 
    {
        printf("INIT FAILED... Retry\n");
        vTaskDelay(100/ portTICK_RATE_MS);
        ret = mpu6050_init(I2C_MASTER_NUM);
    }
    printf("INIT SUCESS...\n"); 
}


//Calculate roll and pitch angles of the MPU after applying the complimentary filter
void calculate_angle(uint8_t* acce_rd ,uint8_t* gyro_rd,int16_t* acce_raw_value,int16_t* gyro_raw_value, float initial_acce_angle,float *roll_angle,float *pitch_angle)
{

    /*Read raw gyro values*/
    int ret = mpu6050_read_gyro(I2C_MASTER_NUM, gyro_rd, BUFF_SIZE);
    shift_buf(gyro_rd, gyro_raw_value, BUFF_SIZE/2);

    /*Read raw acce values*/
	ret = mpu6050_read_acce(I2C_MASTER_NUM, acce_rd, BUFF_SIZE);
    shift_buf(acce_rd, acce_raw_value, BUFF_SIZE/2);

    //Get pitch angle using complimentary filter
    complimentary_filter(acce_raw_value, gyro_raw_value, complimentary_angle, BUFF_SIZE/2,initial_acce_angle);
    
    *roll_angle = complimentary_angle[0];
    *pitch_angle = complimentary_angle[1];
}


