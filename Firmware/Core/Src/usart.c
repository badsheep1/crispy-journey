#include "usart.h"

struct circularBuffer {
  uint8_t data[BUFFER_SIZE];
  uint8_t head;
  uint8_t tail;
};

static Circle RX_Buffer;
static Circle TX_Buffer;

static void (*RX_Callback)(void);
static void (*TX_Callback)(void);

USART_STATUS USART_Init(uint32_t baudrate) {

  if (baudrate == 0) { // 0 Baudrate is invalid
    return USART_FAILURE;
  }

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

  uint8_t clkStatus = ((RCC->CFGR & (3U << 2)) >>
                       2); // Status for which CLK is used as APB input
  uint32_t fClk = 0;

  if (clkStatus == 0) {
    fClk = HSI_FREQ;
  } else if (clkStatus == 1) {
    fClk = HSE_FREQ;
  } else if (clkStatus == 2) {
    if ((RCC->PLLCFGR & (1U << 22)) >> 22) {
      fClk = HSE_FREQ;
    } else {
      fClk = HSI_FREQ;
    }
    uint32_t PLLN = (RCC->PLLCFGR & (0x1FF << 6)) >> 6;
    uint32_t PLLM = (RCC->PLLCFGR & 0x3F);
    uint32_t PLLP = (((RCC->PLLCFGR & (3U << 16)) >> 16) * 2) + 2;

    fClk *= PLLN;
    fClk /= (PLLM * PLLP);
  }

  uint32_t prescalar = 0;
  if ((RCC->CFGR & (1U << 12)) >> 12) {
    uint32_t PPRE = (RCC->CFGR & (7U << 10)) >> 10;
    PPRE = ~PPRE;
    PPRE &= 7U;
    prescalar = 16 / (1U << PPRE);
  } else { // Left most bit being 0, doesn't scale the clk input.
    prescalar = 1;
  }
  fClk /= prescalar;

  uint32_t USARTDIV =
      ((8 * (2 - ((USART2->CR1 >> 15) & 1U))) * fClk) / baudrate;

  USART2->BRR &= ~(0xFFFF); // Clearing the BRR Register

  if (USART2->CR1 & (1U << 15)) {
    if (USARTDIV & (1U << 3)) {
      USARTDIV += (1U << 3); // If the fraction rounds up and overflows into bit
                             // 3, we perform a addition operation to carry it
                             // over to the Mantissa.
    }
  }

  USART2->BRR |= USARTDIV;

  USART2->CR1 |= (1U << 13); // USART enabled

  // Initializing Circular Buffer Architecture
  RX_Buffer.head = RX_Buffer.tail = 0;
  TX_Buffer.head = TX_Buffer.tail = 0;

  return USART_SUCCESS;
}

// Circular Buffer Manipulations:

USART_STATUS enqueue(Circle *buffer, uint8_t byte) {
  uint8_t nextPos =
      (buffer->tail + 1) % BUFFER_SIZE; // Modulo by 256, wraps from 256 to 0

  if (nextPos != buffer->head) { // Checks if the buffer is full
    buffer->data[buffer->tail] = byte;
    buffer->tail = nextPos;
  } else {
    return USART_FAILURE;
  }
  return USART_SUCCESS;
}

USART_STATUS dequeue(Circle *buffer, uint8_t *byte) {
  if (buffer->head != buffer->tail) {
    *byte = buffer->data[buffer->head];
    buffer->head = (buffer->head + 1) % BUFFER_SIZE;
    return USART_SUCCESS;
  }
  return USART_FAILURE;
}

USART_STATUS USART_Transmit(uint8_t byte) {
  if (enqueue(&TX_Buffer, byte)) {
    USART2->CR1 |=
        (1U << 7); // Enabling TXIE after enqueuing ensuring non-empty buffer
    return USART_SUCCESS;
  }

  return USART_FAILURE;
}

USART_STATUS USART_Receive(uint8_t *byte) {
  __disable_irq(); // Disables all Interrupts to prevent a Race Condition on RX
                   // Buffer
  USART_STATUS returnStatus = dequeue(&RX_Buffer, byte);
  __enable_irq();
  return returnStatus;
}

// Interrupt Handler
void USART2_IRQHandler(void) {

  // RXNE Bit indicating Read data register is not empty
  if (USART2->SR & (1U << 5)) {
    uint8_t RX_data = USART2->DR;
    enqueue(&RX_Buffer, RX_data);
  }

  // TXE Bit indicating Transmit data register is empty
  if (USART2->SR & (1U << 7)) {
    uint8_t TX_data = 0;
    if (dequeue(&TX_Buffer, &TX_data)) {
      USART2->DR = TX_data;
    } else {
      USART2->CR1 &= ~(1U << 7); // Disabling TXIE
    }
  }
}

void RX_RegisterCallback(void (*callback)(void)) {
  if (callback != NULL) {
    RX_Callback = callback;
  }
}

void TX_RegisterCallback(void (*callback)(void)) {
  if (callback != NULL) {
    TX_Callback = callback;
  }
}
