//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

//#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "diag/Trace.h"

#include "Timer.h"
#include "BlinkLed.h"

#include "stm32f401_discovery.h"
#include "main.h"
#include "uart.h"

extern char *str;

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

/* Private function prototypes -----------------------------------------------*/
static void Error_Handler(void);

int sampleNum = 500;
int dc_offset=0;
unsigned long time;
int sampleTime=10;
int rate;
int prev_rate=0;
float angle[]={0};

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

  UART_Init();

  UART_TX((uint8_t*)"UART Initialized \r\n", COUNTOF("UART Initialized \r\n")-1);

  /*Gyro Init*/
  BSP_GYRO_Init();

  BSP_GYRO_GetXYZ(gyro_val);

  //Calculate initial DC offset and noise level of gyro
  for(int n=0;n<sampleNum;n++){
      BSP_GYRO_GetXYZ(gyro_val);
      dc_offset+=(int)gyro_val[2];
  }
  dc_offset=dc_offset/sampleNum;

  // Infinite loop
  while (1)
    {
//	HAL_Delay(500);
	BSP_LED_Toggle(LED3);

	BSP_GYRO_GetXYZ(gyro_val);

	// Every 10 ms take a sample from the gyro
	if(HAL_GetTick() - time > sampleTime) {
	    time = HAL_GetTick();
	    BSP_GYRO_GetXYZ(gyro_val);
	    rate = ((int)gyro_val[2]-dc_offset)/100;

	    angle[0] += ((double)(prev_rate + rate)* sampleTime) / 2000;

	    // remember the current speed for the next loop rate integration.
	    prev_rate = rate;

	    // Keep our angle between 0-359 degrees
	    if (angle[0] < 0) angle[0] += 360;
	    else if (angle[0] >= 360) angle[0] -= 360;

	}

	UART_Float_TX(angle);

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
