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

//Functions for custom adjustments
float map(float x, float min_in, float max_in, float min_out, float max_out);

float constrain(float x, float lower_limit, float higher_limit);

float absolute(float number);

//Assign GPIO 2 and 15 to buttons
void enable_buttons();

//Check if the specified button is pressed, returns 1 if pressed
int pressed_switch(int button_num);

//Change ADC parameters if required 
void adc1_init();

//Initialise GPIOs for MCPWM
void mcpwm_gpio_initialize();

//Intialise MCPWM 
void mcpwm_initialize();

//Functions to control bot motion
void bot_forward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle1, float duty_cycle2);

void bot_backward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle1, float duty_cycle2);

void bot_spot_left(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle1, float duty_cycle2);

void bot_spot_right(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle1, float duty_cycle2);

void bot_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

#endif
