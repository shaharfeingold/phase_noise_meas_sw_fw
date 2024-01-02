#!/opt/homebrew/bin/python3

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
"""
file        : maim.py
owner       : shaharf
description : main file to call each class and excute commands and functionality.
todo        : 1. wrap some function to be able to jump at the end of operation
"""


# todo saray need to create a infite loop to enter the right format:
# ip: x.x.x.x when each x is <= 255
# port: any postive number not float
# freq: float > 0 (right now the freq should be lower then 1.1) # todo saray replace 1.1 with defines
# with each retry supply the correct promt. 

def setup_connection(logic_unit):
    max_attempts = 3
    for attempt in range(max_attempts):
        try:
            ip_addr = input("Enter IP address of Red Pitaya: ")
            port = input("Enter Port Number of Red Pitaya: ")
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
    freq = user_interface.get_freq_from_user()
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

def wait_4_data_and_unload(meas_data, logic_unit, logic_cfg):
    # wait until header of data recv which indicat of finish operation.
    # todo shahar need to make sure that we are blocked up until the header recv. maybe add some sleep
    meas_data.decode_header(logic_unit)

    # signal user the now unload data
    user_interface.print_data_unload()

    # unload data
    
    for i in range(meas_data.data_size_expected):
        meas_data.store_new_line_wrapper(logic_unit)
    
    msg_to_send = "got all msg and sending this ack" # if we want to send this need to make sure that its 1024 B wide.
    msg_to_send = msg_to_send.ljust(1023, '!')
    msg_to_send = msg_to_send.encode() + b'\x00'
    print("msg_to_send len", len(msg_to_send))
    print(msg_to_send)
    str_to_debug = "recived from logic #" + str(meas_data.data_size) + " is all expected data (" + str(meas_data.data_size_expected) + ") rcvr ? " + str(meas_data.check_all_data_recv())
    print(str_to_debug)
    logic_unit.send_data(msg_to_send)
    logic_cfg.got_finish = True # todo wrap around function of rcvr data

def data_anylsis(meas_data):
    # todo shahar implement + add print to screen
    meas_data.compute_ftt()
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
    
# def main():
#     # first set up all the modules:
#     logic_unit = connect.LogicConnection()
#     logic_cfg = logic_config.LogicConfig()
#     meas_data = data_mgm.Data()

#     if not setup_connection(logic_unit):
#         return

#     freq = input("Please Enter Freq for operation\n")
#     logic_cfg.get_freq_from_user(freq)

#     # send config msg
#     msg_to_send = str(defines.CONFIG) + str(logic_cfg.send_to_logic())
#     print(msg_to_send + '\n')
#     logic_unit.send_data(msg_to_send.encode())

#     # get the echoed msg from the server and set flags
#     buffer = logic_unit.rcvr_data().decode()
#     print("rcvr data from server : " + buffer + '\n')
#     if buffer == msg_to_send:
#         logic_cfg.ack_rcvr()
#         print(logic_cfg)
#         print(logic_unit)
#         print("test succeed")
    
#     logic_unit.close_connection()
#     return 0


def DebugTest():
    InitStage = True
    ConfigStage = True
    NeedToExit = False

    while (True):
        if (InitStage):
            # first set up all the modules:
            logic_unit = connect.LogicConnection()
            logic_cfg = logic_config.LogicConfig()
            meas_data = data_mgm.Data()
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
        wait_4_data_and_unload(meas_data, logic_unit, logic_cfg)

        # data anylsis:
        data_anylsis(meas_data)

        # prompt user how to continue
        ConfigStage, NeedToExit = end_op(logic_cfg, logic_unit)

        if (NeedToExit):
            break
    
    return 0

if __name__ == '__main__':
    if (defines.DEBUG == True): # todo shahar consider to remove this, we are not using main 
        DebugTest()  # todo shahar rename to name 
