import os
import time
import math
import random
from subprocess import Popen, PIPE
from queue import Queue, Empty
from threading import Thread
from datetime import datetime
from Py_to_pd import Py_to_pd
import serial

PD_PATH = "" #pi
PORT_SEND_TO_PD = 3000 #port to communicate message TO PD
PORT_RECEIVE_FROM_PD = 4000 #port to receive messages FROM PD
loop_status = {1:0, 2:0, 3:0, 4:0, 5:0, 6:0, 7:0, 8:0}

def read_pd_input(proc, q):
    """
    callback function
    Thread process to read PureData input
    Requires the queue, that stores the socket output
    """
    while True:
        pd_input = proc.readline()
        q.put(pd_input)
        time.sleep(1/100)

def process_pd_input(q):
    """
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
            time.sleep(1/10)
            pass

def handle_pd_msg(msg):
    """
    Handle a msg from puredata, which is split by | characters
    
    
    x = msg.split("|")
    del x[-1] #remove last element of the list (PD automatically adds \n)
    if x[0] == "counter":
        if not buttons.options_open:
            set_metronome(int(x[1]), int(x[2]))
    if x[0] == "status":
        handle_status(x[1], x[2:])
    """
    pass

def handle_status(action, payload):
    """
    Handle status messages from PD
    """
    payload = [int(i) for i in payload]
    action_handler = {
        "clear_rec"     : clear_record,
        "start_rec"     : record, 
        "start_overdub" : overdub, 
        "stop_rec"      : finish_record, #green
        "stop_overdub"  : finish_overdub, #green
        "wait_rec"      : wait_record, #orange
        "mute_rec"      : mute_record #blue/green
        }
    try:
        action_handler[action](payload)
    except:
        #not recognized action from PD
        print("unknown action received/crash: ", str(action))

def clear_record(payload):
    buttons.set_button_color(payload[0], COLORS[7]) #off
    loop_status[payload[0]] = 0
    if not buttons.options_open:
        lcd.lcd_display_string_pos("|", 1, (payload[0]-1)*2)
        display_loop_status(replace_loop=payload[0])
        

def wait_record(payload):
    buttons.set_button_color(payload[0], COLORS[6]) #orange
    lcd.lcd_write_char_pos(3, 1, (payload[0]-1)*2)

def record(payload):
    buttons.set_button_color(payload[0], COLORS[0]) #red
    lcd.lcd_write_char_pos(1, 1, (payload[0]-1)*2)

def overdub(payload):
    buttons.set_button_color(payload[0], COLORS[0]) #red
    lcd.lcd_write_char_pos(1, 1, (payload[0]-1)*2)

def finish_record(payload):
    #payload: 0 -> number of loops, 1 -> loop nr.
    buttons.set_button_color(payload[1], COLORS[1]) #green
    lcd.lcd_write_char_pos(0, 1, (payload[1]-1)*2)
    loop_status[payload[1]] = payload[0]
    display_loop_status(replace_loop=payload[1])

def display_loop_status(full_replace=False, replace_loop=0):
    """
    if full_replace:
        status_to_str = ''.join('{}{}'.format("|", val) for val in loop_status.values())
        lcd.lcd_display_string(status_to_str, 1)
    else:
        lcd.lcd_display_string_pos(str(loop_status[replace_loop]), 1, (replace_loop-1)*2+1)
    """
    pass

def finish_overdub(payload):
    buttons.set_button_color(payload[0], COLORS[1]) #green
    lcd.lcd_write_char_pos(0, 1, (payload[0]-1)*2)

def mute_record(payload):
    if payload[0] == 1: #mute
        lcd.lcd_write_char_pos(2, 1, (payload[1]-1)*2)
        buttons.set_button_color(payload[1], COLORS[2]) #blue
    if payload[0] == 0: #unmute
        lcd.lcd_write_char_pos(0, 1, (payload[1]-1)*2)
        buttons.set_button_color(payload[1], COLORS[1]) #green

def set_metronome(value, total_beats):
    block_size = math.floor(SCREEN_SIZE / max(total_beats, 4))
    if value == 0:
        lcd.lcd_display_string(block_size * BLOCK + (SCREEN_SIZE - block_size) * BLANK, 2)
    else:
        lcd.lcd_display_string_pos(block_size * BLOCK, 2, block_size*value)

def read_button_status():
    """
    Thread function that continuously reads the button_pad input
    """
    try:
        arduinoSerial = serial.Serial(port='/dev/ttyS0', baudrate=115200, timeout=.1)
    except:
        print("Can't open serial com")
    while True:
        msg = arduinoSerial.readline().decode('utf-8')
        if(msg):
            x_list = msg.split() #Receiving buttonId and value
            print("From serial id " , x_list[0] , " value ", x_list[1] )
            handleButtonPress(int(x_list[0]), int(x_list[1]))
        time.sleep(1/2000)
        
def handleButtonPress(buttonId, value):
    if(value == 1):         #Button PRESSED
        send_msg.press_button(buttonId+1)
    elif(value == 2):        #Button HOLD
        send_msg.clear_loop(buttonId+1)
    

def handle_button_press(column, row):
    """
    This function handles the button press, not the release
    There's a lot of sintax regarding the
    """
    #Send button press
    if buttons.options_open:
        # Handle options
        button_num = 1 + 4 * column + row
        update_option_lcd()
        if not button_num in [11, 12, 14, 15, 16]:
            # Unknown options button
            lcd.lcd_display_string("Use but 14/15/16", 1)
        if button_num == 12:
            for drumpad_button in DRUMPAD_BUTTONS:
                #Set buttons to white color
                buttons.set_button_color(drumpad_button, random.choice(COLORS))
        if button_num == 11:
            #reset screen
            lcd.lcd_clear()
            lcd.lcd_display_string("screen reset",1)
            update_option_lcd()
        if button_num == 14:
            # next option
            buttons.option_number = 0 if buttons.option_number == (len(buttons.options)-1) else buttons.option_number + 1
            update_option_lcd()
        if button_num == 15:
            # apply option
            if buttons.option_number == 0:
                # select_kit
                buttons.option_values[buttons.option_number] = 1 if buttons.option_values[buttons.option_number] == buttons.total_drumkits else buttons.option_values[buttons.option_number] + 1
                send_msg.select_kit(buttons.option_values[buttons.option_number])
            if buttons.option_number == 1:
                # toggle_input
                buttons.option_values[buttons.option_number] = 1 - buttons.option_values[buttons.option_number]
                send_msg.toggle_audio_input(buttons.option_values[buttons.option_number])
            if buttons.option_number == 2:
                # clear_all
                send_msg.clear_all()
                buttons.option_values[buttons.option_number] = 1
                buttons.active_loops = {1:False, 2:False, 3:False, 4:False, 5:False, 6:False, 7:False, 8:False}
                buttons.init_loop = True
                loop_status = {1:0, 2:0, 3:0, 4:0, 5:0, 6:0, 7:0, 8:0}
            update_option_lcd()
        if button_num == 16:
            # quit options
            toggle_options()
    else:
        buttons.button_press_time[column][row] = datetime.now()
        button_num = 1 + 4 * column + row
        if button_num > 8 or not buttons.active_loops[button_num]:
            # Press the button if drumkit or no active loop
            # For active loops: wait for release timer (overdub or (un)mute)
            send_msg.press_button(button_num)
        if not buttons.init_loop and button_num <= 8 and (buttons.button_press_time[column][row] - buttons.button_prev_press_time[column][row]).total_seconds() < 0.3:
            # clear when: not initial loop, pressed a loop button, and pressed twice within 0.3 sec.
            send_msg.clear_loop(button_num)
            buttons.active_loops[button_num] = False
        # Set previous button press time
        buttons.button_prev_press_time[column][row] = buttons.button_press_time[column][row]


def handle_button_release(column, row):
    """
    Function that handles when a button is released (up)
    - Checks if the button is actually pressed
    - Checks if the button is an active loop, and monitors the time of the
    button press
    - Checks if the pressed button = 13, if long press: open options
    - Lastly: reset the button timer and press time
    """
    #Send key release
    button_num = 1 + 4 * column + row
    press_time = datetime.now()
    if button_num < 9:
        # loop button
        if buttons.active_loops[button_num]:
            #active loop: release longer than 1 second: overdub loop, else press_button
            if (press_time - buttons.button_press_time[column][row]).total_seconds() >= 0.5:
                #send overdub loop if row 1 or 2
                send_msg.overdub(button_num)
            else:
                #press button only if not held down > 0.5 seconds
                send_msg.press_button(button_num)
        if not buttons.init_loop:
            #turn into an active loop if this is not the first press of the initial loop
            #in that case; you don't want to wait until release for mute.
            buttons.active_loops[button_num] = True
        else:
            #Set the initial loop to false; initial loop is now recorded
            buttons.init_loop = False
    if (press_time - buttons.button_press_time[column][row]).seconds >= 0.5 and button_num == 13:
        #open the option menu
        toggle_options()


# Set up communication to PureData
send_msg = Py_to_pd(PD_PATH, PORT_SEND_TO_PD)

# Get the number of drum samples
drum_files = os.listdir('../puredata/drum_sounds')
kit = ['crash', 'hh', 'kick', 'snare']
num_drumkits = max([int(x.split('_')[1]) for x in drum_files if x.split('_')[0] in kit])


# start the socket
args = ["pdreceive", str(PORT_RECEIVE_FROM_PD)]
process_socket_PD = Popen(args, stdout=PIPE) #second process to read PD
proc_q = Queue() #queue for messages from PD
# Seperate threads for reading/handling the output from PD
read_pd_thread = Thread(target = read_pd_input, args = (process_socket_PD.stdout, proc_q))
process_pd_thread = Thread(target = process_pd_input, args = (proc_q, ))
# Thread to handle the button press/release
handle_button_press_release = Thread(target = read_button_status, args = ())

read_pd_thread.start()
process_pd_thread.start()
handle_button_press_release.start()


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

"""
display_loop_status(full_replace=True)
lcd.lcd_display_string("Ready to play!", 2)
"""

while True:
    # Run button loop
    #buttons.scan()
    time.sleep(1/1000)
