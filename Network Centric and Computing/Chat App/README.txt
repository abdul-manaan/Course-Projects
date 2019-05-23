					********************************************************
					*         WHATSAPP MODEL (Developer: Abdul Manan)      *
					********************************************************
----------------------
Command Line Arguments
----------------------
For server:

Type: server.py [IP Address] [Port Number]
e.g you can type: 
				server.py 192.0.0.1 6100 
For clients:

Type: app.py [Server IP] [Server Port NO.] [Your Name]
e.g. You can type:
				app.py 192.0.0.1 6100 Abdul-Manan


-------------------------
Design Decisions & Issues
-------------------------

Q: What are some features of this app?

Ans: This APP is basically a chat application for different clients to chat together. They can chat with each other individually or by forming groups. The additional features 
     include you can see which of your friends are online and which are offline. Moreover, you can send ALL types of files including audio and video files. You can set your 
     status too and your friends will be able to see your status.	 		 

Q: What was my approach and how did I actually implement?

Ans: I basically started with a simple client-server model to send text to each other. Then, I added following features gradually:
	-> introduced more clients 
	-> client-client text messaging
	-> file sharing
	-> status updating
	-> keeping data of users
	-> keeping track of who are online and who are offline
	-> making groups
	-> admin of groups and options for joining/leaving a group
	-> file sharing in groups
	-> A User Interface

Q: How multithreading is done?

Ans: Actually I have used multithreading in both sidesi.e. client side and server side. On client side, it is for receiving data from server constantly. On the server side, 
     the session of each new user is in a new thread. 	

Q: What were some of the difficulties and how do I handle them?

Ans: The biggest difficulty I faced first was PYTHON itself. So, I had to spent about 1 week in making myself comfortable with python.
     Then, it was multithreading. I had to read a lot to get enough idea about it so that I could implement it.
     Afterwards, it was file sharing. I was able to send file from client to server but then, from server to other clients, it was not that easy. It was a major problem
     for me till the last day.

Q: Why have I done this assignment alone?
Ans: I have decided to do this assignment alone because more or less everybody in LUMS is a SNAKE. I mean, they are not sincere. But those who are, they usually do not like to 
     work. 

	--------------------------------------	
		Thanks for Reading....
	--------------------------------------

      
 
