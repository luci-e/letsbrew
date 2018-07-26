#pragma once

#ifndef LB_PROTOCOL_INCLUDE
#define LB_PROTOCOL_INCLUDE

#include <map>
#include <string>
#include <queue>
#include <list>
#include <cstdint>
#include <sstream>
#include <regex>

namespace letsbrew{

using namespace std;

#define MAX_MESSAGE_SIZE 512

	enum LB_CMD{
		BREW,
		CANCEL,
		STATE,
		KEEPWARM
	};

	enum PARSE_CODE{
		PARSE_OK,
		PARSE_BAD_HEADER,
		PARSE_BAD_BODY,
		PARSE_BAD_REQUEST
	};

	struct lb_request_header{
		uint32_t id;
		uint32_t usr;
		LB_CMD CMD;
		uint32_t time;
	};

	struct lb_request{
		lb_request_header request_header;
		map<string, string> request_params;
	};


	struct lb_response_header{
		uint32_t id;
		uint32_t server;
		LB_CMD CMD;
		uint32_t time;
	};

	struct lb_response {
		lb_response_header response_header;
		map<string, string> response_params;
	};

	/**
	 * Takes a well formed request_header and puts the result in the result struct
	 * @param lb_request_header
	 * @param result
	 * @return
	 */
	int lb_parse_header( const string &lb_header, lb_request_header &result ) ;

	/**
	* Takes a well formed request_header and puts the result in the result struct
	* @param lb_body
	* @param result
	* @return
	*/
	int lb_parse_body( const string &lb_body, lb_request &result );

	/**
	 * Takes a well formed request and puts the result in an lb_request struct
	 * @param lb_request
	 * @param result
	 * @return
	 */
	int lb_parse_request( const string &lb_request_string, lb_request &result );
}
#endif // LB_PROTOCOL_INCLUDE
