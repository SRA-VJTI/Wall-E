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

#ifndef _MPU_H
#define _MPU_H

#include "sra18.h"

#define I2C_MASTER_SCL_IO    21   /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO    22    /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_1   /*!< I2C port number for master dev */
#define I2C_MASTER_TX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_MASTER_FREQ_HZ    100000     /*!< I2C master clock frequency */

#define MPU6050_ADDR  0x68    /*!< slave address for BH1750 sensor */
#define WRITE_BIT  I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT   I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN   0x1     /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS  0x0     /*!< I2C master will not check ack from slave */
#define ACK_VAL    0x0         /*!< I2C ack value */
#define NACK_VAL   0x1         /*!< I2C nack value */

#define ALPHA 0.9834

#define RAD_TO_DEG 57.27272727

#define BUFF_SIZE 6

#define ACCE_START_ADD 0x3B	//Accelerometer start address
#define GYRO_START_ADD 0x43	//gyroscope start address


//Initialise and power ON, MPU6050
esp_err_t mpu6050_init(i2c_port_t i2c_num);

//Read accelerometer values
esp_err_t mpu6050_read_acce(i2c_port_t i2c_num, uint8_t* data_rd, size_t size);

//Read gyroscope values
esp_err_t mpu6050_read_gyro(i2c_port_t i2c_num, uint8_t* data_rd, size_t size);

//Initialise the I2C bus and install driver to specified pins
void i2c_master_init();

//Display values stored in a buffer in heap
void disp_buf(int16_t* buf, int len);

//Shift 8-bit values to 16-bit variable
void shift_buf(uint8_t* buf_1, int16_t* buf_2, int len);

//Returns time in miliseconds elapsed since start of system
uint32_t msec();
int64_t esp_timer_get_time(void);

//Add complimentary filter on raw accelerometer and gyroscope values 
esp_err_t complimentary_filter(int16_t* acce_raw_value, int16_t* gyro_raw_value, float complimentary_angle[], int len, float initial_acce_angle);

//Proceed only when MPU is initialise
void start_mpu();

//Calculate roll and pitch angles of the MPU after applying the complimentary filter
void calculate_angle(uint8_t* acce_rd ,uint8_t* gyro_rd,int16_t* acce_raw_value,int16_t* gyro_raw_value, float initial_acce_angle,float *roll_angle,float *pitch_angle);

#endif