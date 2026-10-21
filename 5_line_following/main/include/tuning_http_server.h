#ifndef TUNING_HTTP_SERVER_H
#define TUNING_HTTP_SERVER_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_spiffs.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "mdns.h"
#include "lwip/apps/netbiosns.h"
#include "esp_http_server.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include "cJSON.h"
#include "sdkconfig.h"
#include "wifi_handler.h"

#define MDNS_INSTANCE "walle pid tuning web server"
#define MDNS_HOST_NAME CONFIG_MDNS_HOST_NAME
#define WEB_MOUNT_POINT "/www"
#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFSIZE (10240)
#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

typedef struct pid_const
{
    float kp;
    float ki;
    float kd;
    bool val_changed;
} pid_const_t;

pid_const_t read_pid_const();
void reset_val_changed_pid_const();
void start_tuning_http_server();

typedef struct {
    uint32_t ms;
    int values[5], bits, phase, inverted, pending, decision, threshold;
    float left, right;
    bool ready, maze;
} lsa_telemetry_t;
void publish_lsa_telemetry(const lsa_telemetry_t *sample);

#endif