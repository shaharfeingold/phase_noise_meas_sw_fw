#!/opt/homebrew/bin/python3

""" file: logic_config.py
    owner: shaharf
    description: send to logic unit configuration to set up for measurment 
    comments: need to check all prints are changed to logger and set to debug 
"""

import sys
import os
import time
import subprocess
import connection_module
import defines
from error_handling import handle_fatal_error, handle_medium_error, handle_easy_error
import socket

class LogicConfig:
    def __init__(self):
        self.freq = 0  # user freq input using get_freq_from_user
        self.phase_inc = 0  # this value send to logic's cordic ip
        self.logic_clk = 50 # todo shahar review according to final logic implementaion| value in MHz 
        self.send_config = False
        self.got_ack = False
        self.store_config = False
        self.start_sent = False
        self.got_finish = False

    def __repr__(self):
        result = "------------------------------"
        result = result + "\nThe following configuration is chosen\n" + "freq : " + str(self.freq)
        result = result + "\nphase inc : " + str(self.phase_inc)
        result = result + "\nstore config : " + str(self.store_config)
        result = result + "\nsend config : " + str(self.send_config)
        result = result + "\ngot ack : " + str(self.got_ack)
        result = result + "\n------------------------------"
        return result

    def get_freq_from_user(self, freq):
        self.freq = int(freq)
        # todo shaharf, add here verbose print to debug.
        self.store_config = True
        try:
            self.convert_freq_to_phase_inc()
        except Exception as e:
            handle_medium_error(f"Error converting frequency to phase increment: {e}")


    def send_to_logic_start_header(self, logic_unit, start_indication):
        """
        """
        if not self.send_config:
            handle_easy_error("Attempted to send start header without configuration")
            return 
        self.start_sent = True
        header = self.build_start_header(start_indication)
        logic_unit.send_data(header) 

    def send_to_logic_end_header(self, logic_unit, end_of_op_user_choice):
        """
        """
        if not self.got_finish:
            handle_easy_error("Attempted to send end header without finishing operation")
            return
        header = self.build_end_header(end_of_op_user_choice)
        logic_unit.send_data(header) 

    def send_to_logic(self, logic_unit): #todo shahar need to change this name to send_to_logic_config_info
        """
        this function return the config freq the user handed to SW. and update its class
        :return: freq.
        """
        if not self.store_config:
            handle_easy_error("Attempted to send configuration without storing it")
            return
        self.send_config = True
        header = self.build_config_header()
        logic_unit.send_data(header)

    def end_ack_rcvr(self, logic_unit):
        """
        """
        if not self.got_finish:
            # todo shaharf, handle error, should not reach this code section if we didn't get send start.
            exit(2)  # todo shaharf, need to see if there is any better way to handle error
        ack_msg = logic_unit.rcvr_data().decode()
        print(ack_msg)
        if (not self.check_end_ack_msg(ack_msg)):
            # todo shahar need to define what to do if we reach here.
            exit(2)

    def start_ack_rcvr(self, logic_unit):
        """
        """
        if not self.start_sent:
            # todo shaharf, handle error, should not reach this code section if we didn't get send start.
            exit(2)  # todo shaharf, need to see if there is any better way to handle error
        ack_msg = logic_unit.rcvr_data().decode()
        print(ack_msg)
        if (not self.check_start_ack_msg(ack_msg)):
            # todo shahar need to define what to do if we reach here.
            exit(2)

    def ack_rcvr(self, logic_unit): # todo shahar need to change the name to config_ack_rcvr
        if not self.store_config:
            # todo shaharf, handle error, should not reach this code section if we didn't get any config.
            exit(2)  # todo shaharf, need to see if there is any better way to handle error
        if not self.send_config:
            # todo shaharf, if we reach here before we send anything to logic error
            exit(2)
        ack_msg = logic_unit.rcvr_data().decode()
        print(ack_msg)
        if (not self.check_ack_msg(ack_msg)):
            # todo shahar need to define what to do if we reach here.
            exit(2)
        self.got_ack = True

    def check_end_ack_msg(self, ack_msg):
        """
        """
        rcev_pkt_type = int(ack_msg[0:2], 16)
        print(rcev_pkt_type)
        rcev_end_byte = int(ack_msg[2:10], 16)
        print(rcev_end_byte)
        rcev_control_byte = int(ack_msg[10:12], 16)
        print(rcev_control_byte)
        if (rcev_pkt_type != defines.CONFIG):
            return False
        if (rcev_end_byte < 253):
            return False
        if (rcev_control_byte != 0):
            return False
        return True
    
    def check_start_ack_msg(self, ack_msg):
        """
        """
        rcev_pkt_type = int(ack_msg[0:2], 16)
        print(rcev_pkt_type)
        rcev_start_bit = int(ack_msg[2:10], 16)
        print(rcev_start_bit)
        rcev_control_byte = int(ack_msg[10:12], 16)
        print(rcev_control_byte)
        if (rcev_pkt_type != defines.START):
            return False
        if (rcev_start_bit != 1):
            return False
        if (rcev_control_byte != 0):
            return False
        return True
    
    def check_ack_msg(self, ack_msg): # todo shahar need to change the name to check_config_ack_msg
        rcev_pkt_type = int(ack_msg[0:2], 16)
        print(rcev_pkt_type)
        rcev_phase_inc = int(ack_msg[2:10], 16)
        print(rcev_phase_inc)
        rcev_control_byte = int(ack_msg[10:12], 16)
        print(rcev_control_byte)
        if (rcev_pkt_type != defines.CONFIG):
            return False
        if (rcev_phase_inc != self.phase_inc):
            return False
        if (rcev_control_byte != 0):
            return False
        return True

    def convert_freq_to_phase_inc(self):
        # todo shaharf need to include calcutation once we detrmined to bus width of cordic
        # todo shaharf need to review this every time new logic rev is released
        a = (2**defines.PHASE_INC_WIDTH) * self.freq
        b = self.logic_clk
        self.phase_inc = (a/b)  # todo shaharf dummy implemntaion

    def convert_end_usr_choice_to_end_byte(self, end_of_op_user_choice):
        if (end_of_op_user_choice == "1"):
            return defines.END_CONNECTION
        elif (end_of_op_user_choice == "2"):
            return defines.REDO
        elif (end_of_op_user_choice == "3"):
            return defines.RESTART
        else:
            print("got invalid end_usr_choice")
            # todo define error handle routine

    def build_end_header(self, end_of_op_user_choice):
        """
        the header itself is 64 bits long (8B)
        header format: 1B     | 4B            | 1B
                       type   | end_type_byte | control_byte
        sender set control byte to FF, recv sends ack msg (echo the msg with unset control byte)
        to support the defined socket interface between server and client, we need to send 1024 B msg 
        => add null bytes to header.
        """
        end_type_end = self.convert_end_usr_choice_to_end_byte(end_of_op_user_choice)
        control_byte = 255
        header = f"{defines.CONFIG:02X}" + f"{end_type_end:08X}" + f"{control_byte:02X}"
        # extend to have 64 bits long header
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
    
    def build_start_header(self, start_indication):
        """
        the header itself is 64 bits long (8B)
        header format: 1B     | 4B        | 1B
                       type   | start_bit | control_byte
        sender set control byte to FF, recv sends ack msg (echo the msg with unset control byte)
        to support the defined socket interface between server and client, we need to send 1024 B msg 
        => add null bytes to header.
        """
        start_bit = int(start_indication)
        control_byte = 255
        header = f"{defines.START:02X}" + f"{start_bit:08X}" + f"{control_byte:02X}"
        # extend to have 64 bits long header
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
    
    def build_config_header(self):
        # the header itself is 64 bits long (8B)
        # header format: 1B     | 4B        | 1B
        #                type   | phase_inc | control_byte
        # sender set control byte to FF, recv sends ack msg (echo the msg with unset control byte)
        # to support the defined socket interface between server and client, we need to send 1024 B msg 
        # => add null bytes to header.
        phase_inc = self.phase_inc
        control_byte = 255
        header = f"{defines.CONFIG:02X}" + f"{phase_inc:08X}" + f"{control_byte:02X}"
        # extend to have 64 bits long header
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