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
            print "The foramt is : \"python node.py PORTNUM\""
            return

        print "Controller Started....."
        serverObj = ChunkServers()
        thread.start_new_thread(listening,(ip,portNum,serverObj))
        while True:
            msg = raw_input()
            print msg
    except Exception as msg:
        print "Error :"
        print msg

    return


def listening(ip,port,serverObj):
    print "Started Listening For Connections"
    s = socket.socket()
    s.bind((ip,port))
    s.listen(100) # max number of connections
    while True:
        client, addr = s.accept()
        print "A Client/Server Connected..."
        thread.start_new_thread(NewConnection,(client,addr,serverObj))

def NewConnection(client,address,serverObj):
    message = client.recv(1024)
    if message[:6] == 'CLIENT':
        print "A CLIENT Connected...."
        thread.start_new_thread(chatWithClient,(client,serverObj))
    elif message[:6] == 'SERVER':
        print 'A Server Connected....'
        message = message.split(' ')
        ip = message[1]
        port = int(message[2])
        serverObj.addAServer(ip,port,client)
    else:
        print "An Unknown Connection was Rejected......"

def chatWithClient(client,serverObj):
    client.send('Ready To Talk....')
    while True:
        msg = client.recv(1024)
        if not msg:
            break
        elif msg[:4] == 'FILE':
            getFileFromClient(client,msg,serverObj)

def getFileFromClient(sock,message,serverObj):
    print "Getting a File from a Client"
    msg = message.split(" ")
    fileName = msg[1]
    size = long(msg[2])

    f = open('temporary', 'wb')
    totalRecv = len(msg[0]) +len(msg[1]) + len(msg[2])+3
    data = message[totalRecv:]
    totalRecv = len(data)
    f.write(data)
    while totalRecv < size:
        data = sock.recv(1024)
        totalRecv += len(data)
        f.write(data)
    f.close()
    print fileName, " Received of Size", size

class ChunkServers:
    def __init__ (self):
        self.totalServers = 0
        self.servers = []

    def getNoOfServers(self):
        return self.totalServers

    def addAServer(self,ip,port,sock):
        self.servers.append({'ip':ip,'port': port, 'sock':sock})
        self.totalServers += 1

    def getSever(self,id):
        if id < len(self.servers):
            return self.servers[id]




if __name__== "__main__":
    main()