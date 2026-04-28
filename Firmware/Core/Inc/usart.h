#ifndef USART_H
#define USART_H

#include "stm32f4xx.h"
#include <stdint.h>

typedef enum { USART_SUCCESS, USART_FAILURE } USART_STATUS;

// Function to Initialize USART2 Module
USART_STATUS USART_Init(uint32_t baudrate);

// USART_STATUS USART_Transmit()

// USART_STATUS USART_Receive()

// Helper Function

USART_STATUS setBaudrate(uint32_t baudrate);

#endif
