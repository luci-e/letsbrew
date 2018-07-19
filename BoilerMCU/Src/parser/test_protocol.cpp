#include <iostream>
#include <string>
#include "stream_reader.hpp"
#include "lb_protocol.hpp"

using namespace std;
using namespace letsbrew;

int test_protocol( char ** argv, int argc ) {

	string good_request = "ID:42\nCMD:BREW\nUSR:21321\nTIME:12322\r\nTEST:BODY\nciccio:buccio";
	stream_reader sr( 512 );
	lb_handler lb_h;

	for ( auto c : good_request ) {
		auto ret = sr.push_char( c );

		switch ( ret ) {
			case STREAM_CODES::STREAM_OUT_OF_BOUND:
			{
				cout << "Stream out of bound " << endl;
				goto out;
			}

			case STREAM_CODES::STREAM_DONE:
			{
				cout << "Stream done " << endl;
				goto out;
			}

		}
	}out:

	letsbrew::lb_request lbr;
	letsbrew::lb_parse_request( lb_h, good_request, lbr );

	return 0;
}
