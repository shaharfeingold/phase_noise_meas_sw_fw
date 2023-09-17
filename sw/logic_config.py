#!/opt/homebrew/bin/python3

""" file: logic_config.py
    owner: shaharf
    description: send to logic unit configuration to set up for measurment """

import sys
import os
import time
import subprocess
import connection_module
import defines


class LogicConfig:
    def __init__(self):
        self.freq = 0  # user freq input using get_freq_from_user
        self.phase_inc = 0  # this value send to logic's cordic ip
        self.send_config = False
        self.got_ack = False
        self.store_config = False

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
        self.convert_freq_to_phase_inc()

    def send_to_logic(self, logic_unit):
        """
        this function return the config freq the user handed to SW. and update its class
        :return: freq.
        """
        if not self.store_config:
            # todo shaharf, handle error, should not reach this code section if we didn't get any config.
            exit(2)  # todo shaharf, need to see if there is any better way to handle error
        self.send_config = True
        header = self.build_config_header()
        logic_unit.send_data(header)

    def ack_rcvr(self, logic_unit):
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

    def check_ack_msg(self, ack_msg):
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
        self. phase_inc = (self.freq * 10)  # todo shaharf dummy implemntaion

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