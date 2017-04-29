
// ***	log_s is a basic UDP server that logs messages received to "echo.log" file.
	
	// in order to work correctly with echo_s, log_s MUST be executed using
	
		// $> ./log_s 9999

	// where '9999' represents the port number for log_s


// *** What's not working:

	// log_s is currently only writing the message received to echo.log; 
	// part 2 deliverable requires timestamp and IP address information
	// which is not currently present.
	
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <fstream>
#include <iostream>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
   int sock, length, n;
   socklen_t fromlen;
   struct sockaddr_in server;
   struct sockaddr_in from;
   char buf[1024];

   if (argc < 2) {
      fprintf(stderr, "ERROR, no port provided\n");
      exit(0);
   }
   
   sock=socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) error("Opening socket");
   length = sizeof(server);
   bzero(&server,length);
   server.sin_family=AF_INET;
   server.sin_addr.s_addr=INADDR_ANY;
   server.sin_port=htons(atoi(argv[1]));
   if (bind(sock,(struct sockaddr *)&server,length)<0) 
       error("binding");
   fromlen = sizeof(struct sockaddr_in);
   
   std::ofstream logfile;
   
   while (1) {
	   
       n = recvfrom(sock,buf, 1024, 0, (struct sockaddr *)&from, &fromlen);
	   
       if (n < 0)
	   { 
		   error("recvfrom");
	   }
	   logfile.open("echo.log", std::ofstream::app);
	   logfile << "\n'" << buf << "' received from: ";
	   logfile.close();
       write(1, "Received a datagram: ", 21);
       write(1, buf, n);
       n = sendto(sock,"Got your message\n", 17, 0, (struct sockaddr *)&from,fromlen);
       if (n  < 0) 
	   {
		   error("sendto");
	   }
   }
   return 0;
 }

