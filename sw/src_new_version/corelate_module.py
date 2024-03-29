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
from scipy.signal import correlate, welch, decimate


""" 
file: data_mgm.py
owner: shaharf
description: store data, get data, send request, manage data
             before data recv server send to client to amount of lines to store, client keep track
             rise the flag when all the expected data recv.
"""


class Corelate:
    def __init__(self, RepeatCount):
        # todo sfeingold cleanup of unused vars
        self.signal_ch0 = ''
        self.fft_signal_ch0 = ''
        self.signal_ch1 = ''
        self.fft_signal_ch1 = ''
        self.NumOfChannels = 0
        self.corelate_result = ''
        self.RepetitionCounter = 0
        self.RepeatCount = RepeatCount
        self.fft_size = defines.MAX_ARRAY_SIZE #todo review in case we want to know pad with zeros with fft
        self.psd_est = np.zeros(self.fft_size)
        self.psd_est_1 = np.zeros(1025)
        self.psd_est_1_dec = np.zeros(1025)
        self.psd_est_1_dec2 = np.zeros(1025)

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
        self.signal_ch0 = meas_data_ch0.signal
        #self.fft_signal_ch0 = meas_data_ch0.fft_result
        self.signal_ch1 = meas_data_ch1.signal
        #self.fft_signal_ch1 = meas_data_ch1.fft_result

        self.NumOfChannels = NumOfChannels
        self.RepetitionCounter += 1
    
    def CalcPsd(self, Id):
            #todo cleanup
        #if self.NumOfChannels == 2:
            #self.corelate_result = np.correlate(self.signal_ch0, self.signal_ch1, mode='full')
            #mixing = self.signal_ch0 * np.conjugate(self.signal_ch1)
            #corelate_result_log = np.log10(self.corelate_result)
            #mixing_log = np.log10(mixing)
            #size = int(len(self.corelate_result))
            # fs = defines.CLOCK_FREQ * 1000000
        diff_signal = self.signal_ch0 - self.signal_ch1
        plt.plot(self.signal_ch0[0:100])
        plt.show()
        exit(1)
        diff_signal_unwrap = np.unwrap(diff_signal)
        utilis.save_signal_to_file(diff_signal_unwrap, Id, self.RepetitionCounter-1) # RepetitionCounter -1 to start from zero
            # diff_signal_dec_10 = decimate(diff_signal_unwrap, 10)
            # diff_signal_dec_100 = decimate(diff_signal_dec_10, 10)
            # fft_diff_signal = np.fft.fft(diff_signal_unwrap)
            # # fft_size = len(diff_signal_unwrap)
            # #using paradigm methods directly
            # self.psd_est = self.psd_est + (1/self.fft_size) * (fft_diff_signal * np.conjugate(fft_diff_signal))
            # # using welch methods from scipy
            # freq, temp = welch(diff_signal_unwrap, fs=fs, nfft=2048)
            # freq1, temp1 = welch(diff_signal_dec_10, fs=fs/10, nfft=2048)
            # freq2, temp2 = welch(diff_signal_dec_100, fs=fs/100, nfft=2048)
            # self.psd_est_1 = self.psd_est_1 + temp
            # self.psd_est_1_dec = self.psd_est_1_dec + temp1
            # self.psd_est_1_dec2 = self.psd_est_1_dec2 + temp2
            # #plt.plot(np.abs(self.corelate_result))
            # #plt.plot(corelate_result_log.real[0:int(size/2)])
            # #mixingPerHz = np.gradient(mixing, freq_vec)
            # #mixing_log = np.log10(mixingPerHz)
            # #freq_vec = np.fft.fftfreq(len(self.signal_ch0), 1/fs)
            
            # if (self.RepetitionCounter == self.RepeatCount):
            #     self.psd_est = self.psd_est / self.RepeatCount
            #     freq_vec = np.arange(0,self.fft_size, 1) * (fs/self.fft_size)
            #     psd_est_db = 10 * np.log10(self.psd_est)
                
            #     plt.figure(figsize=(10, 6))
            #     plt.plot(freq_vec[0:self.fft_size//2], psd_est_db.real[0:self.fft_size//2])
            #     plt.xscale('log')
            #     plt.title("PSD - paradigm")
            #     plt.xlabel("Frequency (Hz)")
            #     plt.ylabel("db/Hz")

            #     plt.tight_layout()

            #     # figure for the welch algo
            #     plt.figure()
            #     plt.semilogx(freq, 10*np.log10(self.psd_est_1/self.RepeatCount))
            #     plt.xscale('log')
            #     plt.title("PSD - welch")
            #     plt.xlabel("Frequency (Hz)")
            #     plt.ylabel("db/Hz")
            #     plt.xlim(0.1,1000000)

            #     # figure for the welch algo with dec by 10 before
            #     plt.figure()
            #     plt.semilogx(freq1, 10*np.log10(self.psd_est_1_dec/self.RepeatCount))
            #     plt.xscale('log')
            #     plt.title("PSD - welch with 10 down sampling")
            #     plt.xlabel("Frequency (Hz)")
            #     plt.ylabel("db/Hz")
            #     plt.xlim(0.1,1000000)

            #     # figure for the welch algo with dec by 100 before
            #     plt.figure()
            #     plt.semilogx(freq2, 10*np.log10(self.psd_est_1_dec2/self.RepeatCount))
            #     plt.xscale('log')
            #     plt.title("PSD - welch with 100 down sampling")
            #     plt.xlabel("Frequency (Hz)")
            #     plt.ylabel("db/Hz")
            #     plt.xlim(0.1,1000000)

            #     plt.show()

                