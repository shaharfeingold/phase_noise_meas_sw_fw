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
from error_handling import handle_fatal_error, handle_medium_error, handle_easy_error
import socket
import time
"""
file : maim.py
owner : shaharf
version : 1
description : main file to call each class and excute commands and functionality.
comments: # todo shahar later will switch to GUI
"""

class UI:
    def __init__(self, IsBatchMode = False, addr = '', port = 0, freq = 0, default_end_op = "1"):
        self.batchmode = IsBatchMode
        self.addr = addr
        self.port = port
        self.freq = freq
        self.default_end_op = default_end_op

    def print_hello_msg(self):
        if (self.batchmode):
            return
        print("########################################")
        print("Wellcome to phase measurment application")
        print("########################################")
        print("step 1: setting up connection to red pitya.")
        print("please retrive the the ip address and the port your red pitya is listening")

    def get_ip_addr(self):
        if (self.batchmode):
            return self.addr
        ip_addr = input("Enter IP address of Red Pitaya: ")
        return ip_addr

    def get_port(self):
        if (self.batchmode):
            return self.port
        port = input("Enter Port Num of Red Pitaya: ")
        return port

    def print_config_setup_msg(self):
        if (self.batchmode):
            return
        print("step 2: please retrive the freq of the oscillator you want to check")

    def get_freq_from_user(self):
        if (self.batchmode):
            return self.freq
        freq = input("Enter Freq of the external oscillator: ")
        return freq

    def print_wait_for_start_op(self):
        # todo do we want to include this in batcmode
        print("set up complete")
        print("please connect the oscillator to the red pitya and when reday follow the instruction on the screen")

    def get_from_user_start_op_indication(self):
        # in version 2 of this application full batchmode
        # result = input("please hit 1 when ready\n")
        time.sleep(1)
        result = "1"
        return result

    def print_wait_to_finish_op(self):
        print("please wait for finish operation")

    def print_data_unload(self):
        print("red pitya finished its operation, start unloading data")

    def print_end_of_op_how_to_proceed(self):
        if (self.batchmode):
            return
        print("you close the window of result, please select how to proceed")

    def print_end_of_op_options(self):
        if (self.batchmode):
            return self.default_end_op
        print("*************************************************************************")
        print("if you want to exit please type 1")
        print("if you want to measure again with the same configuration please select 2")
        print("if you want to set new configuration please select 3")
        print("*************************************************************************")
        result = input("please enter your choice\n")
        return result
