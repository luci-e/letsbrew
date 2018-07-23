#include "HAL.hpp"
#define NUMCHANNELS 2
#define BUFSIZE 512

#include "stream_reader.hpp"
#include "cmsis_os.h"

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

namespace letsbrew{

class Controller{

    unsigned int ticks_to_go;
    AUTOMERRORS last_error;
    AUTOMSTATE state;
    HAL * hal;
    //stream_reader *channels[NUMCHANNELS];//sr( 512 );
    stream_reader channels[2] = {stream_reader(BUFSIZE), stream_reader(BUFSIZE) };
	private:
        void timer_expired();
        void respond(unsigned int channel, char * msg);
        char response_message_buffer[BUFSIZE];
        void compile_response();
        int error_to_code(AUTOMERRORS err);
        osMutexId mutex;

    public:

        Controller(HAL * usehal);

        void tick();
        AUTOMERRORS abort();
        AUTOMERRORS brew();
        AUTOMERRORS keep_warm(unsigned int seconds);
        char * state_to_str();
        char * last_err_to_str();
        char * err_to_str(AUTOMERRORS err);

        void parse(unsigned int channel,char new_character);
};
}
