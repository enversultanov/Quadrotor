//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "diag/Trace.h"

#include "Timer.h"
#include "BlinkLed.h"

#include "stm32f401_discovery.h"
#include "main.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F4 led blink sample (trace via ITM).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// Then demonstrates how to blink a led with 1Hz, using a
// continuous loop and SysTick delays.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the ITM output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

/* UART handler declaration */
UART_HandleTypeDef UartHandle;

/* Buffer used for transmission */
uint8_t aTxBuffer[] = " **** UART_TwoBoards_ComPolling ****  **** UART_TwoBoards_ComPolling ****  **** UART_TwoBoards_ComPolling **** ";

/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];

/* Private function prototypes -----------------------------------------------*/
static void Error_Handler(void);


// Definitions visible only within this translation unit.
namespace
{
  // ----- Timing definitions -------------------------------------------------

  // Keep the LED on for 2/3 of a second.
  constexpr Timer::ticks_t BLINK_ON_TICKS = Timer::FREQUENCY_HZ * 2 / 3;
  constexpr Timer::ticks_t BLINK_OFF_TICKS = Timer::FREQUENCY_HZ
      - BLINK_ON_TICKS;
}

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wunused-parameter"
//#pragma GCC diagnostic ignored "-Wmissing-declarations"
//#pragma GCC diagnostic ignored "-Wreturn-type"

float gyro_val[3]={0};

int
main(int argc, char* argv[])
{
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
    Error_Handler();
  }

  if(HAL_UART_Transmit(&UartHandle, (uint8_t*)"UART Initialized \r\n", COUNTOF("UART Initialized \r\n")-1, 5000)!= HAL_OK)
  {
    Error_Handler();
  }

  /*Gyro Init*/
  BSP_GYRO_Init();

  BSP_GYRO_GetXYZ(gyro_val);

  int c=0;
  char str[100];
  float gyro_read = 0;

//  int d1 = adc_read;            // Get the integer part (678).
//  float f2 = adc_read - d1;     // Get fractional part (0.01234567).
//  int d2 = trunc (f2 * 10000);   // Turn into integer (123).
//  float f3 = f2 * 10000 - d2;   // Get next fractional part (0.4567).
//  int d3 = trunc (f3 * 10000);   // Turn into integer (4567).

//   c=sprintf (str, "adc_read = %d.%04d%04d\r\n", d1, d2, d3);

  // Infinite loop
  while (1)
    {
//	HAL_Delay(500);
	BSP_LED_Toggle(LED3);

	BSP_GYRO_GetXYZ(gyro_val);

	gyro_read = gyro_val[0];

	int d1 = gyro_read;            // Get the integer part (678).
	float f2 = gyro_read - d1;     // Get fractional part (0.01234567).
	int d2 = trunc (f2 * 10000);   // Turn into integer (123).
	float f3 = f2 * 10000 - d2;   // Get next fractional part (0.4567).
	int d3 = trunc (f3 * 10000);   // Turn into integer (4567).

	c=sprintf (str, "Gyro X value = %d.%04d%04d\r\n", d1, d2, d3);



	/*##-2- Start the transmission process #####################################*/
	/* While the UART in reception process, user can transmit data through
	   "aTxBuffer" buffer */
//	if(HAL_UART_Transmit(&UartHandle, (uint8_t*)aTxBuffer, TXBUFFERSIZE, 5000)!= HAL_OK)
//	{
//	  Error_Handler();
//	}



	if(HAL_UART_Transmit(&UartHandle, (uint8_t*)str, c, 5000)!= HAL_OK)
	{
	  Error_Handler();
	}

    }
  // Infinite loop, never return.
}

#pragma GCC diagnostic pop

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
    /* Turn LED5 (RED) on */
    BSP_LED_On(LED5);
    while(1)
    {
    }
}

// ----------------------------------------------------------------------------
