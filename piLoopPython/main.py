import os
import time
import math
import random
from subprocess import Popen, PIPE
from queue import Queue, Empty
from threading import Thread
from datetime import datetime
import serial
import os
import enum


class TrackState(enum.Enum):
    CLEAR_REC = 0
    START_REC = 1
    STOP_REC = 2
    START_OVERDUB = 3
    STOP_OVERDUB = 4
    WAIT_REC = 5
    MUTE_REC = 6

class MsgId(enum.Enum):
    STATUS = 0
    COUNTER = 1
    
class Button(enum.Enum):
    RELEASED = 0
    PRESSED = 1
    HOLD = 2



class Py_to_pd:
    def __init__(self, pd_path, send_port):
        self.path = pd_path
        self.port = send_port

    def send2Pd(self, channel, ID='', value = ''):
        """
        Send messages from Python to PD.
        channel: Link each message to a channel to be received in PD
            0: DSP status (0: off, 1: on)
            1: select_kit (int value)
        message: should be a string msg to be send as variable to PD
        """
        os.system("echo '" + str(channel) + " " + str(ID) + " " + str(value) + ";' | "+
            self.path + "pdsend " + str(self.port))


def read_pd_input(proc, q):
    """ read_pd_input
    callback function
    Thread process to read PureData input
    Requires the queue, that stores the socket output
    """
    while True:
        pd_input = proc.readline()
        q.put(pd_input)


def process_pd_input(q):
    """ process_pd_input
    callback function
    Constantly read the queue for new messages from PD
    """
    while True:
        try:
            #reads the queue without blocking
            pd_input = q.get().decode()
            if pd_input:
                handle_pd_msg(pd_input)
        except Empty:
            #time.sleep(1/10)
            pass


def handle_pd_msg(msg):
    """
    Handle a msg from puredata, which is split by | characters. 
    Send data to Arduino
    """
    x = msg.split("|")
    del x[-1] #remove last element of the list (PD automatically adds \n)
    for b in x:
        arduinoSerial.write(b.encode())


def set_metronome(value, total_beats):
    block_size = math.floor(SCREEN_SIZE / max(total_beats, 4))
    if value == 0:
        lcd.lcd_display_string(block_size * BLOCK + (SCREEN_SIZE - block_size) * BLANK, 2)
    else:
        lcd.lcd_display_string_pos(block_size * BLOCK, 2, block_size*value)


def readSerial():
    """
    Thread function reads the button_pad input
    """
    buff = [0,0,0]
    i = 0
    if(arduinoSerial.inWaiting() >= 3):
        while(arduinoSerial.inWaiting()):
            byte = arduinoSerial.read()
            buff[i] = int.from_bytes(byte, byteorder='little', signed=False)
            i = i +1
            if(i==3):
                i=0
                ch = buff[0]
                btnId = buff[1] + 1
                value = buff[2]
                send_msg.send2Pd(ch,btnId,value)
                
        
    # msg = arduinoSerial.readline().decode('utf-8')
    # if(msg):
        # x_list = msg.split() #Receiving buttonId and channel
        # print(x_list)
        # ch = int(x_list[0])
        # btnId = int(x_list[1]) + 1
        # send_msg.send2Pd(ch,btnId)
        

 

# Config
PD_PATH = "" #pi
PORT_SEND_TO_PD = 3000          #port to communicate message TO PD
PORT_RECEIVE_FROM_PD = 4000     #port to receive messages FROM PD
SERIAL_PORT = '/dev/ttyS0'      #Serial port to communicate with Arduino
SERIAL_BAUD_RATE = 115200       #serial speed

# Set up communication to PureData
send_msg = Py_to_pd(PD_PATH, PORT_SEND_TO_PD)


# start the socket
args = ["pdreceive", str(PORT_RECEIVE_FROM_PD)]
process_socket_PD = Popen(args, stdout=PIPE) #second process to read PD
proc_q = Queue() #queue for messages from PD

# Arduino serial communication
try:
    arduinoSerial = serial.Serial(port=SERIAL_PORT, baudrate=SERIAL_BAUD_RATE, timeout=.1)
except:
    print("Can't open serial com")

# Seperate threads for reading/handling the output from PD
read_pd_thread = Thread(target = read_pd_input, args = (process_socket_PD.stdout, proc_q))
process_pd_thread = Thread(target = process_pd_input, args = (proc_q, ))

#Start thread
read_pd_thread.start()
process_pd_thread.start()


# Setup audio card
os.chdir('script')
os.system('./Playback_to_Lineout.sh')
os.system('./Record_from_Headset.sh')
os.chdir('../')

# start PD
os.chdir('../puredata')
#os.system(PD_PATH + 'pd -nogui main.pd &')
os.system(PD_PATH + 'pd main.pd &')
time.sleep(2)


# Run button loop
while True:
    readSerial()
