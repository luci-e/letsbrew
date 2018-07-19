

#include <iostream>
#include "controller.hpp"
#define TIMETOBREW 60
#define BREWSTARTTEMP 80
#define BREWMAXTOSTART 50
#define KWKEEPABOVE 60
#define KWKEEPBELOW 65
#define KWMINTOSTART 50

#define seconds_to_ticks(x) x * 1000

using namespace std;





Controller::Controller(HAL * usehal){
	hal = usehal;
    state = IDLE;
    last_error = NOERROR;
}

const char * Controller::last_err_to_str(){
    switch(state){
        case NOERROR:
            return "No error";
        case BREWINPROGRESS:
            return "Brewing already in progress";
        case BREWTEMPERATURETOOHIGH:
            return "Unable to start brewing, heater too hot!";
        case HEATERERROR:
            return "Heater error";
        case KEEPWARMTOOCOLD:
            return "Unable to keep-warm, too cold";
        case KEEPWARMINPROGRESS:
            return "Keep-warm already in progress";
        default: break;

    }
}



const char * Controller::state_to_str(){
    switch(state){
        case IDLE:
            return "IDLE";
            break;
        case BREWPREHEATING:
            return "Brewing preheating";
            break;
        case BREWBREWING:
            return "Brewing";
            break;
        case KEEPWARMWARMING:
            return "Keep-warm active";
            break;
        case KEEPWARMIDLE:
            return "Keep-warm idle";
            break;
        case ERR:
            return "Error state";
            break;
        default: break;

    }
}



void Controller::abort(){
    hal->stop_heater();
    state = IDLE;
    last_error = NOERROR;
    ticks_to_go = 0;
}
void Controller::keep_warm(unsigned int seconds){
    switch(state){
        case IDLE:
            if(hal->get_temperature() < KWMINTOSTART){
                last_error = KEEPWARMTOOCOLD ;
            }
            else{
                state = KEEPWARMIDLE;
                last_error =NOERROR;
                ticks_to_go = seconds_to_ticks(seconds);
            }
            break;
        case BREWPREHEATING:
            last_error = BREWINPROGRESS;
            break;
        case BREWBREWING:
            last_error = BREWINPROGRESS;
            break;
        case KEEPWARMWARMING:
            last_error = KEEPWARMINPROGRESS;
            break;
        case KEEPWARMIDLE:
            last_error = KEEPWARMINPROGRESS;
            break;
        case ERR:
            break;
        default:break ;

    }
}
void Controller::brew(){
    switch(state){
        case IDLE:
            if(hal->get_temperature() > BREWMAXTOSTART){
                last_error =BREWTEMPERATURETOOHIGH;
            }
            else{
                state =BREWPREHEATING;
                last_error =NOERROR;
            }
            break;
        case BREWPREHEATING:
            last_error = BREWINPROGRESS;
            break;
        case BREWBREWING:
            last_error = BREWINPROGRESS;
            break;
        case KEEPWARMWARMING:
            last_error = KEEPWARMINPROGRESS;
            break;
        case KEEPWARMIDLE:
            last_error = KEEPWARMINPROGRESS;
            break;
        case ERR:
            break;
        default: break;

    }
}

void Controller::tick(){
    switch(state){
        case IDLE:
        	hal->set_blink_mode(0);
            return;
            break;
        case BREWPREHEATING:
        	hal->set_blink_mode(1);
            if(hal->get_temperature() > BREWSTARTTEMP){
                state = BREWBREWING;
                hal->start_heater();
                ticks_to_go = seconds_to_ticks(TIMETOBREW);
            }
            break;
        case BREWBREWING:
        	hal->set_blink_mode(1);
            ticks_to_go-=1;
            if(ticks_to_go == 0){
                last_error = NOERROR;
                hal->stop_heater();
                state = IDLE;
            }
            break;
        case KEEPWARMWARMING:
        	hal->set_blink_mode(1);
            ticks_to_go-=1;
            if(ticks_to_go == 0){
                last_error = NOERROR;
                hal->stop_heater();
                state = IDLE;
            }
            if(hal->get_temperature()> KWKEEPBELOW){
                hal->stop_heater();
                state = KEEPWARMIDLE;
            }

            break;
        case KEEPWARMIDLE:
        	hal->set_blink_mode(1);
            ticks_to_go-=1;
            if(ticks_to_go == 0){
                last_error = NOERROR;
                hal->stop_heater();
                state = IDLE;
            }
            if(hal->get_temperature()<KWKEEPABOVE){
                hal->start_heater();
                state = KEEPWARMWARMING;
            }
            break;
        case ERR:
        	hal->set_blink_mode(2);
            break;
        default: break;

    }

}


