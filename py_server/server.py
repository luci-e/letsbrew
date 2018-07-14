import http.server
import socketserver
import pdb
import os 
import json
import serial
import time
import threading
import sys

SERIAL_PORT = '\\.\\COM4' + sys.argv[1]
BAUDRATE = 115200
PORT = 80
ROOT_DIRECTORY = './httpd'

serial_stream = serial.Serial(port=SERIAL_PORT, baudrate=BAUDRATE, parity=serial.PARITY_NONE,
                            stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=0)
serial_lock = threading.Lock()


def sync_serial_read():
    serial_lock.acquire()
    print( serial_stream.read(15).decode(encoding='ascii') )
    serial_lock.release()

def sync_serial_write( data ):
    serial_lock.acquire()
    serial_stream.write( str.encode(data) )
    serial_lock.release()

class lb_request_handler( http.server.SimpleHTTPRequestHandler ):

    def __init__( self, request, client_address, server):
        super(lb_request_handler, self).__init__( request, client_address, server )

    def do_POST( self ):
        request_path = self.path
        
        print("\n----- Request Start ----->\n")
        print(request_path)
        
        request_headers = self.headers
        content_length = request_headers.get('Content-Length')
        
        str_cmd = self.rfile.read(int(content_length))
        print(str_cmd)
        print("<----- Request End -----\n")

        json_cmd = json.loads(str_cmd)

        test_data = {'hello_there': 'general_kenobi' }

        self.wfile.write(str.encode(
            "HTTP/1.1 200 OK\r\nContent-type: text/html; charset=UTF-8\r\n\r\n" + json.dumps(test_data))
        )

        sync_serial_write( "hello there")
        sync_serial_read()


def main():
    os.chdir(ROOT_DIRECTORY)
    with socketserver.TCPServer(("", PORT), lb_request_handler) as httpd:
        print("serving at port", PORT)
        httpd.serve_forever()

if __name__ == '__main__':
    main()
