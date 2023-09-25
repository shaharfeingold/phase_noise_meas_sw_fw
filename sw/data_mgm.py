#!/opt/homebrew/bin/python3

import defines
import connection_module
import binascii
import logging
import numpy as np
import matplotlib.pyplot as plt
import utilis_func as utilis

""" 
file: data_mgm.py
owner: shaharf
description: store data, get data, send request, manage data
             before data recv server send to client to amount of lines to store, client keep track
             rise the flag when all the expected data recv.
"""


class Data:
    def __init__(self):
        self.real_data = []
        self.img_data = []
        self.data_size = 0
        self.data_size_expected = 0
        self.data_type = 0 # only real -> 0, only img -> 1, both real and img -> 2
        self.sample_freq = 50 # sampling frequncy in units of MHz # todo shahar review this defualt value # todo shahar prio1 need to pass this parameters using the config pkt or using a define
        self.req_sent = False
        self.data_rcvr = False
    
        # create a logger under Data class
        self.logger = logging.getLogger(__name__)
        self.logger.setLevel(logging.DEBUG)
        self.handler = logging.StreamHandler()
        self.handler.setLevel(logging.DEBUG)
        self.formater = logging.Formatter('%(name)s | %(message)s')
        self.handler.setFormatter(self.formater)
        self.logger.addHandler(self.handler)

    def __repr__(self):
        result = "------------"
        result = result + '\n' + "Data Size : {}".format(str(self.data_size))
        result = result + '\n' + "Req To Send Data Send ? : {}".format(str(self.req_sent))
        result = result + '\n' + "Data Received From Server ? : {}".format(str(self.data_rcvr))
        return result

    def send_req_to_data(self):
        # todo make sure it is resamlbe to logic config module
        return

    def check_all_data_recv(self):
        self.logger.debug("entered check_all_data_recv")
        if self.data_size == self.data_size_expected:
            self.logger.debug("return value from check_all_data_recv = True")
            return True
        self.logger.debug("return value from check_all_data_recv = FALSE")
        return False
    
    def decode_header(self, logic_unit):
        self.logger.debug("entered decode header")
        # todo shahar review, we assume that header is send and recived
        header = logic_unit.rcvr_data().decode()
        self.logger.debug("recv header from the socket = %s", header)
        pkt_type = header[0:2]
        len = header[2:10]
        array_type = header[10:12]
        self.data_type = int(array_type, 16)
        self.data_size_expected = int(len, 16)
        self.logger.debug("the decoded header is: pkt_type = %s, data_type = %d, expected_length = %d", pkt_type, self.data_type, self.data_size_expected)

    # todo shahar need to also include in decode_header rcve function !!! prio1
    def store_new_line_wrapper(self, logic_unit):
        self.logger.debug("entered store_new_line_wrapper when data_type = %s", self.data_type)
        if (self.data_type == defines.REAL_DATA_MSG):
            # todo shahar need to review waht is the data width in B we are sending and make it as a defines
            buffer = logic_unit.client_socket.recv(4)
            hex_string = binascii.hexlify(buffer).decode()
            self.logger.debug("recv from socket = %s", hex_string)
            self.store_new_line(hex_string)
        elif (self.data_type == defines.IMG_DATA_MSG):
            buffer = logic_unit.client_socket.recv(4)
            hex_string = binascii.hexlify(buffer).decode()
            self.logger.debug("recv from socket = %s", hex_string)
            self.store_new_line(hex_string)
        else:
            buffer = logic_unit.client_socket.recv(4)
            hex_string_1 = binascii.hexlify(buffer).decode()
            buffer = logic_unit.client_socket.recv(4)
            hex_string_2 = binascii.hexlify(buffer).decode()
            self.logger.debug("recv from socket = %s and %s", hex_string_1, hex_string_2)
            self.store_new_line(hex_string_1, hex_string_2)

    def store_new_line(self, item, item2 = ''):
        self.logger.debug("entered store_new_line")
        # todo shahar need to see that on real + img together.
        # todo shahar need to convert string to hex. defined later after we knoew the fixed point format.
        if self.check_all_data_recv():
            print("Error trying to store new item but all data already received in client side")
            # todo shaharf swith to exception or error handling !!!
            return
        self.data_size += 1
        if (self.data_type == defines.REAL_DATA_MSG):
            self.real_data.append(item)
        elif (self.data_type == defines.IMG_DATA_MSG):
            self.img_data.append(item)
        else:
            self.real_data.append(item)
            self.img_data.append(item2)

    def check_all_data_recv_and_mark(self):
        self.logger.debug("check_all_data_recv_and_mark")
        if self.data_size == self.data_size_expected:
            self.data_rcvr = True
            return True
        return False

    def get_index(self, index):
        self.logger.debug("entered get_index")
        if index > (self.data_size - 1):
            print("Trying to access item out of range")
            return
            # todo shaharf switch to exception or error handling !!!
        return self.data[index]

    def compute_ftt(self):
        # todo shahar review, if we are using this methods we are assuming that we are getting the data before fft (only real value)
        # Create a sample signal
        if (self.data_type != defines.REAL_DATA_MSG):
            self.logger.debug("Error ! trying to compute fft with value which are not only real")
            # todo shahar continue implement how to handle this.
        
        signal = np.array(utilis.convertList32HexToInt(self.real_data))
        # print(self.real_data)
        # print(signal)
        # Compute the FFT   
        fs = self.sample_freq*1000000 # todo converd to Hz units
        fft_result = np.fft.fft(signal)
        frequencies = np.fft.fftfreq(len(fft_result), 1/fs)  # Frequency values

        # Plot the FFT result
        plt.figure(figsize=(10, 6))
        plt.plot(frequencies, np.abs(fft_result))
        plt.title("FFT ")
        plt.xlabel("Frequency (Hz)")
        plt.ylabel("Amplitude")

        plt.tight_layout()
        plt.show()