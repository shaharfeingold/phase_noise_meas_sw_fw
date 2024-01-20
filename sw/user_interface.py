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
"""
file : maim.py
owner : shaharf
description : main file to call each class and excute commands and functionality.
comments: # todo shahar later will switch to GUI
"""

def print_hello_msg():
    print("########################################")
    print("Wellcome to phase measurment application")
    print("########################################")
    print("step 1: setting up connection to red pitya.")
    print("please retrive the the ip address and the port your red pitya is listening")

def get_ip_addr():
    ip_addr = input("Enter IP address of Red Pitaya: ")
    return ip_addr

def get_port():
    port = input("Enter Port Num of Red Pitaya: ")
    return port

def print_config_setup_msg():
    print("step 2: please retrive the freq of the oscillator you want to check")

def get_freq_from_user():
    freq = input("Enter Freq of the external oscillator: ")
    return freq

def print_wait_for_start_op():
    print("set up complete")
    print("please connect the oscillator to the red pitya and when reday follow the instruction on the screen")

def get_from_user_start_op_indication():
    result = input("please hit 1 when ready\n")
    return result

def print_wait_to_finish_op():
    print("please wait for finish operation")

def print_data_unload():
    print("red pitya finished its operation, start unloading data")

def print_end_of_op_how_to_proceed():
    print("you close the window of result, please select how to proceed")

def print_end_of_op_options():
    print("*************************************************************************")
    print("if you want to exit please type 1")
    print("if you want to measure again with the same configuration please select 2")
    print("if you want to set new configuration please select 3")
    print("*************************************************************************")
    result = input("please enter your choice\n")
    return result
