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

def process( splitted_list):
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
            brew_request = '1 BREW 10\0'
            print( sync_serial_read())
            sync_serial_write( brew_request )
            time.sleep(0.5)

def json_to_lb( json ):
    lb_cmd = "";

    lb_cmd += "ID : " + json_cmd["ID"] + "\n";
    lb_cmd += "USR : " + json_cmd["USR"] + "\n";
    lb_cmd += "TIME : " + json_cmd["TIME"] + "\n";
    lb_cmd += "CMD : " + json_cmd["CMD"] + "\n";
    lb_cmd += "\r\n"

    CMD = json["CMD"];

    if( CMD == 'BREW'):
            lb_cmd += "EXEC_TIME : " + json_cmd["EXEC_TIME"] + "\n";
            lb_cmd += "H2O_TEMP : " + json_cmd["H2O_TEMP"] + "\n";
            lb_cmd += "H2O_AMOUNT : " + json_cmd["H2O_AMOUNT"] + "\n";
    elif ( CMD == 'KEEPWARM'):
            lb_cmd += "DURATION : " + json_cmd["DURATION"] + "\n";
    elif (CMD ==  'STATE'):
            lb_cmd += "\n";
    
    return lb_cmd;

class lb_request_handler( http.server.SimpleHTTPRequestHandler ):

    def __init__( self, request, client_address, server):
        super(lb_request_handler, self).__init__( request, client_address, server )

    def do_POST( self ):
        request_path = self.path
        
        request_headers = self.headers
        content_length = request_headers.get('Content-Length')
        
        str_cmd = self.rfile.read(int(content_length))
        print(str_cmd)

        json_cmd = json.loads(str_cmd)
        print(json_cmd)

        if( serial_stream ):
            request = json_to_lb( json_cmd )
            sync_serial_write( request )
            line = sync_serial_read()
            self.wfile.write(str.encode("HTTP/1.1 200 OK\r\nContent-type: text/html; charset=UTF-8\r\n\r\n" + line))

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
                qsem.release()

            except:
                print("exception in serial thread")

            finally:
                qlock.release()
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

                state_request = '1 STATE 1\0'
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
                #serial_lock.acquire()

                #state_request = '1 STATE 1\0'
                #serial_stream.write(bytes(state_request,'ascii'))
                #serial_stream.flush()
                #line = serial_stream.readline()
                #print(line)
                #serial_lock.release()
                #continue
                qsem.acquire()
                qlock.acquire()
                line = queue.pop()
                splitted = line.decode('ascii').split(',')
                if len(splitted)==8:
                    try:
                        status= process(splitted)
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
                qlock.release()


def build_request( command ):
    cmd = ""

    cmd += '"ID":{}\n'.format(random.randint(0, 32000))
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
    else:
        return None

    return cmd+'\0'

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

        http_thread = HTTP_Server_Thread()
        http_thread.start()

    if( '--interactive' in sys.argv):

        print("Starting interactive shell...")

        while(True):
            line = sys.stdin.readline()
            cmd = build_request(line)

            if(cmd is None):
                if line == '\n':
                    line = '0 BREW 0\n'
                    sync_serial_write( line + '\0')
            else:
                sync_serial_write( cmd )

if __name__ == '__main__':
    main()
