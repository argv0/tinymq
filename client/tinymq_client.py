#!/usr/bin/env python

import sys
import socket
import struct

class tinymq_client(object):
    GET = 0
    ACK = 1
    PUT = 2
    def __init__(self, ip, port):
        self.ip = ip
        self.port = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.setblocking(1)

    def _send(self, format, data):
        return self.socket.send(struct.pack(format, data))

    def _recv(self, nbytes, format):
        return struct.unpack(format, self.socket.recv(nbytes))

    def connect(self):
        return self.socket.connect((self.ip, self.port))

    def _send_command(self, command):
        return self._send("L", command)

    def _send_get(self):
        self._send_command(self.GET)
        return struct.unpack("LL", self.socket.recv(16))

    def get(self):
        msg_id, msg_len = self._send_get()
        recvd = 0
        buf = []
        while (recvd < msg_len):
            data = self.socket.recv(msg_len)
            buf.append(data)
            recvd += len(data)
        msg_data = "".join(buf)
        return (msg_id, msg_data)

    def ack(self, message_id):
        self.socket.send(struct.pack("LL", self.ACK, message_id))

    def put(self, message):
        msg_len = len(message)
        self.socket.send(struct.pack("LL%ss" % msg_len,
                                     self.PUT, msg_len, message))
        msg_id, = self._recv(8, "L")
        return msg_id

def test():
    c = tinymq_client("127.0.0.1", 5673)
    c.connect()
    msg = "a"*1024
    for i in range(0, 10000):
        print c.put(msg)
    for i in range(0, 10000):
        msg_id, msg_data = c.get()
        print msg_id, msg_data
        c.ack(msg_id)

if __name__ == "__main__":
    test()
