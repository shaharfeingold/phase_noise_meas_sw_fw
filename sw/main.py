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
file : maim.py
owner : shaharf
description : main file to call each class and excute commands and functionality.
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

    # wait for recv from server operation finished
    user_interface.print_wait_to_finish_op()

    # wait until header of data recv which indication of finish operation.
    # todo shahar need to make sure that we are blocked up until the header recv. maybe add some sleep
    meas_data.decode_header(logic_unit)

    # signal user the now unload data
    user_interface.print_data_unload()

    # unload data
    
    for i in range(meas_data.data_size_expected):
        meas_data.store_new_line_wrapper(logic_unit)

    # buffer = logic_unit.rcvr_data().decode()
    # print("rcvr data from server 1 : " + buffer + '\n')
    # meas_data.decode_header(buffer)
    # buffer = logic_unit.client_socket.recv(4)
    # hex_string = binascii.hexlify(buffer).decode()
    # print("rcvr data from server 2 : " + hex_string + '\n')
    # meas_data.store_new_line(hex_string)
    # buffer = logic_unit.client_socket.recv(4)
    # hex_string = binascii.hexlify(buffer).decode()
    # print("rcvr data from server 3 : " + hex_string + '\n')
    # meas_data.store_new_line(hex_string)
    # buffer = logic_unit.client_socket.recv(4)
    # hex_string = binascii.hexlify(buffer).decode()
    # print("rcvr data from server 4 : " + hex_string + '\n')
    # meas_data.store_new_line(hex_string)

    msg_to_send = "got all threre msg and sending this ack" # if we want to send this need to make sure that its 1024 B wide.
    print(msg_to_send + '\n')
    logic_unit.send_data(msg_to_send.encode())
    return 0

if __name__ == '__main__':
    if (defines.DEBUG == True):
        DebugTest()
    else:
        main()
