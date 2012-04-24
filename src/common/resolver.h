#include "response_reader.h"
#include "io.h"

using namespace std;

#ifndef RESOLVER_H_
#define RESOLVER_H_

class Resolver
{
	private:
		UDPClient* udpClient;
		ResponseReader* responseReader;
		string nextNameServer(Response response);
		char* resolveRdataValue(uint16_t type, char* RDATA);
	public:
		Resolver(ResponseReader* reader);
		Response queryNameServer(string nameserver, Message req);
};

#endif
