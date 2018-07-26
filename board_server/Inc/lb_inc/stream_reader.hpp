/*
 * stream_reader.hpp
 *
 *  Created on: 10 Jul 2018
 *      Author: lucie
 */

#ifndef STREAM_READER_HPP_
#define STREAM_READER_HPP_

#include <cstdint>
#include <cstdlib>
#include <string>
#include "globals.h"
using namespace std;

enum STREAM_CODE{
	STREAM_OK,
	STREAM_OUT_OF_BOUND,
	STREAM_DONE
};

class stream_reader{

	uint32_t current_length;
	uint32_t max_length;


public:
	bool message_complete = false;
	//string message_buffer;
	string message_buffer;

	stream_reader( size_t max_message_size ){
		message_buffer.resize( max_message_size );
		max_length = MESSAGEBUFSIZE;
		current_length = 0;
	}

	STREAM_CODE push_char( char c ){
		if( current_length >= max_length ){
			return STREAM_OUT_OF_BOUND;
		}

		message_buffer[current_length++] = c;

		if( c == '\0' ){
			message_complete = true;
			return STREAM_DONE;
		}

		return STREAM_OK;
	}

	void clean(){
		current_length = 0;
		message_complete = false;
	}
};

#endif /* STREAM_READER_HPP_ */
