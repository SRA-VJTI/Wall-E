#include "utils.h"

const char log_level_char[5] = { 'E', 'W', 'I', 'D', 'V'};
const char log_level_color[5][7] = {"\e[31m", "\e[33m", "\e[32m", "\e[39m", "\e[39m"};

/**
 * @brief adds timestamp to the log message
 * 
 * @param log_level log level of the log message
 * @param timestamp timestamp provided by ESP in milliseconds
 * @param log_message log message to be sent through wifi
 * @return char* final log message with timestamp
 */
char* generate_log_message_timestamp(uint log_level, uint32_t timestamp, char* log_message)
{
    log_level = log_level%5;

    std::string log_string = std::string(log_level_color[log_level]) + log_level_char[log_level] + std::string(" (") + std::to_string(timestamp) + std::string(") ") + std::string(log_message) + std::string("\e[39m") + std::string("\n");

    char *c_log_string = (char*) malloc(sizeof(char)*(log_string.size()+1));
    memcpy(c_log_string, log_string.c_str(), log_string.size()+1);
    
    return c_log_string;
}