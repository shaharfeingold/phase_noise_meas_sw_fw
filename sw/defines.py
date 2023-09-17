#!/opt/homebrew/bin/python3

"""
file: defines.py
    owner: shaharf
    description: general defines
    """

# packet types:
IDLE = 0
CONFIG = 1
DATA = 2
ERROR = 3

# data msg types defines:
REAL_DATA_MSG = 0
IMG_DATA_MSG = 1
REAL_IMG_DATA_MSG = 2

DEBUG = True # todo shahar review this to init main flow and not debug mode