#include "usart.h"

USART_STATUS USART_Init(uint32_t baudrate) {
  /*USART 2 relies on GPIOA pins PA2 and PA3 must configure and enable*/
  RCC->AHB1ENR |=
      (1U << 1); // Sets bit 0 high, enables GPIOA AHB1 peripheral clock.
  RCC->AHB2ENR |= (1U << 17); // Enables USART2 Clock

  return USART_SUCCESS;
}
