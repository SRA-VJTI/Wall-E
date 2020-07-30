#ifndef UDP_HANDLER_H
#define UDP_HANDLER_H

#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#define HOST_IP_ADDR CONFIG_SERVER_IP_ADDRESS
#define PORT CONFIG_SERVER_PORT

struct network_data
{
    char rx_buffer[128];
    char addr_str[128];
    int addr_family;
    int ip_protocol;
    struct sockaddr_in dest_addr;
    int sock;
};

void network_manager(struct network_data* nm);
int send_data(struct network_data* nm, char* payload);
char* receive_data(struct network_data* nm);
void close_network_manager(struct network_data* nm);

#endif
