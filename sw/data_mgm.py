#!/opt/homebrew/bin/python3

""" 
file: data_mgm.py
owner: shaharf
description: store data, get data, send request, manage data
             before data recv server send to client to amount of lines to store, client keep track
             rise the flag when all the expected data recv.
"""


class Data:
    def __init__(self):
        self.data = []
        self.data_size = 0
        self.data_size_expected = 0
        self.req_sent = False
        self.data_rcvr = False

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
        if self.data_size == self.data_size_expected:
            return True
        return False

    def store_new_line(self, item):
        if self.check_all_data_recv():
            print("Error trying to store new item but all data already received in client side")
            # todo shaharf swith to exception or error handling !!!
            return
        self.data_size += 1
        self.data.append(item)

    def check_all_data_recv_and_mark(self):
        if self.data_size == self.data_size_expected:
            self.data_rcvr = True
            return True
        return False

    def get_index(self, index):
        if index > (self.data_size - 1):
            print("Trying to access item out of range")
            return
            # todo shaharf switch to exception or error handling !!!
        return self.data[index]