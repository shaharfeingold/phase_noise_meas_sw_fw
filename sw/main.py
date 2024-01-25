#!/opt/homebrew/bin/python3
import argparse
import sys
import os
import subprocess
import tkinter
import logic_config
import connection_module as connect
import defines
import data_mgm
import binascii
import user_interface
from error_handling import handle_fatal_error, handle_medium_error, handle_easy_error
import socket
import utilis_func
import corelate_module as corelate

"""
file        : maim.py
owner       : shaharf
description : main file to call each class and excute commands and functionality.
todo        : 1. need to connect the args from parser to the software logic itself
"""

def parse_arguments():
    parser = argparse.ArgumentParser(description="Phase Measurement Application")
    parser.add_argument("-m", "--mail", help="Email address to send reports to", type=str, required=True, dest='mail')
    # parser.add_argument("--help", help="Display help message", action="store_true")
    parser.add_argument("--debug_mode", help="Run in debug mode", action="store_true")
    parser.add_argument("--config_from_command_line", help="Set configuration from command line", dest='batchmode', action="store_true")
    parser.add_argument("-i", "--ip_addr", help="in case of config from command line specify the red pitaya ip address", dest='ip', type=str)
    parser.add_argument("-p", "--port", help="in case of config from comaand line specify the red pitaya port", dest='port', type=int)
    parser.add_argument("--data_only", help="Run in data-only mode", action="store_true") #just send mail and not showing graph.
    parser.add_argument("-v", "--verbos", help="verbose print", choices=["info,debug,error,fatal"], type=str) # todo shahar need to support this
    parser.add_argument("-n", "--num_of_channels", help="specify the number of channels", dest='channels', choices=[1,2], type=int, default=2)
    return parser.parse_args()


def setup_connection(logic_unit):
    max_attempts = 3
    for attempt in range(max_attempts):
        try:
            #ip_addr = input("Enter IP address of Red Pitaya: ")
            while True:
                ip_addr = user_interface.get_ip_addr()
                if validate_ip(ip_addr):
                    break
                print("Invalid IP format. Please try again.")

            # port = input("Enter Port Number of Red Pitaya: ")
            while True:
                port = user_interface.get_port()
                if validate_port(port):
                    break
                print("Invalid port number. Please enter a positive integer.")

            logic_unit.get_from_user_connection_info(ip_addr, port)
            logic_unit.connect_socket()
            print("Connection established successfully.")
            return True  # Return True if connection is successful
        except Exception as e:
            if attempt < max_attempts - 1:
                print("Connection failed, retrying...")
            else:
                handle_medium_error(f"Connection error: {e}")
                return False  # Return False if all attempts fail

def setup_logic_config(logic_unit, logic_cfg):
    user_interface.print_config_setup_msg()
    #freq = user_interface.get_freq_from_user()
    while True:
        freq = user_interface.get_freq_from_user()
        if validate_freq(freq):
            break
        print("Invalid frequency. Please enter a positive float in range 0 < f < ", defines.MAX_FREQUENCY)

    logic_cfg.get_freq_from_user(freq)
    logic_cfg.send_to_logic(logic_unit)

    # wait and get ack
    logic_cfg.ack_rcvr(logic_unit)

def wait_4_start_op(logic_cfg, logic_unit):
    user_interface.print_wait_for_start_op()
    start_indication = user_interface.get_from_user_start_op_indication()
    logic_cfg.send_to_logic_start_header(logic_unit, start_indication)
    
    # wait and get ack
    logic_cfg.start_ack_rcvr(logic_unit)

    # wait for recv from server operation finished
    user_interface.print_wait_to_finish_op()

def wait_4_data_and_unload(meas_data_ch0, meas_data_ch1, logic_unit, logic_cfg, NumOfChannels):
    # wait until header of data recv which indicat of finish operation.
    # todo shahar need to make sure that we are blocked up until the header recv. maybe add some sleep
    meas_data_ch0.decode_header(logic_unit)

    if (NumOfChannels == 2):
        meas_data_ch1.CopyHeaderFromOther(meas_data_ch0) # todo implelent

    # signal user the now unload data
    user_interface.print_data_unload()

    # unload data
    
    for i in range(meas_data_ch0.data_size_expected):
        meas_data_ch0.store_new_line_wrapper(logic_unit)

    # according to protocol in case of 2 channels the arrays are sent: array after array
    if (NumOfChannels == 2):
        for i in range(meas_data_ch1.data_size_expected):
            meas_data_ch1.store_new_line_wrapper(logic_unit)        
    
    msg_to_send = "got all msg and sending this ack" # if we want to send this need to make sure that its 1024 B wide.
    msg_to_send = msg_to_send.ljust(1023, '!')
    msg_to_send = msg_to_send.encode() + b'\x00'
    print("msg_to_send len", len(msg_to_send))
    print(msg_to_send)
    str_to_debug = "recived from logic #" + str(meas_data_ch0.data_size) + " is all expected data (" + str(meas_data_ch0.data_size_expected) + ") rcvr ? " + str(meas_data_ch0.check_all_data_recv())
    print(str_to_debug)
    logic_unit.send_data(msg_to_send)
    logic_cfg.got_finish = True # todo wrap around function of rcvr data

def data_anylsis(meas_data_ch0, meas_data_ch1, NumOfChannels, mail, corelate_mod):
    # todo shahar implement + add print to screen
    meas_data_ch0.compute_ftt()
    if (NumOfChannels == 2):
        meas_data_ch1.compute_ftt()

    # send raw data to user according to channel
    if (NumOfChannels == 1):
        utilis_func.save_and_send_array_in_a_file(meas_data_ch0, meas_data_ch0, mail) # bug in case of one channels we are sending both channels
    else:
        utilis_func.save_and_send_array_in_a_file(meas_data_ch0, meas_data_ch1, mail)

    corelate_mod.GetSignals(meas_data_ch0, meas_data_ch1, NumOfChannels)
    corelate_mod.CalcCorelate()
    # wait to close window

def end_op(logic_cfg, logic_unit):
    user_interface.print_end_of_op_how_to_proceed()
    # wait for user to choose: redo ? new config ? exit:
    end_of_op_user_choice = user_interface.print_end_of_op_options()

    # send to logic end of operation packet according to usr choice.
    logic_cfg.send_to_logic_end_header(logic_unit, end_of_op_user_choice)

    # wait and get ack
    logic_cfg.end_ack_rcvr(logic_unit)

    if (end_of_op_user_choice == '1'):
        logic_unit.close_connection()
        return False, True
    
    elif (end_of_op_user_choice == '2'):
        return False, False
    
    elif (end_of_op_user_choice == '3'):
        return True, False

    else:
       print("unvalid choice please try again")
        # todo shahar implement as while a loop

    # logic_unit.close_connection() # todo shahar consider to remove

def validate_ip(ip_addr):
    try:
        parts = ip_addr.split('.')
        len_check = len(parts) == 4
        range_check = all(0 <= int(part) <= 255 for part in parts)
        return len_check and range_check
    except ValueError:
        return False

def validate_port(port):
    try:
        return int(port) > 0
    except ValueError:
        return False

def validate_freq(freq):
    try:
        return 0 < float(freq) <= defines.MAX_FREQUENCY  
    except ValueError:
        return False


def Main(args):
    InitStage = True
    ConfigStage = True
    NeedToExit = False

    NumOfChannels = args.channels

    while (True):
        if (InitStage):
            # first set up all the modules:
            logic_unit = connect.LogicConnection()
            logic_cfg = logic_config.LogicConfig()
            meas_data_ch0 = data_mgm.Data()
            meas_data_ch1 = data_mgm.Data()
            corelate_mod = corelate.Corelate()
            InitStage = False

            # set up user interface
            user_interface.print_hello_msg()

            if not setup_connection(logic_unit):
                return
            
        if (ConfigStage):
            # start config red pitya
            setup_logic_config(logic_unit, logic_cfg)
            ConfigStage = False

        # wait for user to start operation
        wait_4_start_op(logic_cfg, logic_unit)

        # wait_for_data_and_unload
        wait_4_data_and_unload(meas_data_ch0, meas_data_ch1, logic_unit, logic_cfg, NumOfChannels)

        # data anylsis:
        data_anylsis(meas_data_ch0, meas_data_ch1, NumOfChannels, args.mail, corelate_mod)

        # prompt user how to continue
        ConfigStage, NeedToExit = end_op(logic_cfg, logic_unit)

        if (NeedToExit):
            break
    
    return 0

def validate_args(args):
    result = True
    # validate mail
    if ('@' in args.mail):
        mail_split = args.mail.split('@')
        # todo shahar contiune here and check for valid structre of a mail
    else:
        print("Invalid Mail Address please try again!")
        result = False

    #validate config from commad line
    if (args.batchmode == True):
        if (args.port == None or args.ip == None):
            print("If config_from_command_line is set to true need to specify ip address and port of the red pitaya")
            result = False 

    if (result == False):
        exit(1)

if __name__ == '__main__':
    args = parse_arguments()
    validate_args(args)
    if (defines.DEBUG == True): # todo shahar consider to remove this, we are not using main 
        Main(args)  # todo shahar rename to name 
    else:
        Main(args)
