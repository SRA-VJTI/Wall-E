#include "udp_handler.h"

static const char *TAG = "udp_handler";

/**
 * @brief Manages UDP connection to the server
 * 
 * @param nm network_data struct which contains necessary data for a UDP connection
 * @return void
 **/
void network_manager(struct network_data* nm)
{
    nm->dest_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
    nm->dest_addr.sin_family = AF_INET;
    nm->dest_addr.sin_port = htons(PORT);
    nm->addr_family = AF_INET;
    nm->ip_protocol = IPPROTO_IP;
    inet_ntoa_r(nm->dest_addr.sin_addr, nm->addr_str, sizeof(nm->addr_str) - 1);

    nm->sock = socket(nm->addr_family, SOCK_DGRAM, nm->ip_protocol);
    if (nm->sock < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    }
    else
    {
        ESP_LOGI(TAG, "Socket created, connected to %s:%d", HOST_IP_ADDR, PORT);
    }
}

/**
 * @brief Sends data to the server through a UDP socket
 * 
 * @param nm A pointer to network_data struct
 * @param payload char array which contains data to be sent
 * @return int - returns -1 if sending failed, number of bytes sent if successfully sent the data
 **/
int send_data(struct network_data* nm, char* payload)
{
    int err = sendto(nm->sock, payload, strlen(payload), 0, (struct sockaddr *)&(nm->dest_addr), sizeof(nm->dest_addr));
    if (err < 0) 
    {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
    }
    else
    {
        ESP_LOGD(TAG, "%s", "Message sent");
    }
    
    return err;
}

/**
 * @brief Receives data from UDP server
 * 
 * @param nm network_data struct which contains connection info
 * @return char array which contains data received
 **/
char* receive_data(struct network_data* nm)
{
    struct sockaddr_in source_addr;
    socklen_t socklen = sizeof(source_addr);
    int len = recvfrom(nm->sock, nm->rx_buffer, sizeof(nm->rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

    if (len < 0) 
    {
        ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
        return NULL;
    }
    else
    {
        nm->rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
        ESP_LOGD(TAG, "Received %d bytes from %s:", len, nm->addr_str);
        ESP_LOGD(TAG, "%s", nm->rx_buffer);
        return nm->rx_buffer;
    }
}

/**
 * @brief Shutdown active connection, deallocate memory
 * 
 * @param nm tcp_network_data struct which contains connection info
 * @return void
 **/
void close_network_manager(struct network_data* nm)
{
    ESP_LOGI(TAG, "%s", "Shutting down socket");
    shutdown(nm->sock, 0);
    close(nm->sock);
    free(nm);
}