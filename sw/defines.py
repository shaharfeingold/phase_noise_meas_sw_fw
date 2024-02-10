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
START = 4
RESTART = 253
REDO = 254
END_CONNECTION = 255 #0xFF (1B)

# todo shaharf review if enough
MAX_MSG_SIZE = 1024  # in terms of bytes, the amount of the bytes to read from socket at once
# todo shahar review above if it is enough or can be reduced.

# data msg types defines:
REAL_DATA_MSG = 0
IMG_DATA_MSG = 1
REAL_IMG_DATA_MSG = 2

DEBUG = True # todo shahar review this to init main flow and not debug mode

PHASE_INC_WIDTH = 16

MAX_FREQUENCY = 20 # maximum frequency value from the user the system is supported. # todo prio1 shahar review before finihs the project

CLOCK_FREQ = 5 # in MHz //todo shahar review changed from 50 to 5 due to down_sampler inside the logic

MAX_ARRAY_SIZE = 1024