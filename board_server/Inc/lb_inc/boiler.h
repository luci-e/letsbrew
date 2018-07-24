
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f401xe.h"

#define BOILERHEATERPORT GPIOA
#define BOILERHEATERPIN GPIO_PIN_5


#ifdef __cplusplus
 extern "C" {
#endif


void heater_on();
void heater_off();
int read_temperature();
void led_blink_mode(unsigned int mode);

#ifdef __cplusplus
}
#endif
