/**
******************************************************************************
* @file    uart_support.c 
* @author  AAS / CL
* @version V1.0.0
* @date    18-May-2015
* @brief   This file implements the generic function for UART communication.
******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/*******************************************************************************
 * Include Files
*******************************************************************************/
#include <stdarg.h>
#include <string.h>
#include <hci.h>

#include "uart_support.h"
#include "main.h"
#include "stm32f4xx_hal.h"

/** @addtogroup X-CUBE-BLE1_Applications
 *  @{
 */

/** @defgroup SampleAppThT
 *  @{
 */
 
/** @defgroup UART_SUPPORT 
 * @{
 */
 
/*******************************************************************************
 * Macros
*******************************************************************************/

/******************************************************************************
 * Local Variable Declarations
******************************************************************************/

/** @defgroup UART_SUPPORT_Exported_Variables
 *  @{
 */
/******************************************************************************
 * Global Variable Declarations
******************************************************************************/

extern UART_HandleTypeDef huart2;
uint8_t uart_header[UARTHEADERSIZE];
uint8_t aRxBuffer[RXBUFFERSIZE];

void *profiledbgfile;

/**
 * @}
 */
 
/******************************************************************************
 * Function Declarations
******************************************************************************/

/** @defgroup UART_SUPPORT_Functions
 *  @{
 */
/******************************************************************************
 * Function Definitions 
******************************************************************************/
/**
 * @brief
 * @param
 */
void PRINT_MESG_UART(const char * format, ... )
{
  va_list ap;
  char buffer [128];
  int n;
  
  va_start(ap, format);
  n = vsnprintf ( buffer, 128, format, ap);
  va_end(ap);
  
  if(HAL_UART_Transmit(&huart2, (uint8_t*)buffer, n, 5000) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @}
 */
 
/**
 * @}
 */
 
 /**
 * @}
 */
 
 /**
 * @}
 */
 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
