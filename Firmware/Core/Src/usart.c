#include "usart.h"

USART_STATUS USART_Init(uint32_t baudrate) {
  /* Configuring Peripheral Clocks */
  RCC->AHB1ENR |= (1U << 0);  // Enables GPIOA AHB1 peripheral clock.
  RCC->APB1ENR |= (1U << 17); // Enables USART2 Clock

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

  uint8_t clkStatus = ((RCC->CFGR & (3U << 2)) >> 2);
  uint32_t sysClk = 0;
  if (clkStatus == 0) {
    sysClk = HSI_FREQ;
  } else if (clkStatus == 1) {
    sysClk = HSE_FREQ;
  } else if (clkStatus == 2) {
    if ((RCC->PLLCFGR & (1U << 22)) >> 22) {
      sysClk = HSE_FREQ;
    } else {
      sysClk = HSI_FREQ;
    }
    uint32_t PLLN = (RCC->PLLCFGR & (0x1FF << 6)) >> 6;
    uint32_t PLLM = (RCC->PLLCFGR & 0x3F);
    uint32_t PLLP = (((RCC->PLLCFGR & (3U << 16)) >> 16) * 2) + 2;

    sysClk *= PLLN;
    sysClk /= (PLLM * PLLP);
  }

  USART2->CR1 |= (1U << 13); // USART enabled

  return USART_SUCCESS;
}
