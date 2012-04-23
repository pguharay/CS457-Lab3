#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <fstream>
#include "resolver.h"
#include "util.h"
#include "response_reader.h"

#ifndef IO_H_
#define IO_H_

#define DNS_PORT "53"

class UDPClient
{
	private:
		addrinfo  		serverAddressQuery;
		addrinfo* 		serverAddressResultList;
		pollfd   		poller;
		sockaddr* 		serverAddress;
		int 			socketID;
		char 			buffer[500];
		void createNetworkDataFromMessage(Message message);
		Response formatResponseToMessage(char* response, ResponseReader* responseReader);
	public:
		UDPClient(string serverHost);
		void sendRequest(Message request);
		Response receiveResponse(ResponseReader* responseReader);
};

#endif
