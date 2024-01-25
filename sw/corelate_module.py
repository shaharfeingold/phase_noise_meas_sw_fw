#!/opt/homebrew/bin/python3

import defines
import connection_module
import binascii
import logging
import numpy as np
import matplotlib.pyplot as plt
import utilis_func as utilis
from error_handling import handle_fatal_error, handle_medium_error, handle_easy_error
import socket
from datetime import datetime
import subprocess
import data_mgm


""" 
file: data_mgm.py
owner: shaharf
description: store data, get data, send request, manage data
             before data recv server send to client to amount of lines to store, client keep track
             rise the flag when all the expected data recv.
"""


class Corelate:
    def __init__(self):
        self.signal_ch0 = ''
        self.signal_ch1 = ''
        self.NumOfChannels = 0
        self.corelate_result = ''

        # create a logger under Data class
        self.logger = logging.getLogger(__name__)
        self.logger.setLevel(logging.DEBUG)
        self.handler = logging.StreamHandler()
        self.handler.setLevel(logging.DEBUG)
        self.formater = logging.Formatter('%(name)s | %(message)s')
        self.handler.setFormatter(self.formater)
        self.logger.addHandler(self.handler)


    def GetSignals(self, meas_data_ch0, meas_data_ch1, NumOfChannels):
        self.logger.debug("entered GetSignals")
        self.signal_ch0 = meas_data_ch0.fft_result
        self.signal_ch1 = meas_data_ch1.fft_result
        self.NumOfChannels = NumOfChannels
    
    def CalcCorelate(self):
        if self.NumOfChannels == 2:
            self.corelate_result = np.correlate(self.signal_ch0, self.signal_ch1, mode='full')
            mixing = self.signal_ch0 * np.conjugate(self.signal_ch1)
            corelate_result_log = np.log10(self.corelate_result)
            #mixing_log = np.log10(mixing)
            plt.figure(figsize=(10, 6))
            size = int(len(self.corelate_result))
            #todo cleanup
            fs = fs = 125*1000000
            freq_vec = np.fft.fftfreq(len(self.signal_ch0), 1/fs)
            #plt.plot(np.abs(self.corelate_result))
            #plt.plot(corelate_result_log.real[0:int(size/2)])
            mixingPerHz = np.gradient(mixing, freq_vec)
            mixing_log = np.log10(mixingPerHz)
            plt.plot(mixing_log.real)
            plt.xscale('log')
            plt.title("Cross-Corelation over fft ")
            #plt.xlabel("Frequency (Hz)")
            plt.ylabel("Amplitude")

            plt.tight_layout()
            plt.show()