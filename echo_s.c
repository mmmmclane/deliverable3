// ***	echo_s is a continous TCP/UDP echo server called from the terminal with syntax:

		// $> ./echo_s <port1> [<port2> <port3>]
	
		// ex: ./echo_s 51720
		// ex: ./echo_s 51717 51718 51719
	
	// where port1 is the port number for the server to listen on, and port2 and port3 are optional, additional
	// port numbers to listen on. echo_s accomplishes listening on multiple ports by forking child processes that
	// create a unique sock on their respective port. echo_s also sends all messages received via UDP to log_s. 
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

// handle TCP messages and send them to log server
void dostuff(int); 

// handle errors
void error(const char *msg);

// create TCP socket and open for listening
int createTCPSock(int portno, struct sockaddr_in addr);

// create and bind UDP socket 
int createUDPSock(int portno, struct sockaddr_in addr);

// continously loop, waiting for communication on TCP and UDP socket
void waitForCommunication(int tcpsock, int udpsock);

// send message buffer to remote log_s
void logMessage(char buffer[]);

int main(int argc, char *argv[])
{
	int udpfd, tcpfd, port;
	int pid1, pid2;
	struct sockaddr_in serv_addr;

	// make sure user passes in port number
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	if(argc > 2)
	{
		pid1 = fork();
		if(pid1 < 0)
		{
			error("ERROR FORKING PID1");
		}
		if(pid1 == 0)
		{
			port = atoi(argv[2]);
			tcpfd = createTCPSock(port, serv_addr);
			udpfd = createUDPSock(port, serv_addr);
			waitForCommunication(tcpfd, udpfd);
			exit(0);
		}
		if(argc > 3)
		{
			pid2 = fork();
			if(pid2 < 0)
			{
				error("ERROR FORKING PID1");
			}
			if(pid2 == 0)
			{
				port = atoi(argv[3]);
				tcpfd = createTCPSock(port, serv_addr);
				udpfd = createUDPSock(port, serv_addr);
				waitForCommunication(tcpfd, udpfd);
				exit(0);
			}
		}		
	}
	
	port = atoi(argv[1]);
	tcpfd = createTCPSock(port, serv_addr);
	udpfd = createUDPSock(port, serv_addr);
	waitForCommunication(tcpfd, udpfd);

	return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff (int sock)
{
	int n;
	int pid3;
	char buffer[256];
	bzero(buffer,256);

// reads the message from the socket
	n = read(sock,buffer,255);
	
// displays error if it can't read from the socket
	if (n < 0) 
	{
		error("ERROR reading from socket");
	}
	
	logMessage(buffer);
	
	printf("Here is the message: %s\n",buffer);

// displays messages
	n = write(sock,"I got your message",18);
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

void waitForCommunication(int tcpsock, int udpsock){

	int maxsock, newtcpsock, ready, pid;
	fd_set fileset;
	struct sockaddr_in c_addr;
	ssize_t n;
	char buffer[1024];
	socklen_t clientlength;
	clientlength = sizeof(struct sockaddr_in);

	signal(SIGCHLD,SIG_IGN);
	FD_ZERO(&fileset);
	maxsock = std::max(tcpsock, udpsock) + 1;
	
	
	while(1){
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
				dostuff(newtcpsock);
				exit(0);
			}
			else
			{				
				close(newtcpsock);
			}
		}
		if(FD_ISSET(udpsock, &fileset))
		{
			clientlength = sizeof(struct sockaddr_in);
			n = recvfrom(udpsock, buffer, 1024, 0,(struct sockaddr *)&c_addr, &clientlength);
			if (n < 0) 
			{
				error("ERROR RECEIVING UDP MESSAGE\n");
			}
			logMessage(buffer);
			write(1, "RECEIVED UDP MESSAGE: ", 22);
			write(1, buffer, n);
			n = sendto(udpsock, "MESSAGE RECEIVED\n", 17, 0,(struct sockaddr *)&c_addr, clientlength);
			if (n  < 0)
			{
				error("sendto");
			}
		}
	}
	close(tcpsock);
	return;
}

void logMessage(char buffer[])
{
	int sock, n;
	unsigned int length;
	struct sockaddr_in server, from;
	struct hostent *hp;
//	char buffer[256];
	char *args[] = {"localhost", "9999"};
	
	
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
	server.sin_port = htons(atoi(args[1]));
	length = sizeof(struct sockaddr_in);
	n = sendto(sock, buffer, strlen(buffer), 0, (const struct sockaddr *) &server, length);
	if(n < 0)
	{
		error("ERROR SENDING LOG MESSAGE");
	}
	close(sock);
	return;
}