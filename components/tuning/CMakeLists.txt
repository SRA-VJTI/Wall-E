idf_component_register(SRCS "tuning.c"
                       INCLUDE_DIRS "include"
                       REQUIRES app_trace app_update asio bootloader bootloader_support
                                bt cbor coap console cxx driver efuse esp32 esp_adc_cal
                                esp_common espcoredump esp_eth esp_event esp_gdbstub
                                esp_http_client esp_http_server esp_https_ota esp_https_server
                                esp_local_ctrl esp_ringbuf esp_rom esp-tls esptool_py
                                esp_websocket_client esp_wifi expat fatfs freemodbus freertos
                                heap idf_test jsmn json libsodium log lwip mbedtls mdns                          
                                mqtt newlib nghttp nvs_flash openssl partition_table
                                protobuf-c protocomm pthread sdmmc soc spiffs spi_flash
                                sra18 tcpip_adapter tcp_transport ulp unity vfs wear_levelling
                                wifi_provisioning wpa_supplicant xtensa)
