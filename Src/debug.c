#include "debug.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"

void app_debug_put_string(char *str) {
	while (*str != '\0') {
		HAL_UART_Transmit(&UART_DEBUG, (uint8_t *)str, 1, 0xFFFF);
		str++;
	}
}

void app_debug_put_buffer(uint8_t *pBuffer, int len) {
	HAL_UART_Transmit(&UART_DEBUG, pBuffer, len, 0xFFFF);
}

void app_log_write(const char * format, ...) {
	static char log_buffer[256] = {0}; 
	va_list list;
  va_start(list, format);
  vsprintf(log_buffer, format, list);
  va_end(list);
	app_debug_put_string(log_buffer);
	memset(log_buffer, 0, 256);
}

uint32_t app_log_get_time(void) {
	return HAL_GetTick();
}

