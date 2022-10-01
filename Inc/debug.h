#ifndef __DEBUG_H
#define __DEBUG_H

#include "config.h"

void app_debug_put_string(char *str);
void app_debug_put_buffer(uint8_t *pBuffer, int len);

#define LOG_COLOR_BLACK   "30"
#define LOG_COLOR_RED     "31"
#define LOG_COLOR_GREEN   "32"
#define LOG_COLOR_BROWN   "33"
#define LOG_COLOR_BLUE    "34"
#define LOG_COLOR_PURPLE  "35"
#define LOG_COLOR_CYAN    "36"
#define LOG_COLOR_WHITE   "37"
#define LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define LOG_BOLD(COLOR)   "\033[1;" COLOR "m"
#define LOG_RESET_COLOR   "\033[0m"
#define LOG_COLOR_E       LOG_COLOR(LOG_COLOR_RED)
#define LOG_COLOR_W       LOG_COLOR(LOG_COLOR_BROWN)
#define LOG_COLOR_I       LOG_COLOR(LOG_COLOR_GREEN)
#define LOG_COLOR_D       LOG_RESET_COLOR//LOG_COLOR(LOG_COLOR_WHITE)
#define LOG_COLOR_V

#define LOG_FORMAT(letter, format) LOG_COLOR_ ## letter #letter ":(%u) [%s]: " format LOG_RESET_COLOR "\r\n"

typedef enum {
    APP_LOG_NONE,
    APP_LOG_ERROR,
    APP_LOG_WARN,
    APP_LOG_INFO,
    APP_LOG_DEBUG,
} app_log_level_t;

#define APP_LOG_LEVEL(level, tag, format, ...) do {\
		if (level == APP_LOG_ERROR)         { app_log_write(LOG_FORMAT(E, format), app_log_get_time(), tag, ##__VA_ARGS__ );}\
		else if (level == APP_LOG_WARN)     { app_log_write(LOG_FORMAT(W, format), app_log_get_time(), tag, ##__VA_ARGS__ );}\
		else if (level == APP_LOG_INFO)     { app_log_write(LOG_FORMAT(I, format), app_log_get_time(), tag, ##__VA_ARGS__ );}\
		else if (level == APP_LOG_DEBUG)    { app_log_write(LOG_FORMAT(D, format), app_log_get_time(), tag, ##__VA_ARGS__ );}\
} while(0)
		
#define LREP(tag, format, ...)              APP_LOG_LEVEL(APP_LOG_DEBUG, tag, format, ##__VA_ARGS__)
#define LREP_INFO(tag, format, ...)         APP_LOG_LEVEL(APP_LOG_INFO, tag, format, ##__VA_ARGS__)
#define LREP_WARNING(tag, format, ...)      APP_LOG_LEVEL(APP_LOG_WARN, tag, format, ##__VA_ARGS__)
#define LREP_ERROR(tag, format, ...)        APP_LOG_LEVEL(APP_LOG_ERROR, tag, format, ##__VA_ARGS__)
		
void app_log_write(const char * format, ...) __attribute__ ((format (printf, 1, 2)));
		
		
uint32_t app_log_get_time(void);
		

#endif
