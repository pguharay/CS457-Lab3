#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <fstream>
#include "resolver.h"

#ifndef IO_H_
#define IO_H_

class UDPClient
{
	public:
		void sendRequest(struct sockaddr_in* serverAddress, struct Request* request);
		struct Response* receiveResponse(struct sockaddr_in* serverAddress);
};


#endif
