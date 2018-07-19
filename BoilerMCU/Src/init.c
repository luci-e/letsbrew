//#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include <string.h>


#ifdef __cplusplus
 extern "C" {
#endif

const unsigned int delay_ms[3] = { 1000,500,100};
unsigned int blink_mode = 0;

volatile uint8_t UART_RxBuffer[UART_BufferSize];
volatile uint8_t UART_TxBuffer[UART_BufferSize];
static char dataOut[MAX_BUF_SIZE];
const uint32_t Usart_BaudRate = 115200;
UART_HandleTypeDef UartHandle;

void SystemClock_Config(void);
void USARTConfig(void);

/* Tasks -----------------------------------------------------------*/

static void prvAutoReloadTimerCallback( TimerHandle_t);

void LedBlinkTask(void const * argument);
void UART_read_task(void const * argument);

/* The periods assigned to the one-shot and auto-reload timers are 3.300 second and half a second, respectively. */

#define mainAUTO_RELOAD_TIMER_PERIOD pdMS_TO_TICKS( 1000 )

void (*pkb)(char*);

int init(TimerCallbackFunction_t pxCallbackFunction,void(*parserCallback)(char*))
{
	pkb = parserCallback;
	HAL_Init();
	SystemClock_Config();
	BSP_LED_Init(LED2);
	USARTConfig();

    TimerHandle_t xAutoReloadTimer;
    BaseType_t xTimer2Started;

    /* Create the auto-reload timer, storing the handle to the created timer in xAutoReloadTimer. */
    //xAutoReloadTimer = xTimerCreate( "AutoReload", mainAUTO_RELOAD_TIMER_PERIOD, pdTRUE, 0, prvAutoReloadTimerCallback );
    xTaskCreate((void *)LedBlinkTask, "BLINK", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate((void *)UART_read_task, "UART", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    xAutoReloadTimer = xTimerCreate( "AutoReload", mainAUTO_RELOAD_TIMER_PERIOD, pdTRUE, 0, pxCallbackFunction );
    /* Check the software timers were created. */
    if( xAutoReloadTimer != NULL ) {
        /* Start the software timers, using a block time of 0 (no block time). */

        xTimer2Started = xTimerStart( xAutoReloadTimer, 0 );

        if( xTimer2Started == pdPASS ) {
            /* Start the scheduler. */
            vTaskStartScheduler();
        }
    }
    /* As always, this line should not be reached. */
    while(1) {}
}
static void prvAutoReloadTimerCallback( TimerHandle_t xTimer ){



}

void LedBlinkTask(void const * argument)
{
  while(1)
  {
	  BSP_LED_Toggle(LED2);
	  vTaskDelay(pdMS_TO_TICKS( delay_ms[blink_mode] ));
  }
}
void UART_read_task(void const * argument)
{
  while(1)
  {
	  char * msg = "read from uart\n";
	  vTaskDelay(pdMS_TO_TICKS( 1000 ));
	  pkb(msg);
  }
}

static void prvAutoReloadTimerCallbackOld( TimerHandle_t xTimer )
{
    TickType_t xTimeNow;
    /* Obtain the current tick count. */
    xTimeNow = xTaskGetTickCount();
    /* Output a string to show the time at which the callback was executed. */
	snprintf( dataOut, MAX_BUF_SIZE, "Timer Auto Reload Fired: %lu\r\n", xTimeNow);
	HAL_UART_Transmit( &UartHandle, ( uint8_t * )dataOut, strlen( dataOut ), 5000 );
}

void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

void USARTConfig( void )
{

  GPIO_InitTypeDef GPIO_InitStruct;

  USARTx_TX_GPIO_CLK_ENABLE();
  USARTx_RX_GPIO_CLK_ENABLE();
  USARTx_CLK_ENABLE();

  GPIO_InitStruct.Pin       = USARTx_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = USARTx_TX_AF;

  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin       = USARTx_RX_PIN;
  GPIO_InitStruct.Alternate = USARTx_RX_AF;

  HAL_GPIO_Init( USARTx_RX_GPIO_PORT, &GPIO_InitStruct );

  UartHandle.Instance        = USARTx;
  UartHandle.Init.BaudRate   = Usart_BaudRate;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;

  if ( HAL_UART_Init( &UartHandle ) != HAL_OK )
  {
    while( 1 );
  }

  UartHandle.pRxBuffPtr = ( uint8_t * )UART_RxBuffer;
  UartHandle.RxXferSize = UART_BufferSize;
  UartHandle.ErrorCode  = HAL_UART_ERROR_NONE;
}

void _Error_Handler(char *file, int line)
{
  while(1)
  {
	  BSP_LED_Toggle(LED2);
	  vTaskDelay(pdMS_TO_TICKS( 50 ));
  }
}


#ifdef __cplusplus
 }
#endif


#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ 
}
#endif
