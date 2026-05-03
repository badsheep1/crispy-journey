#ifndef USART_H
#define USART_H

#include "stm32f4xx.h"
#include <stdint.h>

#define HSI_FREQ 16000000U
#define HSE_FREQ 8000000U

#define BUFFER_SIZE 256

typedef struct circularBuffer Circle;

typedef enum { USART_SUCCESS, USART_FAILURE } USART_STATUS;

// Function to Initialize USART2 Module
USART_STATUS USART_Init(uint32_t baudrate);

// USART_STATUS USART_Transmit()

// USART_STATUS USART_Receive()

// Circular Buffer Manipulations

void enqueue(Circle *buffer);

// Helper Function

#endif
