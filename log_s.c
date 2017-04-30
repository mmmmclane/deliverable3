// 	log_s is a basic UDP server that logs messages received by echo_s to "echo.log" file.
	//
	// log server is initialized using the following command:
	//
	// ./log_s
	//
	// by default, log_s listens on port 8888, but can listen on user-specified port
	// using argument "-port":
	//
	// ./log_s -port 9999
	//
//

	
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
   int num_args=argc, port=8888;
   socklen_t fromlen;
   struct sockaddr_in server;
   struct sockaddr_in from;
   char buf[1024];
   
   std::string arg;
   if(num_args > 2)
   {
	   for(int i = 1; i < num_args; i++)
	   {
		   arg = argv[i];
		   if(arg == "-port")
		   {
				port = atoi(argv[i+1]);
		   }
	   }
   }
   
   sock=socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) error("Opening socket");
   length = sizeof(server);
   bzero(&server,length);
   server.sin_family=AF_INET;
   server.sin_addr.s_addr=INADDR_ANY;
   server.sin_port=htons(port);
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
	   logfile << "\n" << buf << " ";
	   logfile.close();
       write(1, buf, n);
       n = sendto(sock,"Message received by log_s and logged to echo.log\n", 49, 0, (struct sockaddr *)&from,fromlen);
       if (n  < 0) 
	   {
		   error("sendto");
	   }
   }
   return 0;
 }

