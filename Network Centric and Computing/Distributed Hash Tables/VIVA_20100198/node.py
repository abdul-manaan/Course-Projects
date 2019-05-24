# This example assumes MAX NUM of NODES IN A NETWORK can be 256 or 2 ^ 8
# IT means The routing table will be of size 8
"""
The routing Table

i      id + 2^i     Succ
0        id+1        
1        id+2
2        id+4
3         id+8
4         id+16
5         id+32
6         id+64
7         id+128


successor is server
predecessor is client
"""
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

myIPAddr = 'localhost'
myPortNum = int(sys.argv[1])



def main():
    try:
        MAXNODES = 128
        portNum = 0
        is_FirstNode = False
        if(len(sys.argv) == 2) :
            portNum = int(sys.argv[1])

        elif (len(sys.argv) == 3 and sys.argv[2] == "start"):
            portNum = int(sys.argv[1])
            is_FirstNode = True

        else:
            print "Error: Incorrect Command Line Arguments"
            print "The foramt is : \"python node.py PORTNUM [start]\""
            print "start is optional!"
            return
        myPortNum = portNum
        myID = hashIP(('localhost',portNum),MAXNODES)
        myInfo = peer('localhost',portNum,myID)
        myFinTable = HashFunction(MAXNODES,myInfo.id)
        myFinTable.createFingerTable()
        #myFinTable.printFingerTable()
        myInfo.ListenForNewConnections(myFinTable)
        NetworkMananger(myInfo,is_FirstNode,myFinTable)
        print "Initializing..."
        time.sleep(5)
        takeUserInput(myFinTable, myInfo)       
    except Exception as msg:
        print "Error :"
        print msg

    return


def takeUserInput(myFinTable,myInfo):
    commmand = ''
    print 'Enter the following commands for following purposes: '
    print 'Enter :q or shutdown to shutdown'
    print 'Enter :f to print FingerTable'
    print 'Enter :s to know who is your Successor'
    print 'Enter :p to know who is your Predecessor'
    print 'Enter \'GET FILENAME\' to get a specific file'
    print 'Enter \'PUT FILENAME\' to put a specific file'
    while commmand != 'shutdown':
        commmand = raw_input('=> ')
        if commmand == ':f':
            myFinTable.printFingerTable()
        elif commmand == ':s':
            if myFinTable.successors:
                print myFinTable.successors[0]
            else:
                print 'None'
        elif commmand == ':p':
            if myFinTable.predecessors:
                print myFinTable.predecessors[0]
            else:
                print 'None'
        elif commmand[:3] == "PUT":
            msg = commmand.split(" ")
            fileHashID = hash(msg[1])
            putFileToNode(myFinTable,myInfo,fileHashID,msg[1])
        else:
            print 'Wait'



def putFileToNode(myFinTable,myInfo,fileID,filename):
    print fileID
    resNodeID = fileID%myFinTable.maxpeers
    print resNodeID
    resNodeID =  myFinTable.findPeer(resNodeID)
    if resNodeID == myInfo.id:
        if fileID not in myFinTable.myStorage:
            myFinTable.myStorage[fileID] = filename
    else:
        ip,port = myFinTable.getpeer(resNodeID)
        msg = "PUT " + filename + " " + str(fileID) + " " + myIPAddr + " " + str(myPortNum)
        sendMsgToAPeer(msg,ip,port)


def NetworkMananger(myInfo,is_FirstNode,myFinTable):
    thread.start_new_thread(NetworkMaintainer, (myInfo,is_FirstNode,myFinTable))

#will make connection with successor
def NetworkMaintainer(myInfo,is_FirstNode,myFinTable):
    try:
        if not is_FirstNode:
            myInfo.sock = myInfo.ConnectToNetwork()
            myInfo.sock.send("NEW %s %s %s"%(str(myInfo.id) , myInfo.ip, str(myPortNum)) )
                
        while True:
            if myInfo.sock is None:
                continue
            data = myInfo.sock.recv(1024)
            if not data:
                #myInfo.sock.shutdown()
                myInfo.sock.close()
                myInfo.sock = None
                break
            print "successor says : " + data
            if data == "PING":
                myInfo.sock.send("PING")
            elif data[:10] == "PLSCONNECT":
                data = data.split(" ")
                cip = data[1]
                cport = int(data[2])  
                myInfo.sock.close()
                myInfo.sock = socket.socket()
                myInfo.sock.connect((cip,cport))
                myInfo.sock.send("NEW %s %s %s"%(str(myInfo.id) , myInfo.ip, str(myPortNum)) )

            elif data[:11] == "PREDECESSOR": #That means I am the predecessor for that node "He is my Successor"
                data = data.split(" ")
                sid = data[1]
                sip = data[2]
                sport = data[3]
                if myInfo.mySuccessor:
                    myInfo.mySuccessor.send("MYNEWSUCCESSOR %s %s %s"%(sid, sip,sport))
                if myInfo.myPredecessor:
                    if myFinTable.successors and myFinTable.predecessors[0] != myFinTable.successors[0]:
                        myInfo.myPredecessor.send("MYNEWSUCCESSOR %s %s %s"%(sid, sip,sport))

                myInfo.mySuccessor = myInfo.sock

                myFinTable.updateSuccessor(int(sid),0)
                myFinTable.updateFingerTable(int(sid))
                myFinTable.addpeer(int(sid),sip,int(sport))

            elif data[:9] == "SUCCESSOR":
                data = data.split(" ")
                # sid = data[1]
                # sip = data[2]
                # sport = data[3]
                # if myInfo.myPredecessor:
                #     myInfo.myPredecessor.send("MYNEWPREDECESSOR %s %s %s" %(sid,sip,sport))
                # if myInfo.mySuccessor:
                #     if myFinTable.predecessors[0] != myFinTable.successors[0]:
                #         myInfo.mySuccessor.send("MYNEWPREDECESSOR %s %s %s" %(sid,sip,sport))
                # myInfo.myPredecessor = myInfo.sock
                # myFinTable.updatePredecessor(int(sid),0)
                # myFinTable.updateFingerTable(int(sid))
                # myFinTable.addpeer(int(sid),sip,int(sport))

            elif data[:16] == "MYNEWPREDECESSOR":
                data = data.split(" ")
                cid = data[1]
                cip = data[2]
                cport = int(data[3])  
                if myInfo.sock:
                    myInfo.sock.close()
                myInfo.sock = socket.socket()
                myInfo.sock.connect((cip,cport))
                myInfo.sock.send("SUCCESSOR %s %s %s"%(str(myInfo.id) , myInfo.ip, str(myPortNum)) )
                if myInfo.myPredecessor:
                    myInfo.myPredecessor.send("MYNEWSUCCESSOR %s %s %s" %(cid,cip,str(cport))) 
                
                myInfo.mySuccessor = myInfo.sock
                myFinTable.updateSuccessor(int(cid),0)
                myFinTable.updateFingerTable(int(cid))
                myFinTable.addpeer(int(cid),cip,int(cport))            

            elif data[:14] == "MYNEWSUCCESSOR":
                data = data.split(" ")
                cid = data[1]
                cip = data[2]
                cport = int(data[3])  
                myFinTable.addpeer(int(cid),cip,cport)
                myFinTable.updateFingerTable(int(cid))

            elif data[:3] == "GET":
                print "data"
            elif data[:3] == "PUT":
                print data
            elif data[:4] == "SHUT":
                if myInfo.sock is myInfo.myPredecessor:
                    myInfo.myPredecessor = None
                elif myInfo.sock is myInfo.mySuccessor:
                    myInfo.mySuccessor = None

    except Exception as msg:
        print "Error" , msg
        myInfo.sock.close()
        myInfo.sock = None



class peer:
    def __init__(self,ip,port,idNum):
        self.sock = None
        self.ip = ip
        self.port = port
        self.idNum = idNum
        self.s = socket.socket() #socket.AF_INET, socket.SOCK_STREAM 
        self.s.bind((self.ip,self.port))
        self.connector = socket.socket()
        self.mySuccessor = None
        self.myPredecessor = None

    def setIP(self):
        try:
            self.ip = raw_input("Enter you IP ")
            self.port = int(raw_input("Enter you Port "))
            return True
        except Exception as _:
            return False

    def ListenForNewConnections(self,me):
        print "Strating Listening for new Connection .... "
        thread.start_new_thread(listening, (self.s,self,me))     
        return True

    def ConnectToNetwork(self):
        print "Connecting to a Node .. "
        (OtIP,OthPort) = self.getOtherPeerIP()
        return connectToPeer(self.connector, OtIP, OthPort)

    # def getPeerFingerTable(self, str):
    #     #Finger Table will be stored as 
    #     #   "FingerTable PeerID#-1-2-3-4-5-6-7-8"
    #     str = str.split(" ")
    #     str = str[1].split("-")
    #     oPeerID = int(str[1])
    #     oFingerTable = {}

    #     for i in range(8):
    #         x = oPeerID + 2 ** i
    #         oFingerTable[x] = int(str[1+i])

    #     return oFingerTable

    def getOtherPeerIP(self):
        try:
            otherPeerIP = raw_input("Enter peer IP: ")
            otherPeerPort = int(raw_input("Enter Port No. of Peer: "))
            return (otherPeerIP,otherPeerPort)
        except Exception as _:
            return self.getOtherPeerIP()

def pingPredecessor(sock):
    try:
        pingMsg = "PING"
        sock.timeout(2)
        sock.send(pingMsg)
        sock.timeout(2)
        msg = sock.recv()
        sock.timeout(None)
        if msg != "PING":
            print "Error"
            print msg
            updatePredecessor()
        return True
    except Exception as _:
        updatePredecessor()



def listening(s,sel,me):
    s.listen(100) # max number of connections
    while True:
        client, addr = s.accept()
        print "Starting a new thread for new Peer"
        thread.start_new_thread(peerMsgReceiver,(client,addr,me,sel))



def peerMsgReceiver(client,addr,me,myInfo):
    try:
        while True:
            msg = client.recv(1024)
            if not msg:
                client.close()
                break
            print "Predecessor says: " ,addr, msg
            if msg[:3] == "NEW": #msg = NEW ID IP PORT
                newPeer(msg, client, addr, me,myInfo)
            elif msg[:3] == "GET": #msg = GET ID fileID 
                getFile()
            elif msg[:3] == "PUT": #msg = PUT Filename FILEID peerIP peerPort
                saveFile(me,myInfo,msg)
            elif msg[:15] == "READYTOSAVEFILE":#msg = READYTOSAVEFILE filename fileID
                sendFileToNode(client, msg)
                client.close()
                break
            elif msg[:4] == "SHUT":# msg = SHUT ID [files]
                closePeer()
            elif msg[:11] == "PREDECESSOR":
                msg = msg.split(" ")
                cid = msg[1]
                cip = msg[2]
                cport = msg[3]
                if myInfo.mySuccessor and myInfo.mySuccessor != client:
                    myInfo.mySuccessor.send("MYNEWSUCCESSOR %s %s %s" % (cid,cip,cport))
                    time.sleep(1)
                if myInfo.myPredecessor:
                    if me.predecessors[0] != me.successors[0]:
                        myInfo.myPredecessor.send("MYNEWSUCCESSOR %s %s %s" % (cid,cip,cport))
                        time.sleep(1)
                print "updating...."
                myInfo.sock = updateSuccessorPointer(cip,int(cport),me,myInfo)
                me.updateSuccessor(int(cid),0)
                me.updateFingerTable(int(cid))
                me.addpeer(int(cid),cip,int(cport))
                print "updated successor"
                break
            elif msg[:9] == "SUCCESSOR":
                msg = msg.split(" ")
                cid = msg[1]
                cip = msg[2]
                cport = msg[3]
                if myInfo.myPredecessor:
                    myInfo.myPredecessor.send("MYNEWPREDECESSOR %s %s %s" % (cid,cip,cport))
                    time.sleep(1)
                if myInfo.mySuccessor:
                    if (me.predecessors) and me.predecessors[0] != me.successors[0]:
                        myInfo.mySuccessor.send("MYNEWPREDECESSOR %s %s %s" % (cid,cip,cport))
                        time.sleep(1)
                me.updatePredecessor(int(cid),0)
                myInfo.myPredecessor = client
                me.updateFingerTable(int(cid))
                me.addpeer(int(cid),cip,int(cport))

            elif msg[:16] == 'MYNEWPREDECESSOR':
                msg = msg.split(" ")
                cid = msg[1]
                cip = msg[2]
                cport = msg[3]
                me.updateFingerTable(int(cid))
                me.addpeer(int(cid),cip,int(cport))

            elif msg[:14] == 'MYNEWSUCCESSOR':
                msg = msg.split(" ")
                cid = msg[1]
                cip = msg[2]
                cport = msg[3]
                # if myInfo.myPredecessor:
                #     myInfo.myPredecessor.send("MYNEWPREDECESSOR %s %s %s" % (cid,cip,cport))
                #     time.sleep(1)
                myInfo.myPredecessor = None
                informPredecessor(cip,int(cport),me)
                me.updateFingerTable(int(cid))
                me.addpeer(int(cid),cip,int(cport))

            elif msg[:4] == "PING":#msg = PING
                client.send("PING")
    except Exception as msg:
        client.close()



def informPredecessor(ip,port,myData):
    s = socket.socket()
    s.connect((ip,port))
    s.send("PREDECESSOR %s %s %s"%(str(myData.ID),myIPAddr,str(myPortNum) ))
    time.sleep(1)
    s.close()


def getFile():
    return

def saveFile(myFinTable,myInfo,msg): #PUT filename fileID peerIP peerPort
    try:
        msgBreak = msg.split(" ")
        print "here"
        fileID = int(msgBreak[2])
        fileNodeID = fileID%myFinTable.maxpeers

        resNodeID = myFinTable.findPeer(fileNodeID)
        print "responsiblePeer" ,resNodeID
        if resNodeID == myInfo.id or msg in myFinTable.forwardedRequestPUT:
            if fileID not in  myFinTable.myStorage:
                myFinTable.myStorage[fileID] = msgBreak[1]
                print "here"
                saveFileToStorage(myInfo,myFinTable,msg)
                if msg in myFinTable.forwardedRequestPUT:
                    myFinTable.forwardedRequestPUT.remove(msg)
        else:
            (ip,port) = myFinTable.getpeer(resNodeID)
            sendMsgToAPeer(msg,ip,port)
            myFinTable.forwardedRequestPUT += [msg] 
            time.sleep(1)
        return
    except Exception as err:
        print err


def sendMsgToAPeer(msg,ip,port):
    s = socket.socket()
    s.connect((ip,port))
    s.send(msg)
    time.sleep(2)
    s.close()

def saveFileToStorage(myInfo,myFinTable,msg):
    msg = msg.split(" ")
    nodeIP = msg[3]
    nodePort = int(msg[4])
    fileID = msg[2]
    fileName = msg[1]
    sock = socket.socket()
    sock.connect((nodeIP,nodePort))
    sock.send("READYTOSAVEFILE %s %s"%(fileName,fileID))

    data = sock.recv(1024)
    if data[:4] != "FILE":
        return
    name = data.split(" ")
    path = str(myInfo.id)
    if not os.path.exists(path):
            os.makedirs(path)

    filename = path+"/"+fileName
    while os.path.isfile(filename):
        filename = filename+'(1)'

    size = long(name[1])
    f = open(filename, 'wb')
    totalRecv = len(name[0]) +len(name[1])+2
    data = data[totalRecv:]
    totalRecv = len(data)
    f.write(data)
    while totalRecv < size:
        data = sock.recv(1024)
        totalRecv += len(data)
        f.write(data)
    f.close()
    sock.close()


def sendFileToNode(sock,msg):
    msg = msg.split(" ")
    filename = msg[1]
    fileID = msg[2]

    if os.path.isfile(filename):
        size = os.path.getsize(filename)
        #print size
        msgToSend = "FILE " + str(size)
        f = open(filename, 'rb')
        bytesToSend = f.read()
        bytesToSend = msgToSend +" "+ bytesToSend
        sock.send(bytesToSend)
        time.sleep(1)
    
    f.close()
def closePeer():
    return

 

def newPeer(msg, client,addr, myData,myInfo):
    print "A new peer has arrived!"
    #client.send ".msg Connected to Node " + self.id
    #client.send ".hashtable"
    msg = msg.split(" ")
    cid = int(msg[1])
    cip = msg[2]
    cport = int(msg[3])
    responsiblePeer = myData.findPeer(cid)
    if (myData.predecessors) == 0:
        #client.send("SUCCESSOR %s %s %s"%(str(myData.ID),myIPAddr,myPortNum ))
        if myInfo.mySuccessor:
            myInfo.mySuccessor.send("MYNEWSUCCESSOR %s %s %s" % (str(cid),cip,str(cport)))
            time.sleep(1)

        if myInfo.myPredecessor:
            myInfo.myPredecessor.send("MYNEWPREDECESSOR %s %s %s" % (str(cid),cip,str(cport)))
            time.sleep(1)
        client.send("PREDECESSOR %s %s %s"%(str(myData.ID),myIPAddr,str(myPortNum) ))
        time.sleep(1)
        updateSuccessorPointer(cip,cport,myData,myInfo)
        myInfo.myPredecessor = client
        myData.updatePredecessor(cid,0)
        myData.updateSuccessor(cid,0)
        myData.updateFingerTable(cid)
        myData.addpeer(cid,cip,cport)

    elif isSuccessor(myData.ID, cid, myData.successors[0]):
        #client.send("SUCCESSOR %s %s %s"%(str(myData.ID),myIPAddr,myPortNum ))
        if myInfo.mySuccessor:
            myInfo.mySuccessor.send("MYNEWSUCCESSOR %s %s %s" % (str(cid),cip,str(cport)))
            time.sleep(1)
        if myInfo.myPredecessor:
            myInfo.myPredecessor.send("MYNEWSUCCESSOR %s %s %s" % (str(cid),cip,str(cport)))
            time.sleep(1)
        myData.updateSuccessor(cid,0)
        myData.updateFingerTable(cid)
        myData.addpeer(cid,cip,cport)
        updateSuccessorPointer(cip,cport,myData,myInfo)

    elif isPredecessor(myData.ID, cid, myData.predecessors[0]):
        if myInfo.mySuccessor:
            myInfo.mySuccessor.send("MYNEWPREDECESSOR %s %s %s" % (str(cid),cip,str(cport)))
            time.sleep(1)
        if myInfo.myPredecessor:
            myInfo.myPredecessor.send("MYNEWPREDECESSOR %s %s %s" % (str(cid),cip,str(cport)))
            time.sleep(1)
        myInfo.myPredecessor = client
        client.send("PREDECESSOR %s %s %s"%(str(myData.ID),myIPAddr,myPortNum ))
        time.sleep(1)
        myData.updatePredecessor(cid,0)
        myData.updateFingerTable(cid)
        myData.addpeer(cid,cip,cport)

    elif myData.ID != myData.getpeer(responsiblePeer):
        ip_toSend, port_toSend = myData.getpeer(responsiblePeer)
        client.send("PLSCONNECT %s %s" %(ip_toSend,str(port_toSend)))
        time.sleep(1)
    else:
        responsiblePeer = myData.successors[0]
        ip_toSend, port_toSend = myData.getpeer(responsiblePeer)
        client.send("PLSCONNECT %s %s" %(ip_toSend,str(port_toSend)))
        time.sleep(1)



########################################################################################################
######################    Connect to a Peer  and Enter yourself in the Network     #####################
########################################################################################################
def connectToPeer(s,otherIP,otherPort):
    s.connect((otherIP,otherPort))
    return s


def updateSuccessorPointer(ip,port,myData,myInfo):
    if myInfo.sock:
        myInfo.sock.close()

    sock = socket.socket()
    sock.connect((ip,port))
    sock.send("SUCCESSOR %s %s %s"%(str(myData.ID), myIPAddr,str(myPortNum)))
    time.sleep(2)
    myInfo.mySuccessor = sock
    myInfo.sock = sock
    return sock

# def getFingerTable(s):
#     s.send("Send me FingerTable")
#     str1 = s.recv(1024)
#     return str1

def getSuccessor(s):
    s.send("Send me YSuccessor")
    str2 = s.recv(1024)
    return str2

def getPredecessor(s):
    s.send("Send me YPredecessor")
    str3 = s.recv(1024)
    return str3


def getmyFilesData(s):
    s.send("Send me MFilesToStore")
    str4 = s.recv(1024)
    return str4 


########################################################################################################
########################################################################################################

def isSuccessor(a , b, c): # a -> myID, b -> clientID, c-> myCurrent Successor
    if b > a and b < c and c > b:
        return True
    elif b > a and b > c and c < a:
        return True
    elif b < a and b < c and c < a:
        return True
    else:
        return False

def isPredecessor(a , b, c): # a -> myID, b -> clientID, c-> myCurrent Predecessor
    if b < a and b > c and c < b:
        return True
    elif b < a and b < c and c > a:
        return True
    elif b > a and b > c and c > a:
        return True
    return False



########################################################################################################
######################    Finger Table Manager and SUCCESSOR/PREDECESSOR store     #####################
########################################################################################################

def hashIP(PeerAddr,MAXNODES):
    return hash(PeerAddr)%MAXNODES


class HashFunction:
    def __init__ (self, MaxNumOfPeers, myID):
        self.maxpeers = MaxNumOfPeers
        self.peers = {}  # (host, port)
        self.ID = myID
        self.hashSize =  int(math.log(self.maxpeers,2))    #fixed Hash Table Size
        self.fingertable = {}   # i = (id+ 2^i , successor)
        self.successors = []    # List of PeerIDs who are my successors
        self.predecessors = []  # List of PeerIDS who are my predecessors
        self.peerSockets = {}
        self.myStorage = {}
        self.forwardedRequestPUT = []

    def addpeer( self, peerid, host, port ):
        if peerid not in self.peers and len(self.peers) < self.maxpeers:
            self.peers[ peerid ] = (host, int(port))
            return True
        
        return False

    def getpeer( self, peerid ):
        #assert peerid in self.peers
        return self.peers[ peerid ]

    def removepeer( self, peerid ):
        if peerid in self.peers:
               del self.peers[ peerid ]

    def createFingerTable(self):
        for i in range(self.hashSize):
            x = (self.ID + 2 ** i) % self.maxpeers
            self.fingertable[i] = (x , self.ID)


    def updatePredecessor(self, peerID, n):
        #update nth predecesoor ---- 
        if peerID not in self.predecessors:
            if len(self.predecessors) == 0:
                self.predecessors.append(peerID)
            elif len(self.predecessors) +1 == n:
                self.predecessors.append(peerID)
            elif len(self.predecessors) >= n:
                self.predecessors[n] = peerID

    def updateSuccessor(self, peerID, n):
        if self.successors == 0:
            self.successors.append(peerID)
        elif len(self.successors) +1 == n:
            self.successors.append(peerID)
        elif len(self.successors) >= n:
            self.successors[n] = peerID


    def updateFingerTable(self, peerID):
        for i in range(self.hashSize):
            start = self.fingertable[i][0]
            end = self.fingertable[(i+1)%self.hashSize][0]
            prevID = self.fingertable[i][1]
            # distOfPrevID = self.fingertable[i][1] -self.fingertable[i][0]  
            # distOfNewID  = peerID - self.fingertable[i][0]
            # if(distOfNewID == 0):
            #     self.fingertable[i] = (self.fingertable[i][0], peerID)
            # elif (distOfNewID > -1 and distOfPrevID > -1) and (distOfNewID < distOfPrevID):
            #     self.fingertable[i] = (self.fingertable[i][0], peerID)
            # elif  (distOfNewID < 0 and distOfPrevID < 0) and (distOfNewID  > distOfPrevID):
            #     self.fingertable[i] = (self.fingertable[i][0], peerID)
            # if (start<end) and (peerID in range(start,end) and  prevID not in range(start,end)) or (peerID in range(start,end) and  prevID not in range(start,end) and peerID == min(peerID,prevID)):
            #     self.fingertable[i] = (self.fingertable[i][0], peerID)
            # elif (start > end) and ((peerID >= start or peerID < end ) and (prevID < start or prevID >= end)) or ((peerID >= start or peerID < end ) and (prevID >= start or prevID < end) and peerID == min(peerID,prevID)): 
            #     self.fingertable[i] = (self.fingertable[i][0], peerID)
            if is_between(peerID,start,end):
                if is_between(prevID,start,end):
                    if peerID == max(peerID,prevID) :
                        self.fingertable[i] = (self.fingertable[i][0], peerID)
                else:
                    self.fingertable[i] = (self.fingertable[i][0], peerID)
    def printFingerTable(self):
        print "Finger Table of Peer ID# ", self.ID
        for x in self.fingertable:
            print self.fingertable[x]

    def findPeer(self,idNum):
        responsiblePeer = -1
        for i in range(self.hashSize):
            start = min(self.fingertable[i][0], self.fingertable[i][1])
            end = self.fingertable[(i+1)%self.hashSize][0]
            if (start <= end) and (idNum in range(start,end)) and self.fingertable[i][1] != self.ID:
                responsiblePeer =  self.fingertable[i][1]
            elif (start > end) and (idNum >= start  or idNum < end) and self.fingertable[i][1] != self.ID:
                responsiblePeer =  self.fingertable[i][1]
        if responsiblePeer == -1:
            responsiblePeer = self.ID
        return responsiblePeer

def is_between(idNum,start,end):
    if idNum == start:
        return True
    elif start <= end :
        return start in range(id,end)
    return (idNum <= start) or (idNum < end)
#####################################################################self.fingertable[i][1] != self.ID###################################
########################################################################################################

        



main()
