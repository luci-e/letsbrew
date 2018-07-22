
#include "stm32f4xx_hal.h"

class HAL{
    public:
        void stop_heater();
        void start_heater();
   
        int get_temperature();
        void set_blink_mode(unsigned int mode);

};

