/*
 * uart.cpp
 *
 *  Created on: 20 okt 2014
 *      Author: ensul1
 */
#include "uart.h"

/* UART handler declaration */
UART_HandleTypeDef UartHandle;

char str[100];

/* Buffer used for transmission */
uint8_t aTxBuffer[] = " **** UART_TwoBoards_ComPolling ****  **** UART_TwoBoards_ComPolling ****  **** UART_TwoBoards_ComPolling **** ";

/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];

void UART_Init(void){
/*##-1- Configure the UART peripheral ######################################*/
/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
/* UART1 configured as follow:
    - Word Length = 8 Bits
    - Stop Bit = One Stop bit
    - Parity = None
    - BaudRate = 9600 baud
    - Hardware flow control disabled (RTS and CTS signals) */
UartHandle.Instance        = USARTx;
UartHandle.Init.BaudRate   = 9600;
UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
UartHandle.Init.StopBits   = UART_STOPBITS_1;
UartHandle.Init.Parity     = UART_PARITY_NONE;
UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
UartHandle.Init.Mode       = UART_MODE_TX_RX;

if(HAL_UART_Init(&UartHandle) != HAL_OK)
{
  /* Turn LED5 (RED) on */
  BSP_LED_On(LED5);
  while(1)
  {
  }
}
}

void UART_Float_TX(float *pData){
  int c = Float2String(*pData);

  if(HAL_UART_Transmit(&UartHandle, (uint8_t*)str, c, 5000)!= HAL_OK)
  {
    /* Turn LED5 (RED) on */
    BSP_LED_On(LED5);
    while(1)
    {
    }
  }
}

void UART_TX(uint8_t *pData, uint16_t Size){
  if(HAL_UART_Transmit(&UartHandle, pData, Size, 5000)!= HAL_OK)
  {
    /* Turn LED5 (RED) on */
    BSP_LED_On(LED5);
    while(1)
    {
    }
  }
}

int Float2String(float float_val){
  int d1 = float_val;            // Get the integer part (678).
  float f2 = float_val - d1;     // Get fractional part (0.01234567).
  int d2 = trunc (f2 * 10000);   // Turn into integer (123).
  float f3 = f2 * 10000 - d2;   // Get next fractional part (0.4567).
  int d3 = trunc (f3 * 10000);   // Turn into integer (4567).

  return sprintf (str, "Value = %d.%04d%04d\r\n", d1, d2, d3);
}
