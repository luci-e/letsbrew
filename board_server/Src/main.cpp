#include "init.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include <string.h>

#include "controller.hpp"

Controller * c;


void callback (TimerHandle_t xTimer){
	c->tick();
}

void parsingCallback(int chan,uint8_t msg){
	c->parse(chan,msg);
}

int main(){

	HAL hal;
	c = new Controller(&hal);
	init(callback,parsingCallback);
	//HAL_Init();

}

