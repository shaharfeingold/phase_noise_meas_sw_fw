#!/opt/homebrew/bin/python3

import defines
import connection_module
import binascii
import logging


def convert32HexToInt(num):
    return int(num, 16)

def convertList32HexToInt(num_list):
    result = []
    for item in num_list:
        result.append(convert32HexToInt(item))
    return result

def convert32HexToFloat(num):
    return float(num, 16)

def convertList32HexToFloat(num_list):
    result = []
    for item in num_list:
        result.append(convert32HexToFloat(item))
    return result
# todo shahar implement convert 32 hex to float.