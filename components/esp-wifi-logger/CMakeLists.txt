set(srcs "wifi_logger.c" "utils.cpp" "udp_handler.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       REQUIRES nvs_flash tuning)