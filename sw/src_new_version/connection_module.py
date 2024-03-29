#!/opt/homebrew/bin/python3

""" file: connection_module.py
    owner: shaharf
    description: class which hold the socket info client side, hold the address of logic and methods which are connected
                 to logic for send and rcvr"""

import defines
import sys
import os
import time
import subprocess
import socket
import logging
import defines
from error_handling import handle_fatal_error, handle_medium_error, handle_easy_error
import socket


class LogicConnection:
    def __init__(self):
        self.logic_ip = 0
        self.logic_port = 0
        self.logic_address = 0
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # client socket to communicate with logic modules
        self.connection_established = False
        self.user_wants_to_close = False

        # create a logger under LogicConnection class
        self.logger = logging.getLogger(__name__)
        self.logger.setLevel(logging.DEBUG)
        self.handler = logging.StreamHandler()
        self.handler.setLevel(logging.DEBUG)
        self.formater = logging.Formatter('%(name)s | %(message)s')
        self.handler.setFormatter(self.formater)
        self.logger.addHandler(self.handler)

    def __str__(self):
        result = "-------------"
        result = result + "\nlogic ip : " + str(self.logic_ip)
        result = result + "\nlogic port : " + str(self.logic_port)
        result = result + "\nConnection Established : " + str(self.connection_established)
        result = result + "\n-------------"
        return result
    
    def __del__(self):
        if self.client_socket:
            self.client_socket.close()
        self.logger.removeHandler(self.handler)  # Shut down the logger
        self.handler.close()


    def send_data(self, data):  # assuming data is converted to bytes object utf 8,
        """
        :args: #todo shahar to be defined
        :description: get data to send, according to type parse it and send it through socket
        :return: # todo shahar TBD
        """
    def send_data(self, data):
        try:
            self.client_socket.sendall(data)
            #print("sending data :", data)
        except Exception as e:
            handle_medium_error(f"Error occurred while sending data: {e}")
            self.close_connection()  # Close the socket if an error occurs

    def rcvr_data(self):
        """
        :args: #todo shahar to be defined
        :description: get data to send, according to type parse it and send it through socket
                      # todo shahar need to define the best way to wait for rcvr without block the exe. consider thread
                      # or subprocess
        :return: # todo shahar TBD
        """
        try:
            data_rcvr = self.client_socket.recv(defines.MAX_MSG_SIZE) # todo shahar review this and remove later
            # may cause a problem when sending data segments less then 1024 B
            return data_rcvr
        except Exception as e:
            handle_medium_error(f"Error occurred while receiving data: {e}")
            self.close_connection()  # Close the socket if an error occurs
            return None

    def connect_socket(self):
        try:
            self.client_socket.connect(self.logic_address)
            self.connection_established = True
        except ConnectionRefusedError as e:
            handle_medium_error("Connection refused. Make sure the server is running.")
            self.cleanup()  # Clean up before exiting
            sys.exit(2)
        except TimeoutError as e:
            handle_medium_error("Connection timed out. The server might be unreachable.")
            self.cleanup()  # Clean up before exiting
            sys.exit(2)
        except socket.error as e:
            handle_medium_error(f"Socket error: {e}")
            self.cleanup()  # Clean up before exiting
            sys.exit(2)
        return 0  
    
    def cleanup(self):
    # Logic to clean up resources
        if self.client_socket:
            self.client_socket.close()

    def get_from_user_connection_info(self, ip, port):
        self.logic_ip = ip
        self.logic_port = int(port)
        self.logic_address = (self.logic_ip, self.logic_port)

    def close_connection(self):
        # todo shaharf need to add flag or indication that user wants to close and server finish sending info
        # todo shaharf we are not relaying on tcp protocol close connection procedure.
        try:
            # pkt_to_send = self.build_packet_close_connection()
            # self.send_data(pkt_to_send)
            # ack_msg_to_check = self.rcvr_data().decode()
            # if not self.check_ack_close_connection_pkt(ack_msg_to_check):
            #    handle_easy_error("Invalid ACK received for close connection packet.")
            # self.check_ack_close_connection_pkt(ack_msg_to_check)
            self.client_socket.close()
        except Exception as e:
            handle_medium_error(f"Error occurred while closing connection: {e}")  
    
    def build_packet_close_connection(self):
        # the pkt itself is 64 bits long (8B)
        # header format: 1B     | 4B        | 1B
        #                type   | null      | control_byte
        # sender set control byte to FF, recv sends ack msg (echo the msg with unset control byte)
        # to support the defined socket interface between server and client, we need to send 1024 B msg 
        # => add null bytes to header.
        try:
            self.logger.debug("entered build_packet_close_connection")
            null_field = '00000000'
            control_byte = 255
            header = f"{defines.END_CONNECTION:02X}" + null_field + f"{control_byte:02X}"
            # extend to have 64 bits long pkt
            header = header + "0000"
            header = header.encode()

            # fill with null chars to have 1024 B msg.
            desired_length = 1024 # todo shahar switch to defines.
            # todo shahar maybe need to 1023 because of the null char that end the entire string. (review with print both client and server)
            # todo shahar need to support the case when we don't need to add null chars (the header itself enough)

            # Calculate the number of null characters to add
            null_characters = b'\x00' * (desired_length - len(header))

            # Concatenate the original string with null characters to reach the desired length
            extended_string = header + null_characters
            return extended_string # the return value is encoded string
        except Exception as e:
            handle_medium_error(f"trying to send close connection packet:{e} but didn't established the connection yet")
            return None
        #if (not self.connection_established):
            #self.logger.debug("trying to send close connection packet but didn't established the connection yet")
            #raise Exception
            # todo shahar need to define how to handle and what to do unter this error   

    def check_ack_close_connection_pkt(self, pkt_to_check):
        self.logger.debug("entered check_ack_close_connection_pkt")
        rcev_pkt_type = int(pkt_to_check[0:2], 16)
        rcev_control_byte = int(pkt_to_check[10:12], 16)
        self.logger.debug("rcev_pkt_type = %d, recv_control_byte = %d", rcev_pkt_type, rcev_control_byte)
        # check pkt
        if ((rcev_control_byte != 0) or (rcev_pkt_type != defines.END_CONNECTION)):
            self.logger.debug("rcve ack close connection packet is not a valid one")
            self.logger.debug("recv_pkt_type = %d, recv_control_byte = %d", rcev_pkt_type, rcev_control_byte)
            return False
            # todo shahar need to define what to do in case false
        return True
    
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