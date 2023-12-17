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

"""
file        : maim.py
owner       : shaharf
description : main file to call each class and excute commands and functionality.
todo        : 1. wrap some function to be able to jump at the end of operation
"""


def main():
    # first set up all the modules:
    logic_unit = connect.LogicConnection()
    logic_cfg = logic_config.LogicConfig()
    meas_data = data_mgm.Data()

    # get from user inputs
    ip_addr = input("Enter IP address of Red Pitaya\n")
    port = input("Enter Port Num of Red Pitaya\n")
    # connect to socket
    logic_unit.get_from_user_connection_info(ip_addr, port)
    logic_unit.connect_socket()
    # get freq to config from user
    freq = input("Please Enter Freq for operation\n")
    logic_cfg.get_freq_from_user(freq)
    # send config msg
    msg_to_send = str(defines.CONFIG) + str(logic_cfg.send_to_logic())
    print(msg_to_send + '\n')
    logic_unit.send_data(msg_to_send.encode())
    # get the echoed msg from the server and set flags
    buffer = logic_unit.rcvr_data().decode()
    print("rcvr data from server : " + buffer + '\n')
    if buffer == msg_to_send:
        logic_cfg.ack_rcvr()
        print(logic_cfg)
        print(logic_unit)
        print("test succeed")
    logic_unit.close_connection()
    return 0

def DebugTest():
    # first set up all the modules:
    logic_unit = connect.LogicConnection()
    logic_cfg = logic_config.LogicConfig()
    meas_data = data_mgm.Data()

    # set up user interface
    user_interface.print_hello_msg()

    # get from user inputs
    ip_addr = user_interface.get_ip_addr()
    port = user_interface.get_port()
    # connect to socket
    logic_unit.get_from_user_connection_info(ip_addr, port)
    logic_unit.connect_socket()

    # start config red pitya
    user_interface.print_config_setup_msg()
    freq = user_interface.get_freq_from_user()
    logic_cfg.get_freq_from_user(freq)
    logic_cfg.send_to_logic(logic_unit)

    # wait and get ack
    logic_cfg.ack_rcvr(logic_unit)

    # wait for user to start operation
    user_interface.print_wait_for_start_op()
    start_indication = user_interface.get_from_user_start_op_indication()
    #todo shahar need to send start indication pakcet
    logic_cfg.send_to_logic_start_header(logic_unit, start_indication)
    
    # wait and get ack
    logic_cfg.start_ack_rcvr(logic_unit)

    # wait for recv from server operation finished
    user_interface.print_wait_to_finish_op()

    # wait until header of data recv which indicat of finish operation.
    # todo shahar need to make sure that we are blocked up until the header recv. maybe add some sleep
    meas_data.decode_header(logic_unit)

    # signal user the now unload data
    user_interface.print_data_unload()

    # unload data
    
    for i in range(meas_data.data_size_expected):
        meas_data.store_new_line_wrapper(logic_unit)

    # todo shahar need to convert number to the right format.

    msg_to_send = "got all msg and sending this ack" # if we want to send this need to make sure that its 1024 B wide.
    print(msg_to_send)
    str_to_debug = "recived from logic #" + str(meas_data.data_size) + " is all expected data (" + str(meas_data.data_size_expected) + ") rcvr ? " + str(meas_data.check_all_data_recv())
    print(str_to_debug)
    logic_unit.send_data(msg_to_send.encode())
    logic_cfg.got_finish = True # todo wrap around function of rcvr data

    # data anylsis:
    # todo shahar implement + add print to screen
    meas_data.compute_ftt()
    # wait to close window

    # prompt user how to continue
    user_interface.print_end_of_op_how_to_proceed()
    # wait for user to choose: redo ? new config ? exit:
    end_of_op_user_choice = user_interface.print_end_of_op_options()

    # send to logic end of operation packet according to usr choice.
    logic_cfg.send_to_logic_end_header(logic_unit, end_of_op_user_choice)

    if (end_of_op_user_choice == '1'):
        logic_unit.close_connection()
    elif (end_of_op_user_choice == '2'):
        # todo shahar implement redo + support in fw
        # todo shahar upuntil we implelent redo we will close connection
        logic_unit.close_connection()
    
    elif (end_of_op_user_choice == '3'):
        # todo shahar implement redo + support in fw
        # todo shahar upuntil we implelent redo we will close connection
        logic_unit.close_connection()
    else:
        print("unvalid choice please try again")
        #todo shahar implement as while a loop

    # logic_unit.close_connection()
    return 0

if __name__ == '__main__':
    if (defines.DEBUG == True):
        DebugTest()
    else:
        main()
