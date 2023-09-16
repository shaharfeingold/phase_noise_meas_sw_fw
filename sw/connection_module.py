#!/opt/homebrew/bin/python3

""" file: connection_module.py
    owner: shaharf
    description: class which hold the socket info client side, hold the address of logic and methods which are connected
                 to logic for send and rcvr"""

import sys
import os
import time
import subprocess
import socket

# todo shaharf review if enough
MAX_MSG_SIZE = 1024  # in terms of bytes, the amount of the bytes to read from socket at once


class LogicConnection:
    def __init__(self):
        self.logic_ip = 0
        self.logic_port = 0
        self.logic_address = 0
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # client socket to communicate with logic modules
        self.connection_established = False

    def __str__(self):
        result = "-------------"
        result = result + "\nlogic ip : " + str(self.logic_ip)
        result = result + "\nlogic port : " + str(self.logic_port)
        result = result + "\nConnection Established : " + str(self.connection_established)
        result = result + "\n-------------"
        return result

    def send_data(self, data):  # assuming data is converted to bytes object utf 8,
        """
        :args: #todo shahar to be defined
        :description: get data to send, according to type parse it and send it through socket
        :return: # todo shahar TBD
        """
        try:
            self.client_socket.sendall(data)
        except Exception as e:
            print("Error occurred ! {}".format(e))  # todo shaharf error handling

    def rcvr_data(self):
        """
        :args: #todo shahar to be defined
        :description: get data to send, according to type parse it and send it through socket
                      # todo shahar need to define the best way to wait for rcvr without block the exe. consider thread
                      # or subprocess
        :return: # todo shahar TBD
        """
        try:
            data_rcvr = self.client_socket.recv(MAX_MSG_SIZE) # todo shahar review this and remove later
            # may cause a problem when sending data segments less then 1024 B
            return data_rcvr
        except Exception as e:
            print("Error occurred ! {}".formt(e))  # todo shaharf error handling

    def connect_socket(self):
        try:
            self.client_socket.connect(self.logic_address)
            self.connection_established = True
            # todo shahar verbose print to log connection success
        except ConnectionRefusedError:
            # todo shahar verbose print to log, error + handle error using return and exit program + show user gui a msg.
            print("Connection refused. Make sure the server is running.")
        except TimeoutError:
            # todo shahar verbose print to log, error + handle error using return and exit program + show user gui a msg.
            # todo shaharf need to define error procedure
            print("Connection timed out. The server might be unreachable.")
        except socket.error as e:
            # todo shahar verbose print to log, error + handle error using return and exit program + show user gui a msg.
            print("Socket error:", e)
        return 0  # todo shaharf if are here we connect succfully

    def get_from_user_connection_info(self, ip, port):
        self.logic_ip = ip
        self.logic_port = int(port)
        self.logic_address = (self.logic_ip, self.logic_port)

    def close_connection(self):
        # todo shaharf need to add flag or indication that user wants to close and server finish sending info
        # todo shaharf we are not relaying on tcp protocol close connection procedure.
        try:
            self.client_socket.close()
        except Exception as e:
            print("Error occurred ! {}".formt(e))  # todo shaharf error handling

"""""
import socket

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# The server's address and port to connect to
server_address = ('localhost', 12345)  # Use the server's address and port
client_socket.connect(server_address)

try:
    # Sending data to the server
    message = "Hello, server!"
    client_socket.sendall(message.encode())

    # Receiving data from the server
    data = client_socket.recv(1024)
    print("Received:", data.decode())

finally:
    client_socket.close()  # Close the client socket when done
    

# Encode the string into bytes (using UTF-8 encoding)
string_bytes = my_string.encode('utf-8')

# Send the bytes over the socket
client_socket.sendall(string_bytes)
"""