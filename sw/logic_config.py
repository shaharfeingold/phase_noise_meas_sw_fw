#!/opt/homebrew/bin/python3

""" file: logic_config.py
    owner: shaharf
    description: send to logic unit configuration to set up for measurment """

import sys
import os
import time
import subprocess


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

    def send_to_logic(self):
        """
        this function return the config freq the user handed to SW. and update its class
        :return: freq.
        """
        if not self.store_config:
            # todo shaharf, handle error, should not reach this code section if we didn't get any config.
            exit(2)  # todo shaharf, need to see if there is any better way to handle error
        self.send_config = True
        return self.freq

    def ack_rcvr(self):
        if not self.store_config:
            # todo shaharf, handle error, should not reach this code section if we didn't get any config.
            exit(2)  # todo shaharf, need to see if there is any better way to handle error
        if not self.send_config:
            # todo shaharf, if we reach here before we send anything to logic error
            exit(2)
        self.got_ack = True

    def convert_freq_to_phase_inc(self):
        # todo shaharf need to include calcutation once we detrmined to bus width of cordic
        # todo shaharf need to review this every time new logic rev is released
        self. phase_inc = (self.freq * 10)  # todo shaharf dummy implemntaion
