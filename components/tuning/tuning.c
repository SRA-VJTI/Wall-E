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

#include "tuning.h"

static EventGroupHandle_t wifi_event_group;

const int CONNECTED_BIT = BIT0;

//Control function for Wifi initialisation and working
esp_err_t event_handler(void *ctx, system_event_t *event)
{
    //const int CONNECTED_BIT = BIT0;
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        printf("%s\n",ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

//Intialise WIFI for ESP32
void initialise_wifi(void)

{
    esp_err_t ret = nvs_flash_init();
    if (ret != ESP_OK) 
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = 
    {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );

}

//Display the webserver, and change values as set on the webpage
void http_server_netconn_serve(struct netconn *conn,float *setpoint,float *pitchKp,float *pitchKd,float *pitchKi,float *yaw_kP,float *yaw_kD,float *yaw_kI, float *forward_offset, float *forward_buffer)
{
  const static char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
  const static char http_index_hml[] = "<html><head><title>Wall E Tuning</title><style type=\"text/css\"> * {box-sizing: border-box;} body {background-color: #222;font-family: Monaco, Helvetica, Arial, sans-serif; padding: 0; color: #fff;}a {text-decoration: none;color: #fff;} .container {width:70%;margin: auto;padding: 40px;align-content: center;}table {width:100%;border-width: 4px;}td {padding: 10px; order-width: 0.5px;text-align: center;}h1 {text-align: center;}h2 {text-align: center;} .values-container {text-align:center; width:61%; margin:auto; padding:50px; align-content:center; border:4px solid #fff;} @media(max-width:768px) {.container {width:90%;} .values-container {width:80%}} .btn {margin:10px; padding: 0 5px; font-size:35px; background-color: #fff; border-radius: 50%; color: #000; font-family: monospace;}</style></head><body><h2>SRA 2019</h2><h1>Wall E Tuning</h1><div class=\"container\"><table border = 1; cellspacing=0; width=70%><tr><td><a class=\"btn\" href =\"b\">-</a></td><td>Pitch Kp</td><td><a class=\"btn\" href =\"a\">+</a></td></tr><tr><td><a class=\"btn\" href =\"d\">-</a></td><td>Pitch Kd</td><td><a class=\"btn\" href =\"c\">+</a></td></tr><tr><td><a class=\"btn\" href =\"g\">-</a></td><td>Pitch Ki</td><td><a class=\"btn\" href =\"e\">+</a></td></tr><tr><td><a class=\"btn\" href =\"i\">-</a></td><td>Yaw Kp</td><td><a class=\"btn\" href =\"h\">+</a></td></tr><tr><td><a class=\"btn\" href =\"k\">-</a></td><td>Yaw Kd</td><td><a class=\"btn\" href =\"j\">+</a></td></tr><tr><td><a class=\"btn\" href =\"m\">-</a></td><td>Yaw Ki</td><td><a class=\"btn\" href =\"l\">+</a></td></tr><tr><td><a class=\"btn\" href =\"o\">-</a></td><td>Setpoint</td><td><a class=\"btn\" href =\"n\">+</a></td></tr><tr><td><a class=\"btn\" href =\"q\">-</a></td><td>Forward Angle Offset</td><td><a class=\"btn\" href =\"p\">+</a></td></tr><tr><td><a class=\"btn\" href =\"s\">-</a></td><td>Forward Angle Buffer</td><td><a class=\"btn\" href =\"r\">+</a></td></tr></table></div></body></html>";
  char pKp[5];
  char pKd[5];
  char pKi[5];
  char yKp[5];
  char yKd[5];
  char yKi[5];
  char setPoint[5];
  char forwardOffset[5];
  char forwardBuffer[5];
  char enter[]="<br>";
  char pitchArr[]="Pitch Parameters";
  char PitchKpArr[]="Kp = ";
  char PitchKdArr[]="Kd = ";
  char PitchKiArr[]="Ki = ";
  char yawArr[]= "Yaw Parameters";
  char YawKpArr[]="Kp = ";
  char YawKdArr[]="Kd = ";
  char YawKiArr[]="Ki = ";
  char setpointArr[]="Setpoint = ";
  char forwardOffsetArr[]="Forward Offset = ";
  char forwardBufferArr[]="Forward Buffer = ";

  char valuesWrapperOpen[] = "<div class=\"values-container\">";
  char valuesWrapperClose[] = "</div";

  struct netbuf *inbuf;
  char *buf;
  u16_t buflen;
  err_t err;

  err = netconn_recv(conn, &inbuf);

  if (err == ERR_OK) {
    netbuf_data(inbuf, (void**)&buf, &buflen);

    if (buflen>=5 &&
        buf[0]=='G' &&
        buf[1]=='E' &&
        buf[2]=='T' &&
        buf[3]==' ' &&
        buf[4]=='/' ) 
    {

      if(buf[5]=='a'){
          *pitchKp+=0.1;
       }
       if(buf[5]=='b'){
          *pitchKp-=0.1;
       }
       if(buf[5]=='c'){
          *pitchKd+=0.1;
       }
       if(buf[5]=='d'){
          *pitchKd-=0.1;
       }
       if(buf[5]=='e'){
          *pitchKi+=0.01;
       }
       if(buf[5]=='g'){
          *pitchKi-=0.01;
       }
       if(buf[5]=='h'){
          *yaw_kP+=0.1;
       }
       if(buf[5]=='i'){
          *yaw_kP-=0.1;
       }
       if(buf[5]=='j'){
          *yaw_kD+=0.1;
       }
       if(buf[5]=='k'){
          *yaw_kD-=0.1;
       }
       if(buf[5]=='l'){
          *yaw_kI+=0.01;
       }
       if(buf[5]=='m'){
          *yaw_kI-=0.01;
       }
     
      if(buf[5]=='n'){
          *setpoint +=0.5;
       }
      if(buf[5]=='o'){
          *setpoint-=0.5;
       }
       if(buf[5]=='p'){
           *forward_offset+=0.5;
        }
        if(buf[5]=='q'){
            *forward_offset-=0.5;
        }
        if(buf[5]=='r'){
            *forward_buffer+=0.5;
        }
        if(buf[5]=='s'){
            *forward_buffer-=0.5;
         }

      sprintf(pKp, "%.1f", *pitchKp);
      sprintf(pKd, "%.1f", *pitchKd);
      sprintf(pKi, "%.2f", *pitchKi);
      sprintf(yKp, "%.1f", *yaw_kP);
      sprintf(yKd, "%.1f", *yaw_kD);
      sprintf(yKi, "%.2f", *yaw_kI);
      sprintf(setPoint, "%.1f", *setpoint);
      sprintf(forwardOffset, "%.2f", *forward_offset);
      sprintf(forwardBuffer, "%.1f", *forward_buffer);

  
      netconn_write(conn, http_html_hdr, sizeof(http_html_hdr)-1, NETCONN_NOCOPY);

      netconn_write(conn, http_index_hml, sizeof(http_index_hml)-1, NETCONN_NOCOPY);
      netconn_write(conn, valuesWrapperOpen, sizeof(valuesWrapperOpen)-1, NETCONN_NOCOPY);
      netconn_write(conn, pitchArr, sizeof(pitchArr)-1 , NETCONN_NOCOPY);
      netconn_write(conn, enter, sizeof(enter)-1, NETCONN_NOCOPY);
      netconn_write(conn, enter, sizeof(enter)-1, NETCONN_NOCOPY);
      netconn_write(conn, PitchKpArr, sizeof(PitchKpArr)-1 , NETCONN_NOCOPY);
      netconn_write(conn, pKp, sizeof(pKp)-1, NETCONN_NOCOPY);
      netconn_write(conn, enter, sizeof(enter)-1, NETCONN_NOCOPY);
      netconn_write(conn, PitchKdArr, sizeof(PitchKdArr)-1 , NETCONN_NOCOPY);
      netconn_write(conn, pKd, sizeof(pKd)-1, NETCONN_NOCOPY);
      netconn_write(conn, enter, sizeof(enter)-1, NETCONN_NOCOPY);
      netconn_write(conn, PitchKiArr, sizeof(PitchKiArr)-1 , NETCONN_NOCOPY);
      netconn_write(conn, pKi, sizeof(pKi)-1, NETCONN_NOCOPY);
      netconn_write(conn, enter, sizeof(enter)-1, NETCONN_NOCOPY);
      netconn_write(conn, enter, sizeof(enter)-1, NETCONN_NOCOPY);
      netconn_write(conn, yawArr, sizeof(yawArr)-1 , NETCONN_NOCOPY);
      netconn_write(conn, enter, sizeof(enter)-1, NETCONN_NOCOPY);
      netconn_write(conn, enter, sizeof(enter)-1, NETCONN_NOCOPY);
      netconn_write(conn, YawKpArr, sizeof(YawKpArr)-1 , NETCONN_NOCOPY);
      netconn_write(conn, yKp, sizeof(yKp)-1, NETCONN_NOCOPY);
      netconn_write(conn, enter, sizeof(enter)-1, NETCONN_NOCOPY);
      netconn_write(conn, YawKdArr, sizeof(YawKdArr)-1 , NETCONN_NOCOPY);
      netconn_write(conn, yKd, sizeof(yKd)-1, NETCONN_NOCOPY);
      netconn_write(conn, enter, sizeof(enter)-1, NETCONN_NOCOPY);
      netconn_write(conn, YawKiArr, sizeof(YawKiArr)-1 , NETCONN_NOCOPY);
      netconn_write(conn, yKi, sizeof(yKi)-1, NETCONN_NOCOPY);
      netconn_write(conn, enter, sizeof(enter)-1, NETCONN_NOCOPY);
      netconn_write(conn, enter, sizeof(enter)-1, NETCONN_NOCOPY);
      netconn_write(conn, setpointArr, sizeof(setpointArr)-1 , NETCONN_NOCOPY);
      netconn_write(conn, setPoint, sizeof(setPoint)-1, NETCONN_NOCOPY);
      netconn_write(conn, enter, sizeof(enter)-1, NETCONN_NOCOPY);   
      netconn_write(conn, forwardOffsetArr, sizeof(forwardOffsetArr)-1 , NETCONN_NOCOPY);
      netconn_write(conn, forwardOffset, sizeof(forwardOffset)-1, NETCONN_NOCOPY);
      netconn_write(conn, enter, sizeof(enter)-1, NETCONN_NOCOPY); 
      netconn_write(conn, forwardBufferArr, sizeof(forwardBufferArr)-1 , NETCONN_NOCOPY);
      netconn_write(conn, forwardBuffer, sizeof(forwardBuffer)-1, NETCONN_NOCOPY);
      netconn_write(conn, enter, sizeof(enter)-1, NETCONN_NOCOPY); 
      netconn_write(conn, valuesWrapperClose, sizeof(valuesWrapperClose)-1, NETCONN_NOCOPY);
    }

  }

  netconn_close(conn);

  netbuf_delete(inbuf);
}