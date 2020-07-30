WiFi Logger Examples
======================

Example app for ESP-IDF Component ![esp-wifi-logger](https://github.com/VedantParanjape/esp-wifi-logger)

## Instructions

```
idf.py menuconfig
idf.py flash
```

![](https://raw.githubusercontent.com/VedantParanjape/esp-component-examples/master/assets/esp_wifi_logger_edited.gif)

## Usage

### How to receive logs

* `sudo apt-get install netcat`     
  Netcat is required to receive logs    
* `nc -lu <PORT>`     
  Receive logs when ***udp*** is used as network protocol   
* `nc -l <PORT>`    
  Receive logs when ***tcp*** is used as network protocol   
* `websocat -s <IP_ADDRESS_OF_YOUR_MACHINE>:<PORT>`     
  Receive logs when ***websocket*** is used as network protocol   
* `websocat -s $(ip -o route get to 8.8.8.8 | sed -n 's/.*src \([0-9.]\+\).*/\1/p'):1234`     
  receive logs when ***websocket*** is used as network protocol, auto fills the ip address    
* **Example**: Assume, *port* is **1212** over TCP, command will be: `nc -l 1212`     

### How to use in ESP-IDF Projects
```
wifi_log_e() - Generate log with log level ERROR
wifi_log_w() - Generate log with log level WARN
wifi_log_i() - Generate log with log level INFO
wifi_log_d() - Generate log with log level DEBUG
wifi_log_v() - Generate log with log level VERBOSE
```

* Usage pattern same as, `ESP_LOGX()`
* Use `wifi_log()_x` function to print logs over wifi
* Example: `wifi_log(TAG, "%s", "logger test");`
* Call `start_wifi_logger()` in `void app_main()` to start the logger. Logging function `wifi_log` can be called to log messages

* Configure `menuconfig`
  * `Example Connection Configuration` *Set WiFi SSID and password*
  * `Component config`
  * `WiFi Logger configuration`
    * `Network Protocol (TCP/UDP/WEBSOCKET)` - Set network protocol to be used 
      * `UDP/TCP Network Protocol`
        * `Server IP Address` - Set the IP Address of the server which will receive log messages sent by ESP32
        * `Port` - Set the Port of the server
      * `WEBSOCKET Network Protocol`
        * `Websocket Server URI` - Sets the URI of Websocket server, where logs are to be sent

*IP Address of the server can be found out by running `ifconfig` on a linux machine*

## Configuration

```
idf.py menuconfig
```
* `Example Connection Configuration`
  * `WiFi SSID` -  Set WiFi SSID to connect
  * `WiFi Password` - Set WiFi Password

* `Component config`
  * `WiFi Logger configuration`
    * `Network Protocol (TCP/UDP/WEBSOCKET)` - Set network protocol to be used 
      * `UDP/TCP Network Protocol`
        * `Server IP Address` - Set the IP Address of the server which will receive log messages sent by ESP32
        * `Port` - Set the Port of the server
      * `WEBSOCKET Network Protocol`
        * `Websocket Server URI` - Sets the URI of Websocket server, where logs are to be sent
    * `Queue Size` - ***Advanced Config, change at your own risk*** Set the freeRTOS Queue size used to pass log messages to logger task.
    * `logger buffer size` - ***Advanced Config, change at your own risk*** Set the buffer size of char array used to generate log messages in ESP format


## Documentation

* ![Refer to component docs for further details](https://github.com/VedantParanjape/esp-wifi-logger/blob/master/README.md)
