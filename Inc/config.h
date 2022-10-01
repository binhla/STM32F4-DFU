#ifndef __CONFIG_H
#define __CONFIG_H

#include "main.h"
#include "stm32f4xx_hal.h"
#include <string.h>

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

#if 1
#define UART_DEBUG huart1
#define DEBUG_UART_INSTANCE USART1

#define UART_DFU	huart2
#define DFU_UART_INSTANCE		USART2
#else
#define UART_DEBUG huart2
#define DEBUG_UART_INSTANCE USART2

#define UART_DFU	huart1
#define DFU_UART_INSTANCE		USART1
#endif

#endif
