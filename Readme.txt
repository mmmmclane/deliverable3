Group 9, Second Deliverable

Members: Stephen Ballenger | netID: smb160530
         Tushara John | netID: tmj150130
         Carla Macias Montoya | netID: cxm150730
         Dawood Nadurath | netID: dsn150130
         Auveed Saki | netID: axs157130

Roles/Responsilibites: Overall, we collaborated as a team to complete the second deliverable. 
Tushara and Carla worked on the multi-port functionality in echo_s, while Stephen, Auveed, and Dawood
worked on logging messages from echo_s to log_s. 


1. Compile all files using 'make'

2. start log server using command './log_s 9999'

3. start echo server using command './echo_s <port1> [<port2> <port3>]'

4. connect TCP or UDP client using command './echo_c <sockettype> <host> <portnumber>'


Program description: 


***	echo_s is a continous TCP/UDP echo server called from the terminal with syntax:

		$> ./echo_s <port1> [<port2> <port3>]
	
		ex: ./echo_s 51720
		ex: ./echo_s 51717 51718 51719
	
	where port1 is the port number for the server to listen on, and port2 and port3 are optional, additional
	port numbers to listen on. echo_s accomplishes listening on multiple ports by forking child processes that
	create a unique sock on their respective port. echo_s also sends all messages received via UDP to log_s. 


***	echo_c is a TCP/UDP client that is called from the terminal with syntax:
		
		$> ./echo_c <sockettype> <host> <portnumber>
		
		ex: ./echo_c UDP localhost 51717
	
		where   <sockettype> specifies the type of socket ("UDP" or "TCP") 
	      		<host> specifies network host (usually "localhost").
	      		<portno> specifies the port number 
	
	echo_c works by determining the type of client to create and prompting user to enter message to be transmitted
	to server (presumably echo_s)


***	log_s is a basic UDP server that logs messages received to "echo.log" file.
	
	in order to work correctly with echo_s, log_s MUST be executed using
	
		$> ./log_s 9999

	where '9999' represents the port number for log_s


*** What's not working:

	log_s is currently only writing the message received to echo.log; 
	part 2 deliverable requires timestamp and IP address information
	which is not currently present.

	