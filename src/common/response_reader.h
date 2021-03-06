#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "dns.h"

using namespace std;

#ifndef RESPONSE_READER_H_
#define RESPONSE_READER_H_

class ResponseReader
{
	private :
		int offset;
		void readAnswer(char*response, Response* message);
		void readAuthoritativeAnswer(char*response, Response* message);
		void readAdditionalAnswer(char* response, Response* message);
	public:
		Response read(char* response);
		string readName(char* response);
		string readPointer(char* response, int offsetPointer);
};


#endif
