import socket 
import thread
import sys
import os
import pickle
import threading


names     = []  #Name of every user at WhatsApp
online    = []  
clntList  = []   # stores port num of those users who are online
Groups    = []   # names of all groups
GroupDetails = {}  # [admins] : [ALL Members]
messagesInQueue = {} # user : [All messages]
filesInQueue = {} # sender :filename :receiver
statusOfUsers = {} # name : status
UsersDetails = {} # UserDeatails [ name ] = (groups,friends)
videoExt = ["mp4", "webm","flv","mkv","vob","mng","wmv"]
picExt   = ["jpg","jpeg","gif","png","bmp","tiff"] 
audioExt = ["wav","aiff","mp3","au","3gp","raw","wv"]



def getNames(client):
	user = client.recv(1024)
        	
	if user in online:
		client.send("User Already Connected.")
		return False , user
			
	if user in names:
		client.send("You're Back!") 
		online.append(user)
		clntList.append(client)
		SendPreviousMsgs()
		SendUserData(user)
		return True , user

	names.append(user)
	online.append(user)
	UsersDetails[user] = ([],[])
	clntList.append(client)
	client.send("Connected")
	#client.send("Your status... ")
	status = client.recv(1024)
	print status
	statusOfUsers[user] = status
	SendPreviousMsgs()
	return True , user


def SendUserData(user):
	try:
		msg = ".Data-groups"
		grp,frnd = UsersDetails[user]
		for i in grp:
			msg = msg + ":" + i
		msg += "-friends"
		for i in frnd:
			msg = msg + ":" +i
		msg = msg + "-admin"
		for i in GroupDetails:
			if user in GroupDetails[i][0]:
				msg += ":" + i
		if user in online:
			n = online.index(user)
			client = clntList[n]
			client.send(msg)
	except Exception as msg:
		print msg	
	 	
def SendPreviousMsgs():
	try:
		for i in messagesInQueue:
			if i in online:
				n = online.index(i)
				msgs = messagesInQueue[i]
				client = clntList[n]
				for i in msgs:
					print i
					client.send(i)
		for i in filesInQueue:
			if i in online:
				a,b,c = filesInQueue[i] 
				sendFile(a,b,c)
	except Exception as msg:
		print msg


def updateFriendList(user,frnd):
	try:
		UsersDetails[user][1].append(frnd)
	except Exception as msg:
		print msg

def UpdateStatus(status,user):
	try:
		#status = status.split(':')
		statusOfUsers[user] = status#[status]
		notifyStatusUpdate(user)
		return

	except Exception as msg:
		print msg



def notifyStatusUpdate(user):
	msg = "updated his status to\n" + statusOfUsers[user]
	st,friends = UsersDetails[user]
	for i in friends:
		if i in online:
			msg = i + ' : ' + msg
			forwardMsg(msg, user)
		elif i in messagesInQueue:
			messagesInQueue[i].append(user + ' ' + msg)
		else :
			messagesInQueue[i] = [user + ' ' + msg]


def func(client, addr):

	x , user = getNames(client)
	if x:
		print "connected to:  ", addr	

	while x:
		try:

			string = client.recv(1024)
			if string[:4] == "file" : 

				getFile(string,user,client)

			elif string[:6] == ".Group":
				CreateAGroup(string,client)		
			elif string[:6] == ".G-MSG" :
				SendGroupMessage(string)
			elif string[:7] == ".G-File":

				GetFileInGroup(string,user,client)

			elif string[:7] == '.status':
				UpdateStatus(string[7:],user)	
			elif string[:8] == '.AddFrnd':
				if string[8:] in names:
					UsersDetails[user][1].append(string[8:])
			elif string[:8] == '.RemFrnd':
				if string[8:] in names:
					if string[8:] in UsersDetails[user][1]:
						UsersDetails[user][1].remove(string[8:])						
			elif string[:7] == ":G-Edit":
				EditGroup(string[8:],user)
			elif string[:9] == '.FStatus-':
				client.send('.FStatus-' + statusOfUsers[string[9:]]) 
			elif string[:7] == '.FSTAT-':	
				if string[7:] in online:
					client.send('.FSTAT-Your Friend is online')
				else:
					 client.send('.FSTAT-Your Friend is offline')
			elif string[:7] == '.GInfo-':
				sendGInfo(string[7:],client) 
			elif string != ":q" :
				print string				
				forwardMsg(string,user)

			else:
				break

		except Exception as msg:
			print msg		
			break
	
	print "disconnected from:  ", addr	
	online.remove(user)
	clntList.remove(client)	
	client.close()

def sendGInfo(grp, client):
	adm,mem = GroupDetails[grp]
	info = '.GInfo-admins'
	for i in adm:
		info = info + ':' + i
	info += '-members'
	for i in mem:
		info = info + ':' +i
	client.send(info)	
	return			
def forwardMsg(msg,user):		
	smsg = msg.split(' : ')
	n = len(online)
	#print (smsg[0] + smsg[1])
	for i in range(n): 
		if online[i] == smsg[0]:
			client = clntList[i]
			client.send(user + " : " + smsg[1])
			print smsg[1]
			return
	if smsg[0] in names:
		msgToSend = (smsg[0], smsg[1])
		if smsg[0] in messagesInQueue:
			messagesInQueue[smsg[0]].append(user + " : " + smsg[1])
		else :
			messagesInQueue[smsg[0]] = [user + " : " + smsg[1]]


def CreateAGroup(msg,sender):
	try:
		msg = msg[7:]
		msg = msg.split(':')

		members = []
		print members
		n =sender.recv(1024)
		n = n.split(':')
		for i in range(int(n[0])):
			members.append(n[i+1])
		
		Groups.append(msg[0])
		print members
		for i in members:
			UsersDetails[i][0].append(msg[0])
		data = ([msg[1]],members)
		GroupDetails[ msg[0] ] = data
		message = ".G-MSG-" + msg[0] + ":" + msg[1] + ": created the group " + msg[0]
		AddGroupMembers(msg[0])
		print "added group members"
		SendGroupMessage(message)
		print "messages sent"
		return	
	except Exception as msg:
		print msg	 

def EditGroup(msg,user):
	# msg = (groupName+":add:"+name ) )
	msg = msg.split(':')
	gname = msg[0]
	command = msg[1]
	name = msg[2]
	if command == 'add':
		adm,mem = GroupDetails[gname]
		if name in names and name not in mem:
			GroupDetails[gname][1].append(name)
			UsersDetails[name][0].append(gname)
			info = ".Group:"+ gname+":"
			adm,mem = GroupDetails[gname]			
			m = len(mem)
			info  =info +  str(m) +":"+user

			for i in members:
				info = info + ":"+ i			
			if name in online:			
				client = clntList[online.index(name)]
				client.send(info)
			else:
				
				if name in messagesInQueue:
					messagesInQueue[name].append(info)
				else :
					messagesInQueue[name] = [info]	
			info = ".G-ADM:"+gname+":"+name	
	elif command == 'del':
		adm,mem = GroupDetails[gname]
		if name in mem:
			GroupDetails[gname][1].remove(name)
			UsersDetails[name][0].remove(gname)
			info = ".G-Rem:"+ gname+":"+name

			if name in online:			
				client = clntList[online.index(name)]
				client.send(info)
			else:
				
				if name in messagesInQueue:
					messagesInQueue[name].append(info)
				else :
					messagesInQueue[name] = [info]	
			info = ".G-RMM:"+gname+":"+name	
	elif command == 'admin':
		adm,mem = GroupDetails[gname]
		if name in mem:
			info = ".G-Admin:"+ gname
			GroupDetails[gname][0].append(name)

			if name in online:			
				client = clntList[online.index(name)]
				client.send(info)
			else:
				
				if name in messagesInQueue:
					messagesInQueue[name].append(info)
				else :
					messagesInQueue[name] = [info]	
		return
	elif command == 'left':
		adm,mem = GroupDetails[gname]
		if name in adm:
			GroupDetails[gname][0].remove(name)
		if name in mem:
			GroupDetails[gname][1].remove(name)
			UsersDetails[name][0].remove(gname)
			info = ".G-Lef:"+gname+":"+name	

	adm,mem = GroupDetails[gname]
	n = len(online)
	for i in range(n): 
		if online[i] in mem:
			client = clntList[i]
			client.send(info)

	if i in mem and i not in online:
		if i in messagesInQueue:
			messagesInQueue[i].append(info)
		else :
			messagesInQueue[i] = [info]
		

#//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
def AddGroupMembers(gname):
	try:
		info = GroupDetails[gname]
		members = info[1]
		admin = info[0]
		n = len(online)
		for i in range(n): 
			if online[i] in members and online[i] not in admin:
				client = clntList[i]
				info = ".Group:"+ gname+":"
				m = len(members)
				info  =info +  str(m) + ":"+admin[0]
				for i in members:
					info = info + ":"+ i
				client.send(info)


		print "ADD GROUP MEMBER RUN SUCCESSFULLY"		
		return	
	except Exception as msg:
		print msg
	
def SendGroupMessage(message):
	try:

		message = message[7:]
		message = message.split(":") 	
		name = message[0]
		info = GroupDetails[ name ]
		sender = message[1]
		members = info[1]
		message = message[1] + " in " + name + " : " + message[2]
		n = len(online)
		for i in range(n): 
			if online[i] in members and online[i] != sender:
				client = clntList[i]
				client.send(".G-MSG:"+ message)

		if i in members and i not in online:
			if i in messagesInQueue:
				messagesInQueue[i].append(".G-MSG:"+ message)
			else :
				messagesInQueue[i] = [".G-MSG:"+ message]		
		print "Messages sent"
		return
	except Exception as msg:
		print msg
			
def getFile(msg,sendername,sender):
	names = msg.split(':')
	size = long(names[3])
	fname = sendername + "-" + names[1]+"-"+ names[2]
	f = open(fname, 'wb')
        data = sender.recv(1024)
        totalRecv = len(data)
        f.write(data)
        while totalRecv < size:
            data = sender.recv(1024)
            totalRecv += len(data)
            f.write(data)
        f.close()		
	sendFile(sendername,fname,names[1])	
	

def sendFile(sender, fname,receiver):
	try:

		fileN = fname.split("-")
		filename = fileN[ len(fileN)-1 ]
		n = len(online)
		for i in range(n): 
			if online[i] == receiver:
				client = clntList[i]
				client.send("File Received from " + sender + " : " + filename + ' : ' + str(os.path.getsize(fname)))


				if os.path.exists(fname):

					f = open(fname, 'rb')
					bytesToSend = f.read(1024)
					client.send(bytesToSend)
					while bytesToSend != "" :
					    bytesToSend = f.read(1024)
					    client.send(bytesToSend)
					f.close()
		if receiver in names and receiver not in online:
			if receiver in filesInQueue:
				filesInQueue[receiver].append((sender,fname,receiver))
			else :
				filesInQueue[receiver] = [(sender,fname,receiver)]	

		return			
	except Exception as msg:
		print msg;
	

def SendFileInGroup(sender,groupName,fileN):
	print groupName
	print fileN
	ad,members = GroupDetails[groupName]
	#members = info[1]
	#fname = sender + "-"+groupName+"-"+fileN
	for i in members:
		if i == sender:
			continue
		else:
			sendFile(sender,fileN,i)
			print "sent to" + sender
	return

def GetFileInGroup(msg,senderN,sender):
	# Suppose sender writes .G-File:group:FileName:Size	
	print msg
	msg = msg.split(':')
	size = long(msg[3])
	fname = msg[2]
	print fname
	gname = msg[1]
	print gname
	fname = senderN+"-"+gname+"-"+fname
	f = open(fname, 'wb')
        data = sender.recv(1024)
        totalRecv = len(data)
	print size
        f.write(data)
        while totalRecv < size:
            data = sender.recv(1024)
            totalRecv += len(data)
            f.write(data)
        f.close()
	print "File catched"		
	SendFileInGroup(senderN,gname,fname)	
	return	
	
def Main():
	host = ''
	port = 90
	if(len(sys.argv) == 3) :
		host = sys.argv[1]
		port = int(sys.argv[2])
	else:
		print ("Argument List is too short or too Long.\nArgument List should be [ IP, Port ]")
		exit(0)



	s = socket.socket()
	s.bind((host,port))


	s.listen(100) # max number of connections

	print "Waiting for new connections"
	while True:
		client, addr = s.accept()
		thread.start_new_thread(func,(client,addr))
		
if __name__=="__main__":
	Main()






		

