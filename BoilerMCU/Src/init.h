#include "FreeRTOS.h"
#include "timers.h"

#ifdef __cplusplus
 extern "C" {
#endif




int init(TimerCallbackFunction_t pxCallbackFunction,void(*parserCallback)(char*));

#ifdef __cplusplus
}
#endif
