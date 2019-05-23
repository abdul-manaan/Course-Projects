import socket 
import thread
import sys
import Queue
import os
import pickle
import threading
import time

friends = []
groups = []
adminOfGroups = []
groupsInfo = {}
friendsInfo = {} # friendsInfo[ name ] = ( Online/Offline , 'status', [Chat List] ) 
username = ""
ip = ""
port = 80
service = 'free' # busy
lock = threading.Lock()
 
messages = Queue.Queue()
videoExt = ["mp4", "webm","flv","mkv","vob","mng","wmv"]
picExt   = ["jpg","jpeg","gif","png","bmp","tiff"] 
audioExt = ["wav","aiff","mp3","au","3gp","raw","wv"]


def Main(): 
	if(len(sys.argv) == 4) :
		ip = sys.argv[1]
		port = int(sys.argv[2])
		username = sys.argv[3]
	else:
		print ("Argument List is too short or too Long.\nArgument List should be [ IP, Port, Name ]")
		exit(0)

	s = socket.socket() #socket.AF_INET, socket.SOCK_STREAM 
	s.connect((ip,port))

	s.send(username)
	status = s.recv(1024)
	print("=> " + status)
	if status == "Connected":	
		MyStatus = raw_input("Your Status......")
		s.send(MyStatus)

	if status == "Connected" or status == "You're Back!":

		thread.start_new_thread(recvMsg,(s,))
		while True:
			showRules(username)
			query = raw_input("=> ") 
			if query == ":q" :
				s.send(":q")
				break;
			elif query[:2] == ":p":
				uploadFile(s,query[2:])
			elif query[:2] == ':s':
				SendFileInGroup(s,query[2:])
			elif query[:2] == ':c' and len(query) == 3:
				n = int(query[2])
				if len(friends) > n : 
					message = raw_input("Enter Message to send: ")
					s.send(friends[n] + " : " + message)
				else:
					print ("Wrong Friend Number!")
			elif query[:2] == ':t' and len(query) == 3:
				n = int(query[2:])
				if len(groups) > n : 
					message = raw_input("Enter Message to send: ")
					s.send(".G-MSG-" + groups[n] + ":" +username+":"+ message)
				else:
					print ("Wrong Group Number!")
			else:
				takeAction(query,s,username)
			time.sleep(5)
			raw_input("\nPress the <ENTER> key to continue...")
			os.system('clear')

	s.close()

def updateStatus(server):
	stat = raw_input("Enter Your Status: ")
	server.send(".status" + stat)
	print "=> Status Successfully Updated!"
	return
	
def recvMsg(s):
	while True:
		try: 
			if service == 'free':
				lock.acquire()
				msg = s.recv(1024)
				if msg[:4] == "File" :
					lock.release()
					downloadFile(s,msg)
					lock.acquire()
					messages.put("=> " + msg)
				elif msg[:6] == ".Group":
					addYourselfInGroup(msg,s)
					messages.put("=> "+msg[7:])
				elif msg[:6] == ".G-MSG":
					messages.put("=> " + msg[7:])
				elif msg[:5] == '.Data':
					getData(msg[6:])
				elif msg[:9] == '.FStatus-':
					print '=> ' + msg[9:]
				elif msg[:7] == '.GInfo-':
					ShowGroupInfo(msg[7:])
				elif msg[:7] == '.FSTAT-':
					print '=> ' + msg[7:]
				else :
					messages.put("=> " + msg)
				lock.release()
		except:
			print ("server disconnected")
			break

def getData(msg):
	msg = msg.split('-')
	grps = msg[0]
	grps = grps.split(':')
	frnds = msg[1]
	frnds = frnds.split(':')
	adm = msg[1]
	adm = adm.split(':')
	for i in grps:
		if i != 'groups':
			groups.append(i)
	for i in frnds:
		if i != 'friends':
			friends.append(i)
	for i in adm:
		if i != 'admin':
			adminOfGroups.append(i) 		
		 
def CreateGroup(server,username):
	listOfMembers = []
	listOfMembers.append(username)
	name = raw_input( "=> Enter Group Name: ")
	groups.append(name)
	adminOfGroups.append(name)
	addMembers = raw_input("=> Do you want to add members? Y/N: ")

	if addMembers == "Y" or addMembers == "y":
		while True:
			member = raw_input("Enter friend name or (:q to exit): ")
			if member == ":q":
				break			
			elif member in friends:
				listOfMembers.append(member)
			else:
				print "No such Friend"

	elif addMembers != "N" or addMembers != "n" :
		print "Incorrect Input"
		return
	print name  + " Group Created Successfully"
	server.send(".Group:" + name + ":" + username)
	n  = str(len(listOfMembers)) 
	for ss in listOfMembers:
		n = n + ":"+ ss
	server.send(n)
	groupsInfo[name] = listOfMembers

	return

def addYourselfInGroup(message,server):
	message = message.split(':')
	gname = message[1]
	list = []
	admin = message[3]
	n = int(message[2])
	for i in range(n):
		list.append(message[i+4])



	if gname not in groupsInfo:	
		groupsInfo[gname] = list
		groups.append(gname)
	else:
		print "already a member"
	print "You added successfully in " + gname + " by " + admin
	return

def GroupMessage(server):
	name = ""	
	while True:
		name = raw_input("=> Enter name of group: ")
		if name == ":q":
			return	
		elif name in groups:
			break
		else:
			print "=> Error: No such group"
	msg = raw_input("Enter Message to send: ")
	server.send(".G-MSG-"+ name + ":" +username+": "+msg )
	return			 	

def AddGroupMember(server):
	name = ""
	groupName = ""
	while True:
		groupName = raw_input("=> Enter name of group: ")
		if groupName == ":q":
			return	
		elif groupName in groups:
			break
		else:
			print "=> Error: No such group"	
	while True:
		name = raw_input("=> Enter name of friend: ")
		if name == ":q":
			return		
		elif name in friends:
			break
		else:
			print "=> Error: No such friend"
	if name not in groupsInfo[groupName]:
		groupsInfo[groupName].append()
	
	server.send(".G-Edit:"+groupName+":add:"+name ) 
	return
	

def DeleteContact(server):
	name = ''
	while True:
		name = raw_input("=> Enter name of friend: ")
		if name in friends:
			break
		else:
			print "=> Error: No such friend"
	friends.remove(name)
	server.send('.RemFrnd' + name)
	return
			
def downloadFile(user,msg):
	try:
		service = 'busy'
		lock.acquire()
		name = msg.split(" : ")
		ext = name[1].split(".")
		if ext[1] in videoExt :
			path = "Videos" 
		elif ext[1] in picExt :
			path = "Images"
		elif ext[1] in audioExt:
			path = "Audio"	
		else:
			path = "Documents" 

		if not os.path.exists(path):
			os.makedirs(path)

		filename = path+"/"+name[1]
		while os.path.isfile(filename):
			filename = filename+'(1)'

		size = long(name[2])
		f = open(filename, 'wb')
		data = user.recv(1024)
		f.write(data)
		totalRecv = len(data)
		while totalRecv < size:
		    data = user.recv(1024)
		    totalRecv += len(data)
		    f.write(data)
		f.close()
		lock.release()
		service = 'free'
	except Exception as msg:
		print msg



def uploadFile(sock,n):
	m = int(n)
	if m >= len(friends):
		print "Incorrect Input!"
		return
	name = friends[m]
	filename = raw_input("Enter File name to send: ")

    	if os.path.isfile(filename):
		fileExt = filename.split('/')
		ext = fileExt[len(fileExt) -1 ] 
		size = os.path.getsize(filename)
		#print size
		sock.send("file:" + name + ":" + ext + ":" + str(size))
		f = open(filename, 'rb')
		bytesToSend = f.read(1024)
		sock.send(bytesToSend)
		while bytesToSend != "":
			bytesToSend = f.read(1024)
			sock.send(bytesToSend)
		f.close()
	else:
		sock.send("ERR ")

def SendFileInGroup(sock,n):
	m = int(n)
	if m >= len(groups):
		print "Incorrect Input!"
	gname = groups[m]
	filename = raw_input("Enter File name to send: ")

    	if os.path.isfile(filename):
		fileExt = filename.split('/')
		ext = fileExt[len(fileExt) -1 ] 
		size = os.path.getsize(filename)
		sock.send(".G-File:" + gname + ":" + ext + ":" + str(size))
		f = open(filename, 'rb')
		bytesToSend = f.read(1024)
		sock.send(bytesToSend)
		while bytesToSend != "":
			bytesToSend = f.read(1024)
			sock.send(bytesToSend)
			#print bytesToSend
		f.close()
		print "File sent successfully"
	else:
		sock.send("ERR ")


def takeAction(input,server,user):
	if input   == ':f' :
		print friends
	elif input == ':af' :
		frnd = raw_input("Enter Friend Name: ")
		friends.append(frnd)
		server.send(".AddFrnd" + frnd)
	elif input == ':fs':
		frnd = raw_input("Enter Friend Name: ")
		getStatus(frnd,server)
	elif input == ':r' :
                print ("You have %d new messages!" %( messages.qsize() ) )
		while not messages.empty():
    			print messages.get() 
	elif input == ':g' :
		print groups
	elif input == ':dc' :
		DeleteContact(server)
	elif input == ':gc' :
		CreateGroup(server,user)
	elif input == ':ag':
		AddGroupMember(server)
	elif input == ':us':
		updateStatus(server)
	elif input == ':ig':
		printGroupInfo(server)
	elif input == ':ma':
		makeAdmin(server)
	elif input == ':dg':
		removeGrpMem(server)
	elif input == ":lg":
		leftGroup(server,user)
	elif input == ":of":
		checkOnline(server)
	return True


def checkOnline(server):
	frnd = raw_input('Enter friend name: ')
	if frnd not in friends:
		print 'You have no such friend!'
		return	
	server.send('.FSTAT-'+ frnd)
	return True
def leftGroup(server,user):
	grp = raw_input("Enter Group name: ")
	if grp not in groups:
		print "No Such Group"
		return
	server.send(":G-Edit:"+grp+":left:"+user)
	groups.remove(grp)
	if grp in adminOfGroups:
		adminOfGroups.remove(grp)

def makeAdmin(server):
	grp = raw_input("Enter Group name: ")
	if grp not in groups:
		print "No Such Group"
		return
	
	mem = raw_input("Enter member name whom you want to make admin: ")
	server.send(":G-Edit:"+grp+":admin:"+mem)

def removeGrpMem(server):
	grp = raw_input("Enter Group name: ")
	if grp not in adminOfGroups:
		print "You are not admin of that group"
		return
	
	mem = raw_input("Enter member name whom you want to remove: ")

	server.send(":G-Edit:"+grp+":rem:"+mem)

				
def getStatus(frnd,server):
	if frnd not in friends:
		print 'You have no such friend!'
		return
	else:
		server.send('.FStatus-' + frnd)

def printGroupInfo(server):
	grp = raw_input("Enter group name: ")
	if grp not in groups:
		print 'No such Group!'	
		return
	if grp in adminOfGroups:
		print 'You are the admin of this group'
	server.send('.GInfo-'+grp)

def ShowGroupInfo(msg):
	msg = msg.split('-')
	adm = msg[0]
	mem = msg[1]
	adm =adm.split(':')
	mem = mem.split(':')
	for i in adm:
		if i == 'admins':
			print( "Following are group admins: ")
		else:
			print(i)
	for i in mem:
		if i == 'members':
			print ('=> Following are groups members')
		elif i not in adm:
			print(i)
	return
 		
def showRules(name):
	print 'Hey %s!\n' % name
	print 'Enter..'
	A = "':f'- To get your friend List"
	B = "':af'- To add a friend"
	C = "':fs'- To check the status of a friend"
	D = "':cN'- To send a message to a friend where N is friend number"
	E = "':pN'- To send a file to a friend where N is friend number"
	F = "':r'- To read incoming messages"
	G = "':g'- To get the names of groups you are in"
	H = "':tN'- To send a message in a group"
	I = "':sN'- To send a file to a group"
	J = "':gc'- To create a group"
	K = "':ig'- To get info of a Group"
	L = "':dc'- To delete a contact from Your contact list"
	M = "':ag'- To add member to a group"
	N = "':ma'- To make someone admin of a group"
	O = "':dg'- To remove someone from a group"
	P = "':us'- To update your current status"
	Q = "':lg'- To leave a group"
	R = "':of'- To check if a friend is online/offline "
	S = "':q'- To quit"
	T = "****************************************************************"
	print ('%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s' % (A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T))

if __name__=="__main__":
	Main()
