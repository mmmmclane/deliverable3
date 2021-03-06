// echo_s is a continous TCP/UDP echo server called from the terminal with syntax:
		//
		// $> ./echo_s <port1> [<port2> <port3>]
		// ex: ./echo_s 51720
		// ex: ./echo_s 51717 51718 51719
		//
	// where port1 is the port number for the server to listen on, and port2 and port3
	//		are optional additional port numbers to listen on
	//
// echo_s has optional command line arguments:
		//
		//  -logip <IPaddress>		# specify IP address of log server
		//							# default IP is assigned by system
		//
		//	-logport <port>			# specify port number of log server
		//							# default port number is 8888
		//
// All messages received by echo_s are sent to log_s via UDP where they are logged to echo.log

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <iostream>
#include <algorithm>

// signal flag for ctrl-c termination
volatile sig_atomic_t flag = 1;


// signal handler for when server is terminated with ctrl-c
void handle_sig(int sig)
{
	flag = 0;
}

// handle TCP messages and send them to log server
void dostuff(int sock, int logport, struct sockaddr_in addr);

// handle errors
void error(const char *msg);

// create TCP socket and open for listening
int createTCPSock(int portno, struct sockaddr_in addr);

// create and bind UDP socket 
int createUDPSock(int portno, struct sockaddr_in addr);

// continously loop, waiting for communication on TCP and UDP socket
void waitForCommunication(int tcpsock, int udpsock, int logport);

// send message buffer to remote log_s
void logMessage(char buffer[], int portno);

char* makeLogString(char *message);


int main(int argc, char *argv[])
{
	int udpfd, tcpfd, port;
	int num_ports=0, num_args=argc;
	char *port_args[num_args];
	std::string logip="";
	int logport=8888;
	int pid1, pid2;
	struct sockaddr_in serv_addr;

	// make sure user passes in port number
	if (argc < 2) {
		fprintf(stderr,"ERROR, incorrect number or args\n");
		exit(1);
	}
	
	std::string arg;
	for(int i = 1; i < num_args; i++)
	{
		arg = argv[i];
		if(arg == "-logip")
		{
			logip = argv[i+1];
			i++;
		}
		else if(arg == "-logport")
		{
			logport = atoi(argv[i+1]);
			i++;
		}
		else
		{
			port_args[num_ports] = argv[i];
			num_ports++;
		}
	}
	
	// tests for argument management above
	// for(int i = 0; i < num_ports; i++)
	// {
			// std::cout << port_args[i] << " "; 
	// }
	// std::cout << "\n" << logip << " " << logipaddress << "\n";
	
	if(num_ports > 2)
	{
		pid1 = fork();
		if(pid1 < 0)
		{
			error("ERROR FORKING PID1");
		}
		if(pid1 == 0)
		{
			port = atoi(port_args[1]);
			tcpfd = createTCPSock(port, serv_addr);
			udpfd = createUDPSock(port, serv_addr);
			waitForCommunication(tcpfd, udpfd, logport);
			exit(0);
		}
		if(num_ports > 3)
		{
			pid2 = fork();
			if(pid2 < 0)
			{
				error("ERROR FORKING PID1");
			}
			if(pid2 == 0)
			{
				port = atoi(port_args[2]);
				tcpfd = createTCPSock(port, serv_addr);
				udpfd = createUDPSock(port, serv_addr);
				waitForCommunication(tcpfd, udpfd, logport);
				exit(0);
			}
		}		
	}
	
	port = atoi(port_args[0]);
	tcpfd = createTCPSock(port, serv_addr);
	udpfd = createUDPSock(port, serv_addr);
	waitForCommunication(tcpfd, udpfd, logport);

	return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff (int sock, int logport, struct sockaddr_in addr)
{
	int n;
	char buffer[256];
	char newbuf[1024];
	bzero(buffer,256);

	time_t messagetime;
	time(&messagetime);
	struct tm *timeinfo = localtime(&messagetime);
	strftime(newbuf, sizeof(newbuf)-1, "%y-%m-%d %H:%M:%S \t\"", timeinfo);
	
// reads the message from the socket
	n = read(sock, buffer, 255);
	
// displays error if it can't read from the socket
	if (n < 0) 
	{
		error("ERROR reading from TCP socket\n");
	}
	printf("RECEIVED TCP MESSAGE: %s\n", buffer);
	
	strtok(buffer, "\n");
	char info[20];
	inet_ntop(AF_INET, &(addr), info, 20);
	strcat(buffer, "\" received from: ");
	strcat(buffer, info);
	strcat(newbuf, buffer);
	strcat(newbuf, "\n");
	logMessage(newbuf, logport);
	
	
	

// displays messages
	n = write(sock,"MESSAGE RECEIVED BY ECHO_S\n", 27);
	if (n < 0) error("ERROR writing to socket");
}



void error(const char *msg)
{
	perror(msg);
	exit(1);
}


// creates and prepares a TCP socket given a port number and sockaddr_in structure
int createTCPSock(int portno, struct sockaddr_in addr)
{
	int sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		error("ERROR CREATING TCP SOCK\n");
	}
	bzero((char*) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(portno);
	if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		error("ERROR BINDING TCP SOCK\n");
	}
	listen(sock,5);
	return sock;
}

// creates and prepares a UDP socket given a port number and sockaddr_in structure
int createUDPSock(int portno, struct sockaddr_in addr)
{
	int sock; 
	int length;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		error("ERROR OPENING UDP SOCK\n");
	}
	length = sizeof(addr);
	bzero(&addr, length);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(portno);
	if (bind(sock, (struct sockaddr *) &addr, length) < 0)
	{
		error("ERROR BINDING UDP SOCK\n");
	}
	return sock;
	
}

void waitForCommunication(int tcpsock, int udpsock, int logport){

	int maxsock, newtcpsock, ready, pid;
	fd_set fileset;
	struct sockaddr_in c_addr;
	ssize_t n;
	char buffer[1024];
	char buf_ctrl[] = {"echo_s is stopping\n"};
	socklen_t clientlength;
	clientlength = sizeof(struct sockaddr_in);

	
	signal(SIGCHLD,SIG_IGN);
	signal(SIGINT, handle_sig);
	FD_ZERO(&fileset);
	maxsock = std::max(tcpsock, udpsock) + 1;
	
	
	while(flag){
		FD_SET(tcpsock, &fileset);
		FD_SET(udpsock, &fileset);
		if((ready = select(maxsock, &fileset, NULL, NULL, NULL)) < 0)
		{
			if(errno == EINTR)
			{
				continue;
			}
			else
			{
				error("error choosing sock");
			}
		}
		// Handle TCP connection case
		if(FD_ISSET(tcpsock, &fileset))
		{
			clientlength = sizeof(c_addr);
			newtcpsock = accept(tcpsock, (struct sockaddr *) &c_addr, &clientlength);
			if (newtcpsock < 0)
			{
				error("ERROR on accept");
			}
			pid = fork();
			if (pid < 0)
			{
				error("ERROR on fork");
			}
			if (pid == 0)  
			{
				close(tcpsock);
				dostuff(newtcpsock, logport, c_addr);
				exit(0);
			}
			else
			{				
				close(newtcpsock);
			}
		}
		if(FD_ISSET(udpsock, &fileset))
		{
			socklen_t len;
			struct sockaddr_storage newaddr;
			char newbuff[1024];
			
			
			clientlength = sizeof(struct sockaddr_in);
			
			time_t messagetime;
			time(&messagetime);
			struct tm *timeinfo = localtime(&messagetime);
			strftime(newbuff, sizeof(newbuff)-1, "%y-%m-%d %H:%M:%S \t\"", timeinfo);
			
			
			n = recvfrom(udpsock, buffer, 1024, 0,(struct sockaddr *)&c_addr, &clientlength);
			
			strtok(buffer, "\n");
			char inf[clientlength];
			inet_ntop(AF_INET, &(c_addr), inf, 20);
			strcat(newbuff, buffer);
			strcat(newbuff, "\" received from: ");
			strcat(newbuff, inf);
			strcat(newbuff, "\n");
			if (n < 0) 
			{
				error("ERROR RECEIVING UDP MESSAGE\n");
			}
			
			//std::cout << "CLIENT INFO: " << inf << "\n";
			logMessage(newbuff, logport);
			write(1, "RECEIVED UDP MESSAGE: ", 22);
			write(1, buffer, n);
			write(1, "\n", 1);
			n = sendto(udpsock, "MESSAGE RECEIVED\n", 17, 0,(struct sockaddr *)&c_addr, clientlength);
			if (n  < 0)
			{
				error("sendto");
			}
		}
	}
	logMessage(buf_ctrl, logport);
	close(tcpsock);
	return;
}

void logMessage(char buffer[], int portno)
{
	int sock, n;
	unsigned int length;
	struct sockaddr_in server;
	struct hostent *hp;
//	char buffer[256];
	char const *args[] = {"localhost"};
	
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0)
	{
		error("ERROR CREATING LOG CLIENT\n");
	}
	server.sin_family = AF_INET;
	hp = gethostbyname(args[0]);
	if(hp == 0)
	{
		error("ERROR UNKNOWN HOST - LOG CLIENT\n");
	}
	bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	server.sin_port = htons(portno);
	length = sizeof(struct sockaddr_in);
	n = sendto(sock, buffer, strlen(buffer), 0, (const struct sockaddr *) &server, length);
	if(n < 0)
	{
		error("ERROR SENDING LOG MESSAGE");
	}
	close(sock);
	return;
}
