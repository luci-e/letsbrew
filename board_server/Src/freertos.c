/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     

#include "app_bluenrg-ms.h"
#include "stm32f4xx_nucleo.h"
#include "globals.h"

#ifdef __cplusplus
 extern "C" {
#endif


/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osTimerId controller_timerHandle;

/* USER CODE BEGIN Variables */

#define TIMER_MS 1000
#define UARTRCVTIMEOUT 500

const unsigned int delay_ms[3] = { 1000,500,100};
unsigned volatile int blink_mode = 0;

extern UART_HandleTypeDef huart2;

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

void LedBlinkTask(void const * argument);
void UART_read_task(void const * argument);
void bluetooth_task(void const * argument);

extern void parsing_callback( int channel, char new_char );
extern void controller_callback (void const *argument);

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
#if !DISABLEBLUETOOTH
  osThreadDef(bluetooth_task, bluetooth_task, osPriorityNormal, 0, 2048);
#endif
  osThreadDef(UART_read_task__, UART_read_task, osPriorityNormal, 0, 512);
  osThreadDef(LedBlinkTask__, LedBlinkTask, osPriorityNormal, 0, 512);
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of controller_timer */
  osTimerDef(controller_timer, controller_callback);
  controller_timerHandle = osTimerCreate(osTimer(controller_timer), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */

  osTimerStart( controller_timerHandle, TIMER_MS);

  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
#if !DISABLEBLUETOOTH
  osThreadCreate(osThread(bluetooth_task), NULL);
#endif
  osThreadCreate(osThread(LedBlinkTask__), NULL);
  osThreadCreate(osThread(UART_read_task__), NULL);

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void bluetooth_task(void const * argument)
{

  /* init code for STMicroelectronics_BlueNRG-MS_1_0_0 */
  /* USER CODE BEGIN StartDefaultTask */

	MX_BlueNRG_MS_Init();
  /* Infinite loop */
  for(;;)
  {
	MX_BlueNRG_MS_Process();
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Application */

void LedBlinkTask(void const * argument)
{
  while(1)
  {
	  BSP_LED_Toggle(LED2);
	  //vTaskDelay(pdMS_TO_TICKS( delay_ms[blink_mode] ));
	  osDelay(delay_ms[blink_mode]);
  }
}

void UART_read_task(void const * argument)
{
  while(1)
  {
	  uint8_t next_char ='\0';
	  if(HAL_UART_Receive(&huart2, &next_char, (uint16_t) 1, UARTRCVTIMEOUT) == HAL_OK){
		  parsing_callback( 0, next_char );
	  }
	  else{
		  osDelay(50);
	  }
  }
}
     

#ifdef __cplusplus
 }
#endif

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
