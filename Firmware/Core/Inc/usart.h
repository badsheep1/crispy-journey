#ifndef USART_H
#define USART_H

#include "stm32f4xx.h"
#include <stdint.h>

#define HSI_FREQ 16000000U
#define HSE_FREQ 8000000U

#define BUFFER_SIZE 256

typedef struct circularBuffer Circle;

typedef enum { USART_FAILURE = 0, USART_SUCCESS } USART_STATUS;

// Function to Initialize USART2 Module
USART_STATUS USART_Init(uint32_t baudrate);

USART_STATUS USART_Transmit(uint8_t byte);

USART_STATUS USART_Receive(uint8_t *byte);

// Circular Buffer Manipulations

USART_STATUS enqueue(Circle *buffer, uint8_t byte);

USART_STATUS dequeue(Circle *buffer, uint8_t *byte);

// Other Function
void RX_RegisterCallback(void (*callback)(void));

void TX_RegisterCallback(void (*callback)(void));

// Helper Function

#endif
