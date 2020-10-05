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

#ifndef _SRA18_H
#define _SRA18_h

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "driver/adc.h"
#include "driver/mcpwm.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#include "esp_adc_cal.h"
#include "esp_attr.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/api.h"

#include "nvs_flash.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

<<<<<<< HEAD
#define GPIO_UNIT0_PWM0A_OUT 32   //Set GPIO 32 as UNIT0_PWM0A 
#define GPIO_UNIT0_PWM0B_OUT 33   //Set GPIO 33 as UNIT0_PWM0A
#define GPIO_UNIT0_PWM1A_OUT 25   //Set GPIO 25 as UNIT0_PWM0B 
#define GPIO_UNIT0_PWM1B_OUT 26   //Set GPIO 26 as UNIT0_PWM0B 
#define GPIO_UNIT0_PWM2A_OUT 27   //Set GPIO 27 as UNIT0_PWM1A 
#define GPIO_UNIT0_PWM2B_OUT 14   //Set GPIO 14 as UNIT0_PWM1A 
#define GPIO_UNIT1_PWM0A_OUT 12   //Set GPIO 12 as UNIT1_PWM0A 
#define GPIO_UNIT1_PWM0B_OUT 13   //Set GPIO 13 as UNIT1_PWM0B 
#define BUTTON_1 39
#define BUTTON_2 36
#define BUTTON_3 35
#define BUTTON_4 34
#define led_pack_1 32 
#define led_pack_2 33
#define led_pack_3 25
#define led_pack_4 26
#define led_pack_5 27
#define led_pack_6 14
#define led_pack_7 12
#define led_pack_8 13
=======
#include "logger.h"

#define GPIO_PWM0A_OUT 27   //Set GPIO 15 as PWM0A - Enable
#define GPIO_PWM0B_OUT 14   //Set GPIO 16 as PWM0B 
#define GPIO_NUM0 25  //GPIO to input pin of motor driver
#define GPIO_NUM1 26  //GPIO to input pin of motor driver
#define GPIO_NUM2 16
#define GPIO_NUM3 17
#define BUTTON_1 2
#define BUTTON_2 15
#define LED_1 0
#define LED_2 5
>>>>>>> origin/dev

//Functions for custom adjustments
float map(float x, float min_in, float max_in, float min_out, float max_out);

float constrain(float x, float lower_limit, float higher_limit);

float absolute(float number);

//Assign GPIO 39, 36 35 and 34 to buttons
void enable_buttons();

//Check if the specified button is pressed, returns 1 if pressed
int pressed_switch(int button_num);

//Change ADC parameters if required 
void adc1_init();

//Initialise GPIOs for MCPWM
void mcpwm_gpio_initialize(int PARALLEL_MODE);

//Intialise MCPWM 
void mcpwm_initialize(int PARALLEL_MODE);

//Functions to control motor motion
void motor_forward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle);

void motor_backward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle);

void motor_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

#endif
