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
import time
import subprocess
import json

"""
file        : maim_script.py
owner       : shaharf
description : main script to call 2 instacne of sub scripts to have 2 mesaurment of 2 idependet red pitya
todo        : # review and diff the main.py and other files against the first version (without the threding)
"""

def parse_arguments():
    parser = argparse.ArgumentParser(description="Phase Measurement Application")
    parser.add_argument("-m", "--mail", help="Email address to send reports to", type=str, required=True, dest='mail')
    # parser.add_argument("--help", help="Display help message", action="store_true")
    parser.add_argument("--debug_mode", help="Run in debug mode", action="store_true") # todo need to support this
    parser.add_argument("--config_from_command_line", help="Set configuration from command line", dest='batchmode', action="store_true")
    parser.add_argument("-i1", "--ip_addr1", help="in case of config from command line specify the red pitaya ip address", dest='ip1', type=str)
    parser.add_argument("-i2", "--ip_addr2", help="in case of config from command line specify the red pitaya ip address", dest='ip2', type=str)
    parser.add_argument("-p1", "--port1", help="in case of config from comaand line specify the red pitaya port", dest='port1', type=int)
    parser.add_argument("-p2", "--port2", help="in case of config from comaand line specify the red pitaya port", dest='port2', type=int)
    parser.add_argument("-f","--freq", help="oscillator working freq", dest='freq', type=str)
    parser.add_argument("--data_only", help="Run in data-only mode", action="store_true") #just send mail and not showing graph. # todo need to support this
    parser.add_argument("-v", "--verbos", help="verbose print", choices=["info,debug,error,fatal"], type=str) # todo shahar need to support this
    parser.add_argument("-r", "--repetition", help="number of repetition of calculation", dest='repeat', choices=range(1,51), type=int, default=1)
    
    return parser.parse_args()

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

def MainScript(args):
    # right now support without the prints of the main functiom
    # also need to supress the prints of the child process

    ########################################################
    ##          Create And Run of sub-scripts             ##
    ########################################################

    ########################################################

    ########################################################
    ##                  Data Anylsis                      ##
    ########################################################
    repeat_counter = 0 
    while(repeat_counter < args.repeat):
        # 1. according to run id and repeat nubmer: open each file and load to a numpy array
        #   a. decimate if we want
        # 2. corelate 2 vectors
        # 3. estimate the PSD
        # 4. add to the result vector
        repeat_counter += 1
    
    # 5. div to normelize.
    # 6. plot

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
        if (args.port1 == None or args.ip1 == None or args.port2 or args.ip2 or args.freq == None):
            print("If config_from_command_line is set to true need to specify working freq, ip address and port of the red pitaya")
            result = False 

    if (result == False):
        exit(1)

if __name__ == '__main__':
    args = parse_arguments()
    validate_args(args)
    MainScript(args)