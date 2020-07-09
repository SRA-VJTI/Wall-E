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

#include "sra18.h"

//Functions for custom adjustments
float map(float x, float min_in, float max_in, float min_out, float max_out)
{
    return (x - min_in) * (max_out - min_out) / (max_in - min_in) + min_out;
}

float constrain(float x, float lower_limit, float higher_limit)
{
    if(x < lower_limit)
        x = lower_limit;
    
    else if(x > higher_limit)
        x = higher_limit;

    return x;
}

float absolute(float number)
{
    if(number < 0)
    {
        return (-1)*number;
    }

    return number;
}

//Assign GPIO 39, 36 35 and 34 to buttons
void enable_buttons()
{
    gpio_set_direction(BUTTON_1,GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_1,GPIO_PULLUP_ONLY);
    gpio_set_direction(BUTTON_2,GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_2,GPIO_PULLUP_ONLY);
    gpio_set_direction(BUTTON_3,GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_3,GPIO_PULLUP_ONLY);
    gpio_set_direction(BUTTON_4,GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_4,GPIO_PULLUP_ONLY);
}

//Check if the specified button is pressed, returns 1 if pressed
int pressed_switch(int button_num)
{
    return !gpio_get_level(button_num);    
}

//Change ADC parameters if required
 void adc1_init(adc1_channel_t channel[4])
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    
    for(int i = 0;i < 4;i++)
    {
      adc1_config_channel_atten(channel[i], ADC_ATTEN_11db);
    }
}


//Initialise GPIOs for MCPWM
 void mcpwm_gpio_initialize()
{
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_UNIT0_PWM0A_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, GPIO_UNIT0_PWM0B_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, GPIO_UNIT0_PWM1A_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, GPIO_UNIT0_PWM1B_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM2A, GPIO_UNIT0_PWM2A_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM2B, GPIO_UNIT0_PWM2B_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0A, GPIO_UNIT1_PWM0A_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0B, GPIO_UNIT1_PWM0B_OUT);
}

//Intialise MCPWM 
 void mcpwm_initialize()
{
    mcpwm_gpio_initialize();
    printf("Configuring Initial Parameters of mcpwm...\n");
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 20000;    //frequency = 500Hz,
    pwm_config.cmpr_a = 0;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    printf("Configuring pwm_config...\n");
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure UNIT0 PWM0A & PWM0B with above settings
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);    //Configure UNIT0 PWM1A & PWM1B with above settings
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_2, &pwm_config);    //Configure UNIT0 PWM2A & PWM2B with above settings
    mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config);    //Configure UNIT1 PWM0A & PWM0B with above settings
    printf("Initialize pwm_init...\n");
}
// Functions to control motors
void motor_forward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle)
{
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_A, 0);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); 
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_B, duty_cycle);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); 
}
void motor_backward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle)
{
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_A, duty_cycle);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); 
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_B, 0);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); 
}
void motor_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_A, 0);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); 
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_B, 0);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); 
}
