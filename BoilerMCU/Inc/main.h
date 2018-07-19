#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#define MAX_BUF_SIZE 256
#define UART_BufferSize 256

 /* User can use this section to tailor USARTx/UARTx instance used and associated resources */
 /* Definition for USARTx clock resources */
 #define USARTx                           USART2
 #define USARTx_CLK_ENABLE()              __USART2_CLK_ENABLE();
 #define DMAx_CLK_ENABLE()                __DMA1_CLK_ENABLE()
 #define USARTx_RX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()
 #define USARTx_TX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()

 #define USARTx_FORCE_RESET()             __USART2_FORCE_RESET()
 #define USARTx_RELEASE_RESET()           __USART2_RELEASE_RESET()

 /* Definition for USARTx Pins */
 #define USARTx_TX_PIN                    GPIO_PIN_2
 #define USARTx_TX_GPIO_PORT              GPIOA
 #define USARTx_RX_PIN                    GPIO_PIN_3
 #define USARTx_RX_GPIO_PORT              GPIOA

 #define USARTx_TX_AF                     GPIO_AF7_USART2
 #define USARTx_RX_AF                     GPIO_AF7_USART2

#endif
