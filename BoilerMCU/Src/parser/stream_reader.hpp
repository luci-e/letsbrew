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

using namespace std;

enum STREAM_CODES{
	STREAM_OK,
	STREAM_OUT_OF_BOUND,
	STREAM_DONE
};

struct stream_reader{
	string message_buffer;
	uint32_t current_length;
	uint32_t max_length;

	stream_reader( size_t max_message_size ){
		message_buffer.resize( max_message_size );
		max_length = max_message_size;
		current_length = 0;
	}

	int push_char( char c ){
		if( current_length >= max_length ){
			return STREAM_OUT_OF_BOUND;
		}

		message_buffer[current_length++] = c;

		if( c == '\0' ){
			return STREAM_DONE;
		}

		return STREAM_OK;
	}

	void clean(){
		current_length = 0;
	}
};

#endif /* STREAM_READER_HPP_ */
