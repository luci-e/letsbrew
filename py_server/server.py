import http.server
import socketserver
import pdb
import os 
import json
import serial
import time
import threading
import sys
from threading import Thread
SERIAL_PORT = ""
BAUDRATE = 115200
PORT = 8000
ROOT_DIRECTORY = './http'

serial_stream = None
serial_lock = threading.Lock()

import paho.mqtt.client as mqtt

THINGSBOARD_HOST = 'localhost'
THINGSBOARD_HOST = '144.76.3.130'
ACCESS_TOKEN = 'BMqsFvyVst8g0v11SjLO'

# Data capture and upload interval in seconds. Less interval will eventually hang the DHT22.
INTERVAL=2

sensor_data = {'temperature': 0, 'humidity': 0}

next_reading = time.time() 

client = mqtt.Client()

# Set access token
client.username_pw_set(ACCESS_TOKEN)

# Connect to ThingsBoard using default MQTT port and 60 seconds keepalive interval
client.connect(THINGSBOARD_HOST, 1883, 60)

client.loop_start()


def serial_init_windows():
        global serial_stream
        try:
            SERIAL_PORT = '\\.\\COM' + sys.argv[1]
            serial_stream = serial.Serial(port=SERIAL_PORT, baudrate=BAUDRATE, parity=serial.PARITY_NONE,
                            stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=5000)
        except Exception:
            print("Could not open serial_stream")
def serial_init_linux():
        global serial_stream
        try:
            SERIAL_PORT = sys.argv[1]
            serial_stream = serial.Serial(port=SERIAL_PORT, baudrate=BAUDRATE, parity=serial.PARITY_NONE,
                            stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=5)
        except Exception:
            print("Could not open serial_stream")

def serial_init():
    if(os.name == 'posix'):
        serial_init_linux()
    else:
        serial_init_windows()

def sync_serial_read():
    serial_lock.acquire()
    print( serial_stream.readline().decode(encoding='ascii') )
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

        if( serial_stream ):
            sync_serial_write( "hello there\0")
            sync_serial_read()

class TBThread(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.done = False

    def stop(self):
        self.done = True
    def run(self):
        while(not self.done):
            time.sleep(1)
            try:
                serial_lock.acquire()

                state_request = \
                    'ID : 1\n\
                    USR : 1\n\
                    TIME : 1\n\
                    CMD : STATE\n\
                    \r\n\
                    EXEC_TIME : 1\n\
                    H2O_TEMP : 1\n\
                    H2O_AMOUNT : 1\n\0'
                serial_stream.write(bytes(state_request,'ascii'))
                serial_stream.flush()
                line = serial_stream.readline()
                print(line)
                splitted = line.decode('ascii').split(',')
                if len(splitted)==8:
                    try:
                        status=self.process(splitted)
                        print(status)
                        client.publish('v1/devices/me/telemetry', json.dumps(status), 1)
                    except:
                        print('error in TBThread.process')
                        print ( sys.exc_info()[0])
                        raise

            except:
                print("exception in TBThread")
                print ( sys.exc_info())
                raise

            finally:
                serial_lock.release()
    def process(self,splitted_list):
        err = splitted_list[0].partition(' ')
        device_status = {}
        device_status['code'] = int(err[0])
        device_status['error'] = err[2]
        device_status['state'] = splitted_list[1]
        device_status['brews'] = splitted_list[2].lstrip(' ').partition(' ')[0]
        device_status['seconds_in_keepwarm'] = splitted_list[3].lstrip(' ').partition(' ')[0]
        device_status['seconds_heater_on'] = splitted_list[4].lstrip(' ').partition(' ')[0]
        device_status['watts_used'] = splitted_list[5].lstrip(' ').partition(' ')[0]
        device_status['temperature'] = splitted_list[6].lstrip(' ').partition(' ')[0]
        device_status['seconds_to_go'] = splitted_list[7].lstrip(' ').partition(' ')[0]
        return device_status


tbthread = None
def main():
    global tbthread
    os.chdir(ROOT_DIRECTORY)
    serial_init()
    tbthread = TBThread()
    tbthread.start()
    with socketserver.TCPServer(("", PORT), lb_request_handler) as httpd:
        print("serving at port", PORT)
        httpd.serve_forever()

if __name__ == '__main__':
    main()
