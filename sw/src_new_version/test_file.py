#!/opt/homebrew/bin/python3

"""
file: test_file
owner: shaharf
description: tmp file to test various function and flow and algorithm
"""

import sys
import os
import time
import subprocess
from logic_config import LogicConfig as LC
from connection_module import LogicConnection as connection


def main():
    # red_p = connection()
    # ip = input("enter ip addres\n")
    # port = input("enter port\n")
    # print(red_p)
    # red_p.get_from_user_connection_info(ip, port)
    # print(red_p)
    # red_p.connect_socket()
    # print(red_p)
    # msg_to_send = "hello from my mac"
    # red_p.send_data(msg_to_send.encode())
    # print(red_p.rcvr_data().decode())
    # red_p.close_connection()
    # print(red_p)
    # config = LC()
    # usr_input = input("please enter freq\n")
    # print(usr_input)
    # config.get_freq_from_user(usr_input)
    # print("config print after get usr input")
    # print(config)
    # config.send_to_logic()
    # print("config print after send to logic")
    # print(config)
    # config.ack_rcvr()
    # print("config print after get ack")
    # print(config)


if __name__ == "__main__":
    main()
    exit(0)