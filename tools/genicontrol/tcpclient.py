#!/usr/bin/env python
# -*- coding: utf-8 -*-

##
##
## Grundfos GENIBus Library for Arduino.
##
## (C) 2007-2013 by Christoph Schueler <github.com/Christoph2,
##                                      cpu12.gems@googlemail.com>
##
##  All Rights Reserved
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License along
## with this program; if not, write to the Free Software Foundation, Inc.,
## 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
##
##


import logging
import socket
import struct
import sys
import time
import threading
from genicontrol.configuration import Config

logger = logging.getLogger("genicontrol")

config = Config()

config.loadConfiguration()  ## TEST!!!

print config.serverIP
print config.subnetMask
print config.serverPort


#SERVER = '192.168.1.2'  # TODO: Adjust to the IP-address of your Arduino board!

#SERVER = 'localhost'
SERVER = socket.gethostname()
PORT = 6734

BUF_SIZE = 1024

socket.setdefaulttimeout(0.5)

def hexDump(data):
    return [hex(x) for x in data]

class Connector(object):
    def __init__(self, serverIP, serverPort):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.settimeout(0.5)

        self.connection = self.sock.connect((serverIP, int(serverPort)))

    def __del__(self):
        self.sock.close()
        print "FIN"

    def write(self, data):
        self.sock.send(bytearray(data))

    def read(self):
        data = bytearray(self.sock.recv(BUF_SIZE))
        return data

#cn = Connector(config.serverIP, config.serverPort)
cn = Connector(SERVER, config.serverPort)

cn.write('hello word!!!')
print "RCV: ", cn.read()
del cn

"""

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.settimeout(0.5)

    conn = s.connect((SERVER, PORT))
except Exception as e:
    print e.message
    msg = "%s: %s -- Press Return to exit." % (e.errno, e.message)
    raw_input(msg)
    sys.exit(1)
print "TCP-client up and running."

while True:
    print "Connect request..."
    try:
        s.send(bytearray(CONNECT_REQ))
    except socket.error as e:
     #print str(e)
         if e.errno != 32: # 'Broken pipe' error isn't that dramatic.
              raise
    try:
      data = s.recv(1024)
      print "Response: ", hexDump(bytearray(data))
      print
    except Exception as e:
        print "Response: ", str(e)
    time.sleep(1.0)
s.close()

"""