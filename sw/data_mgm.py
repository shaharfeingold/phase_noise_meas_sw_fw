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
        self.sample_freq = defines.CLOCK_FREQ # sampling frequncy in units of MHz # todo shahar review this defualt value # todo shahar prio1 need to pass this parameters using the config pkt or using a define
        self.req_sent = False
        self.data_rcvr = False

        self.signal = '' # signal - only real (after convert to float)
        self.fft_result = '' # fft result of signal
        self.freq_vec_for_fft = '' # freq. vec for fft plot
    
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
    
    def __del__(self):
        # Close any open files or network connections
        # Shut down the logger
        self.logger.removeHandler(self.handler)
        self.handler.close()


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
    # todo shahar need to review waht is the data width in B we are sending and make it as a defines
    def store_new_line_wrapper(self, logic_unit):
        self.logger.debug("entered store_new_line_wrapper when data_type = %s", self.data_type)
        try:
            if self.data_type == defines.REAL_DATA_MSG or self.data_type == defines.IMG_DATA_MSG:
                buffer = logic_unit.client_socket.recv(4)
                hex_string = binascii.hexlify(buffer).decode()
                self.logger.debug("recv from socket = %s", hex_string)
                self.store_new_line(hex_string)
            else:  # For both real and img data # todo review , i think this broken # todo shahar need to fix implementaion
                for _ in range(2):
                    buffer = logic_unit.client_socket.recv(4)
                    hex_string = binascii.hexlify(buffer).decode()
                    self.logger.debug("recv from socket = %s", hex_string)
                    self.store_new_line(hex_string)
        except socket.error as e:
            handle_medium_error(f"Socket error during data reception: {e}")
            logic_unit.close_connection()
            return


    def store_new_line(self, item, item2 = ''):
        self.logger.debug("entered store_new_line")
        # todo shahar need to see that on real + img together.
        # todo shahar need to convert string to hex. defined later after we knoew the fixed point format.
        if self.check_all_data_recv():
            handle_easy_error("Attempted to store new item but all data already received")
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
            handle_easy_error("Attempt to access item out of range in get_index")
            return None
        return self.data[index]

    def compute_ftt(self):
        # todo shahar review, if we are using this methods we are assuming that we are getting the data before fft (only real value)
        # Create a sample signal
        if (self.data_type != defines.REAL_DATA_MSG):
            handle_easy_error("Attempted to compute FFT with invalid data type")
            return
            # todo shahar continue implement how to handle this.
        
        self.signal = np.array(utilis.convertList32HexToFloat(self.real_data))
        # print(self.real_data)
        # print(signal)
        # save the data in a file and send it to user. # todo shahar ask from user the mail address.
        # Compute the FFT   
        fs = self.sample_freq*1000000 # todo converd to Hz units
        self.fft_result = np.fft.fft(self.signal)
        #self.save_and_send_array_in_a_file(signal, self.fft_result)
        self.freq_vec_for_fft = np.fft.fftfreq(len(self.fft_result), 1/fs)  # Frequency values

        # Plot the FFT result
        plt.figure(figsize=(10, 6))
        plt.plot(self.freq_vec_for_fft, np.abs(self.fft_result))
        plt.title("FFT ")
        plt.xlabel("Frequency (Hz)")
        plt.ylabel("Amplitude")

        plt.tight_layout()
        plt.show()

    # def save_and_send_array_in_a_file(self, signal, fft_result):
    #     current_datetime = datetime.now()
    #     formatted_date_time = current_datetime.strftime('%d%m%Y_%H_%M_%S')
    #     file_path_1 = "signal_" + formatted_date_time + ".txt"
    #     file_path_2 = "fft_result_" + formatted_date_time + ".txt"

    #     try:
    #         with open(file_path_1, 'w') as file:
    #             np.savetxt(file, signal, delimiter=',')
    #         with open(file_path_2, 'w') as file:
    #             np.savetxt(file, fft_result, delimiter=',')

    #         self.send_file_to_mail(file_path_1, file_path_2)

    #     except Exception as e:
    #         handle_easy_error(f"Error saving or sending files: {e}")

    
    # def send_file_to_mail(self, file_path_1, file_path_2):
    #     body_msg = "attached below raw result for further analysis"
    #     command1 = ['echo', body_msg] 
    #     command2 = ['mutt' , '-s' ,'phase noise meas - raw result' , '-a' , file_path_1, '-a', file_path_2  , '--' , 'shahar.feingold@gmail.com']


    #     #todo change the address to user address
    #     self.logger.debug("%s\n%s", command1, command2)
    #     try:
    #         process1 = subprocess.run(command1, stdout=subprocess.PIPE, text=True)
    #         output1 = process1.stdout
    #         # Use the output of the first command as input for the second command
    #         process2 = subprocess.run(command2, input=output1, stdout=subprocess.PIPE, text=True)
            
    #     except subprocess.CalledProcessError as e:
    #         # If the command execution fails, capture the error
    #         print("Command '{}' returned non-zero exit status {}.".format(e.cmd, e.returncode))
    #         print("Error output:\n", e.stderr)

        
    
    def CopyHeaderFromOther(self, other_meas_data):
        self.logger.debug("entered CopyHeaderFromOther")
        self.data_type = other_meas_data.data_type
        self.data_size_expected = other_meas_data.data_size_expected