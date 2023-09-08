#!/opt/homebrew/bin/python3

import sys
import os
import subprocess
import tkinter
import logic_config
import connection_module as connect
import defines
import data_mgm

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


if __name__ == '__main__':
    main()
