Group 9, Third Deliverable
https://github.com/mmmmclane/deliverable3

Members: Stephen Ballenger | netID: smb160530 | user1
         Tushara John | netID: tmj150130 | user2
         Carla Macias Montoya | netID: cxm150730 | user3
         Dawood Nadurath | netID: dsn150130 | user4
         Auveed Saki | netID: axs157130 | user5 and user6

Roles/Responsibilities: 

	user1 - Stephen modified echo_s to accept “-logip” argument
	user2 - Tushara modified echo_s to accept “-logport” argument
	user3 - Carla modified log_s to accept “-port” argument
	user4 - Dawood modified echo_s and log_s to handle ctrl-c signal
	user5 - Auveed modified log_s so that it will log ctrl-c signal
	user6 - Auveed updated ReadMe.txt to reflect group member work 

Directions: 

1. Clone repo using located at http://www.github.com/mmmmclane/deliverable3 using git

2. Compile all files using 'make'

3. start log server using command './log_s’ (use -port <port> to specify port number)

4. start echo server using command './echo_s <port1> [<port2> <port3>]' 
	(use -logip <IP> or -logport <port> to specify log server IP or port number)

5. connect TCP or UDP client using command './echo_c <sockettype> <host> <portnumber>'


Program description: 


*** 	echo_s is a continuous TCP/UDP echo server called from the terminal with syntax:
		
		 $> ./echo_s <port1> [<port2> <port3>]
		 ex: ./echo_s 51720
		 ex: ./echo_s 51717 51718 51719
		
	 where port1 is the port number for the server to listen on, and port2 and port3
			are optional additional port numbers to listen on
	
	echo_s has optional command line arguments:
		
	-logip <IPaddress>		# specify IP address of log server
					# default IP is assigned by system
		
	-logport <port>			# specify port number of log server
					# default port number is 8888
		
 * All messages received by echo_s are sent to log_s via UDP where they are logged to echo.log


***	echo_c is a TCP/UDP client that is called from the terminal with syntax:
		
		$> ./echo_c <sockettype> <host> <portnumber>
		
		ex: ./echo_c UDP localhost 51717
	
		where   <sockettype> specifies the type of socket ("UDP" or "TCP") 
	      		<host> specifies network host (usually "localhost").
	      		<portno> specifies the port number 
	
	echo_c works by determining the type of client to create and prompting user to enter message to be transmitted
	to server (presumably echo_s)



*** 	log_s is a basic UDP server that logs messages received by echo_s to "echo.log" file.
	
	log server is initialized using the following command:
	
	 	$> ./log_s
	
	 by default, log_s listens on port 8888, but can listen on user-specified port
	 using argument "-port":
	
	 	./log_s -port 9999
	