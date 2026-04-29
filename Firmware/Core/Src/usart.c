#include "usart.h"

USART_STATUS USART_Init(uint32_t baudrate) {
  /* Configuring Peripheral Clocks */
  RCC->AHB1ENR |= (1U << 0);  // Enables GPIOA AHB1 peripheral clock.
  RCC->AHB1ENR |= (1U << 17); // Enables USART2 Clock

  /* Configuring GPIO Pins for USART */
  GPIOA->MODER &= ~(0b1111 << 4); // Clears the PA2 and PA3 bits
  GPIOA->MODER |=
      (0b1010 << 4); // Sets both PA2 and PA3 to Alternative Function Mode.

  GPIOA->AFR[0] &= ~(0xFF << 8);
  GPIOA->AFR[0] |= (0x77 << 8);

  /* Configuring USART Register */
  USART2->CR1 |= (1U << 2); // Receiver enabled
  USART2->CR1 |= (1U << 3); // Transmitter enabled

  USART2->CR1 |= (1U << 5); // RX interrupt enabled
  USART2->CR1 |= (1U << 6); // TC interrupt enabled
  USART2->CR1 |= (1U << 7); // TXE Interrupt enabled

  uint32_t fclk = HAL_RCC_GetPCLK1Freq();
  uint32_t usartDIV = fclk / baudrate;

  uint32_t fraction = usartDIV % 16;
  uint32_t mantissa = (usartDIV >> 4);

  USART2->BRR = (mantissa << 4) & 0xFFF0 | (fraction & 0x000F);

  USART2->CR1 |= (1U << 13); // USART enabled

  return USART_SUCCESS;
}
