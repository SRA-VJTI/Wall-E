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

#ifndef _TUNING_H
#define _TUNING_h

#include "sra18.h"

#define EXAMPLE_WIFI_SSID CONFIG_ESP_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_ESP_WIFI_PASSWORD

//Control function for Wifi initialisation and working
esp_err_t event_handler(void *ctx, system_event_t *event);

//Intialise WIFI for ESP32
void initialise_wifi();

//Display the webserver, and change values as set on the webpage
void http_server_netconn_serve(struct netconn *conn,float *setpoint,float *pitchKp,float *pitchKd,float *pitchKi,float *yaw_kP,float *yaw_kD,float *yaw_kI, float *forward_offset, float *forward_buffer);

#endif
