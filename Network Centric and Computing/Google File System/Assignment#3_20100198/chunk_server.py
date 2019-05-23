import Queue
import os
import pickle
import thread
import time
import hashlib
import random
import socket
import sys
import math





def main():
    try:
        No_Of_Chunk_Servers = 0
        ip = 'localhost'
        portNum = 0
        if(len(sys.argv) == 2) :
            portNum = int(sys.argv[1])
        else:
            print "Error: Incorrect Command Line Arguments"
            print "The foramt is : \"python chunk_server.py PORTNUM\""
            return

        print "Server Started....."
        myData  = MyData(ip,portNum)
        sock = socket.socket()
        controllerIP = raw_input("=> Enter Controller IP: ")
        controllerPort = input("=> Enter Controller Port: ")
        sock.connect((controllerIP,controllerPort))
        thread.start_new_thread(listening,(sock,myData))
        while True:
            msg = raw_input()
            print msg
    except Exception as msg:
        print "Error :"
        print msg

    return


class MyData:

    def __init__ (self,ip,port):
        self.ip = ip
        self.Port = port

def listening(sock,myData):
    print "Started Listening For Controller Requests"
    msgToSend = 'SERVER' + ' ' + myData.ip + ' ' + str(myData.Port) 
    sock.send(msgToSend)
    while True:
        msg = sock.recv(1024)
        if not msg:
            break
        print msg




if __name__== "__main__":
    main()