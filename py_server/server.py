import http.server
import socketserver
import pdb
import os 
import json
import serial
import time
import threading
import sys
import random
import re
from threading import Thread
SERIAL_PORT = ""
BAUDRATE = 115200
PORT = 8000
ROOT_DIRECTORY = './http'

serial_stream = None
serial_lock = threading.Lock()

qlock = threading.Lock()
queue = []
qsem = threading.Semaphore(0)
import paho.mqtt.client as mqtt

reply_reg = re.compile("(\d+) (\d+) ([^,]+), ([^,]+), (\d+) brews, (\d+) seconds in keepwarm, (\d+) seconds heater on, (\d+) Watts used, (-?\d+) temperature, (\d+) seconds to go.*", flags = re.ASCII)

def process( line  ):
    device_status = {}

    try:
        matches = reply_reg.match(line).groups()

        device_status['code'] = int(matches[1])
        device_status['error'] = matches[2]
        device_status['state'] = matches[3]
        device_status['brews'] = int(matches[4])
        device_status['seconds_in_keepwarm'] = int(matches[5])
        device_status['seconds_heater_on'] = int(matches[6])
        device_status['watts_used'] = int(matches[7])
        device_status['temperature'] =int( matches[8] )
        device_status['seconds_to_go'] = int(matches[9])
    except:
        return None

    return device_status

def serial_init_windows():
        global serial_stream
        try:
            SERIAL_PORT = '\\.\\COM' + sys.argv[1]
            serial_stream = serial.Serial(port=SERIAL_PORT, baudrate=BAUDRATE, parity=serial.PARITY_NONE,
                            stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=0)
        except Exception:
            print("Could not open serial_stream")
def serial_init_linux():
        global serial_stream
        try:
            SERIAL_PORT = sys.argv[1]
            serial_stream = serial.Serial(port=SERIAL_PORT, baudrate=BAUDRATE, parity=serial.PARITY_NONE,
                            stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=1)
        except Exception:
            print("Could not open serial_stream")

def serial_init():
    if(os.name == 'posix'):
        serial_init_linux()
    else:
        serial_init_windows()

def sync_serial_read():
    serial_lock.acquire()
    line = serial_stream.readline().decode(encoding='ascii') 
    serial_lock.release()
    return line

def sync_serial_write( data ):
    serial_lock.acquire()
    serial_stream.write(bytes(data,'ascii'))
    serial_stream.flush()
    serial_lock.release()

class brew_thread(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.done = False
    def stop(self):
        self.done = True
    def run(self):
        while(True):
            _, brew_request = build_request('B')
            print( sync_serial_read())
            sync_serial_write( brew_request )
            time.sleep(0.5)

def json_to_lb( json_cmd ):
    lb_cmd = "";

    lb_cmd += '"ID" : {}\n\
"USR" : {}\n\
"TIME" : {}\n\
"CMD" : "{}"\n'.format(json_cmd["ID"], json_cmd["USR"], json_cmd["TIME"], json_cmd["CMD"])

    CMD = json_cmd["CMD"];

    if( CMD == 'BREW'):
            lb_cmd += '"H2O_AMOUNT" : {}\n\
"H2O_TEMP" : {}\n\
"EXEC_TIME" : {}\n\0'.format(json_cmd["H2O_AMOUNT"], json_cmd["H2O_TEMP"], json_cmd["EXEC_TIME"])
    elif ( CMD == 'KEEPWARM'):
            lb_cmd += '"DURATION" : {}\n\0'.format(json_cmd["DURATION"])
    elif (CMD ==  'STATE'):
        pass
    
    return lb_cmd


class lb_request_handler( http.server.SimpleHTTPRequestHandler ):
        def __init__( self, request, client_address, server):
            super(lb_request_handler, self).__init__( request, client_address, server )

        def do_POST( self ):
            request_path = self.path
            
            request_headers = self.headers
            content_length = request_headers.get('Content-Length')
            
            str_cmd = self.rfile.read(int(content_length))

            json_cmd = json.loads(str_cmd)
            request = json_to_lb(json_cmd)

            print("Received request: ")
            print(request)

            if( serial_stream ):
                sync_serial_write( request )
                self.wfile.write(str.encode("HTTP/1.1 200 OK\r\nContent-type: text/html; charset=UTF-8\r\n\r\n" + "Request queued!" ))

class HTTP_Server_Thread(Thread):    
    def __init__(self):
        Thread.__init__(self)
        self.done = False

    def stop(self):
        self.done = True

    def run(self):
        print('HTTP Server started')
        with socketserver.TCPServer(("", PORT), lb_request_handler) as httpd:
            print("serving at port", PORT)
            httpd.serve_forever()

class Serial_reader_Thread(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.done = False

    def stop(self):
        self.done = True
    def run(self):
        print("serial reader started")
        while(not self.done):
            time.sleep(0.1)
            try:
                serial_lock.acquire()
                line = serial_stream.readline()
                if (len(line)>0):
                    print(line)
                qlock.acquire()
                queue.append(line)
                qlock.release()
                qsem.release()

            except:
                print("exception in serial thread")

            finally:
                serial_lock.release()

class Serial_poller_Thread(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.done = False

    def stop(self):
        self.done = True
    def run(self):
        print("serial poller started")
        while(not self.done):
            time.sleep(4)
            try:
                serial_lock.acquire()
                _, state_request = build_request('S')
                serial_stream.write(bytes(state_request,'ascii'))
                serial_stream.flush()
            except:
                print("exception in serial writer")

            finally:
                serial_lock.release()
 

class TBThread(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.done = False

    def stop(self):
        self.done = True
    def run(self):
        print("Thingsboard reporter started")
        while(not self.done):
            try:
                qsem.acquire()
                qlock.acquire()
                line = queue.pop().decode('ascii')
                try:
                    status= process(line)
                    if(status is not None):
                        print("Status: {}".format(json.dumps(status)))
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
                qlock.release()


def build_request( command ):
    cmd = ""

    cmd_id = random.randint(0, 32000)
    cmd += '"ID":{}\n'.format(cmd_id)
    cmd += '"USR":{}\n'.format(random.randint(0, 32000))
    cmd += '"TIME":{}\n'.format(random.randint(0, 32000))

    if(  'B' in command ):
        cmd += '"CMD":"BREW"\n'
        cmd += '"H2O_TEMP":3\n'
        cmd += '"H2O_AMOUNT":3\n'
    elif('K' in command):
        cmd += '"CMD":"KEEPWARM"\n'
        cmd += '"DURATION":3\n'
    elif('S' in command):
        cmd += '"CMD":"STATE"\n'
    elif( 'C' in command):
        cmd += '"CMD":"CANCEL"\n'
    else:
        return None, None

    return cmd_id, cmd+'\0'

tbthread = None
def main():
    global client
    os.chdir(ROOT_DIRECTORY)
    serial_init()
    srt = Serial_reader_Thread()
    srt.start()
    

    if( '--start_thingsboard' in sys.argv ):
        global tbthread


        tbthread = TBThread()


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
        spt = Serial_poller_Thread()
        spt.start()
        tbthread.start()

    if( '--start_server' in sys.argv ):
        srt = Serial_reader_Thread()
        srt.start()

        http_thread = HTTP_Server_Thread()
        http_thread.start()

    if( '--interactive' in sys.argv):

        print("Starting interactive shell...")

        while(True):
            line = sys.stdin.readline()
            _, cmd = build_request(line)

            if(cmd is None):
                if line == '\n':
                    _, request = build_request('B')
                    sync_serial_write( request)
            else:
                sync_serial_write( cmd )

if __name__ == '__main__':
    main()
