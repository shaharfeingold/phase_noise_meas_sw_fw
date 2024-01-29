#!/opt/homebrew/bin/python3

import defines
import connection_module
import binascii
import logging
import struct
import numpy as np
from datetime import datetime
import subprocess
from error_handling import handle_fatal_error, handle_medium_error, handle_easy_error


def convert32HexToInt(num):
    return int(num, 16)

def convertList32HexToInt(num_list):
    result = []
    for item in num_list:
        result.append(convert32HexToInt(item))
    return result

def convert32HexToFloat(num):
    bytes_data = bytes.fromhex(num)

    # Unpack bytes as a float
    result = struct.unpack('!f', bytes_data)[0]
    print("value before " + num + " value after " + str(result))
    return result

def convertList32HexToFloat(num_list):
    result = []
    for item in num_list:
        result.append(convert32HexToFloat(item))
    return result
# todo shahar implement convert 32 hex to float.

def save_and_send_array_in_a_file(meas_data_ch0, meas_data_ch1, mail, path_list):
    current_datetime = datetime.now()
    formatted_date_time = current_datetime.strftime('%d%m%Y_%H_%M_%S')
    file_path_1_ch0 = "signal_" + formatted_date_time + "_ch0.txt"
    file_path_2_ch0 = "fft_result_" + formatted_date_time + "_ch0.txt"
    file_path_1_ch1 = "signal_" + formatted_date_time + "_ch1.txt"
    file_path_2_ch1 = "fft_result_" + formatted_date_time + "_ch1.txt"

    try:
        with open(file_path_1_ch0, 'w') as file:
            np.savetxt(file_path_1_ch0, meas_data_ch0.signal, delimiter=',')
            path_list.append(file_path_1_ch0)
        with open(file_path_1_ch1, 'w') as file:
            np.savetxt(file_path_1_ch1, meas_data_ch1.signal, delimiter=',')
            path_list.append(file_path_1_ch1)
        with open(file_path_2_ch0, 'w') as file:
            np.savetxt(file_path_2_ch0, meas_data_ch0.fft_result, delimiter=',')
            path_list.append(file_path_2_ch0)
        with open(file_path_2_ch1, 'w') as file:
            np.savetxt(file_path_2_ch1, meas_data_ch1.fft_result, delimiter=',')
            path_list.append(file_path_2_ch1)

        # move the call to outside to be able to send one mail instead of many in case of repetiton
        #send_file_to_mail(file_path_1_ch0, file_path_1_ch1, file_path_2_ch0, file_path_2_ch1, mail)

    except Exception as e:
        handle_easy_error(f"Error saving or sending files: {e}")
    return path_list

    
def send_file_to_mail(path_list, mail):
    body_msg = "attached below raw result for further analysis"
    #files = ' '.join(['-a ' + s for s in path_list])
    files_sublist = [['-a ', s] for s in path_list]
    files = [elem for sublist in files_sublist for elem in sublist]
    command1 = ['echo', body_msg] 
    command2 = ['mutt' , '-s' ,'phase noise meas - raw result'] + files + ['--' , mail]


    #todo change the address to user address
    # self.logger.debug("%s\n%s", command1, command2)
    print(command1)
    print(command2)
    try:
        process1 = subprocess.run(command1, stdout=subprocess.PIPE, text=True)
        output1 = process1.stdout
        # Use the output of the first command as input for the second command
        process2 = subprocess.run(command2, input=output1, stdout=subprocess.PIPE, text=True)
            
    except subprocess.CalledProcessError as e:
        # If the command execution fails, capture the error
        print("Command '{}' returned non-zero exit status {}.".format(e.cmd, e.returncode))
        print("Error output:\n", e.stderr)