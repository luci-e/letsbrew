/*
 * uart_support.c
 *
 *  Created on: 22 Jul 2018
 *      Author: lucie
 */

#include "uart_support.h"

void PRINT_MESG_UART(const char * format, ... ){
	va_list ap;
	char buffer[TX_BUFFER_SIZE];
	int n;

	va_start(ap, format);
	n = vsnprintf ( buffer, TX_BUFFER_SIZE, format, ap) + 1;

	if(HAL_UART_Transmit(&huart2, (uint8_t*)buffer, n, 5000) != HAL_OK) {
		Error_Handler();
	}

	va_end(ap);
}
