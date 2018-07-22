#include "HAL.hpp"

enum AUTOMSTATE{
    IDLE,
    BREWPREHEATING,
    BREWBREWING,
    ERR,
    KEEPWARMIDLE,
    KEEPWARMWARMING

};

enum AUTOMERRORS{
    NOERROR,
    BREWINPROGRESS,
    BREWTEMPERATURETOOHIGH,
    HEATERERROR,
    KEEPWARMTOOCOLD,
    KEEPWARMINPROGRESS
};

class Controller{

    unsigned int ticks_to_go;
    AUTOMERRORS last_error;
    AUTOMSTATE state;
    HAL * hal;
    private:
        void timer_expired();

    public:

        Controller(HAL * usehal);

        void tick();
        AUTOMERRORS abort();
        AUTOMERRORS brew();
        AUTOMERRORS keep_warm(unsigned int seconds);
        const char * state_to_str();
        const char * last_err_to_str();
        void parse(char * message);
};
