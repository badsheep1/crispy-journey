#include "usart.h"

USART_STATUS USART_Init(uint32_t baudrate) {
  RCC->AHB1ENR |= (1U << 0);  // Enables GPIOA AHB1 peripheral clock.
  RCC->AHB1ENR |= (1U << 17); // Enables USART2 Clock

  // Enabling GPIO Pins:
  GPIOA->MODER &= ~(0b1111 << 4); // Clears the PA2 and PA3 bits
  GPIOA->MODER |=
      (0b1010 << 4); // Sets both PA2 and PA3 to Alternative Function Mode.

  return USART_SUCCESS;
}
