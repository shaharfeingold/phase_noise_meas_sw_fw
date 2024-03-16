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
import numpy as np
from scipy.signal import correlate, welch, decimate
import matplotlib.pyplot as plt


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
    parser.add_argument("-p1", "--port1", help="in case of config from comaand line specify the red pitaya port", dest='port1', type=str)
    parser.add_argument("-p2", "--port2", help="in case of config from comaand line specify the red pitaya port", dest='port2', type=str)
    parser.add_argument("-f","--freq", help="oscillator working freq", dest='freq', type=str)
    parser.add_argument("--data_only", help="Run in data-only mode", action="store_true") #just send mail and not showing graph. # todo need to support this
    parser.add_argument("-v", "--verbos", help="verbose print", choices=["info,debug,error,fatal"], type=str) # todo shahar need to support this
    parser.add_argument("-r", "--repetition", help="number of repetition of calculation", dest='repeat', choices=range(1,51), type=int, default=1)
    
    return parser.parse_args()

def get_src_dir():
    # assume that both main files (father and child) are withing the same dir
    return os.path.realpath(os.path.dirname(__file__))

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
    cwd = os.getcwd()
    src_dir = get_src_dir()

    script_name = os.path.join(src_dir, 'main.py')
    script1_args = ['-m', args.mail, '--config_from_command_line', '-i', args.ip1, '-p', args.port1, '-f', args.freq, '-r', str(args.repeat), '-n', '2', '--id', '1']
    script2_args = ['-m', args.mail, '--config_from_command_line', '-i', args.ip2, '-p', args.port2, '-f', args.freq, '-r', str(args.repeat), '-n', '2', '--id', '2']

    print([script_name] + script1_args)

    # Start the scripts in parallel
    process1 = subprocess.Popen(['python3', script_name] + script1_args)
    #process2 = subprocess.Popen(['python3', script_name] + script2_args)

    # Wait for both processes to finish
    process1.wait()
    #process2.wait()

    ########################################################

    ########################################################
    ##                  Data Anylsis                      ##
    ########################################################
    repeat_counter = 0 
    temp_psd_estimation = np.zeros(1025)
    temp_psd_est_down_10 = np.zeros(1025)
    temp_psd_est_down_100 = np.zeros(1025)
    freq_vec = 0
    freq_vec1 = 0
    freq_vec2 = 0
    fs = defines.CLOCK_FREQ * 1000000
    while(repeat_counter < args.repeat):
        # 1. according to run id and repeat nubmer: open each file and load to a numpy array
        id1_file_path = "signal_id{}_r{}.txt".format(1, repeat_counter)
        id2_file_path = "signal_id{}_r{}.txt".format(2, repeat_counter)
        data_id1 = np.genfromtxt(id1_file_path)
        # data_id2 = np.genfromtxt(id2_file_path)
        #   a. decimate if we want to decimate
        data_id1_down10 = decimate(data_id1, 10)
        data_id1_down100 = decimate(data_id1_down10, 10)
        # 2. corelate 2 vectors
        # todo add here
        # 3. estimate the PSD
        freq, temp = welch(data_id1, fs=fs, nfft=2048) # todo change to the corelate version
        freq1, temp1 = welch(data_id1_down10, fs=fs/10, nfft=2048) # todo change to the corelate version
        freq2, temp2 = welch(data_id1_down100, fs=fs/100, nfft=2048) # todo change to the corelate version
        # 4. add to the result vector
        temp_psd_estimation = temp_psd_estimation + temp
        temp_psd_est_down_10 = temp_psd_est_down_10 + temp1
        temp_psd_est_down_100 = temp_psd_est_down_100 + temp2
        freq_vec = freq
        freq_vec1 = freq1
        freq_vec2 = freq2
        repeat_counter += 1
    
    # 5. div to normelize.
    psd_estimation = temp_psd_estimation / args.repeat
    psd_est_d10 = temp_psd_est_down_10 / args.repeat
    psd_est_d100 = temp_psd_est_down_100 / args.repeat
    psd_est_db = 10 * np.log10(temp_psd_estimation)
    psd_est_db_d10 = 10 * np.log10(temp_psd_est_down_10)
    psd_est_db_d100 = 10 * np.log10(temp_psd_est_down_100)
    # 6. plot
    plt.figure()
    plt.semilogx(freq_vec, psd_est_db)
    plt.xscale('log')
    plt.title("PSD est - welch algo")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("db/Hz")
    plt.xlim(0.1,1000000)

    plt.figure()
    plt.semilogx(freq_vec1, psd_est_db_d10)
    plt.xscale('log')
    plt.title("PSD est - welch algo | decimate by 10")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("db/Hz")
    plt.xlim(0.1,100000)
    
    plt.figure()
    plt.semilogx(freq_vec2, psd_est_db_d100)
    plt.xscale('log')
    plt.title("PSD est - welch algo | decimate by 100")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("db/Hz")
    plt.xlim(0.1,10000)

    plt.show()
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