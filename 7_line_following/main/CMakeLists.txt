# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "line_following.c" "tuning_http_server.c" "wifi_handler.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

register_component()

spiffs_create_partition_image(www "${CMAKE_CURRENT_SOURCE_DIR}/../frontend" FLASH_IN_PROJECT)
