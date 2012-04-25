#include "../../src/common/resolver.h"

Resolver::Resolver(ResponseReader* reader)
{
	responseReader = reader;
}

Response Resolver::queryNameServer(string nameserver, Message request)
{
	Response response;
	bool resolved = false;

	do
	{
		udpClient = new UDPClient(nameserver);

		udpClient->sendRequest(request);
		response = udpClient->receiveResponse(responseReader);

		if(ntohs(response.header.ANCOUNT) <= 0)
		{
			nameserver = nextNameServer(response);
		}
		else
		{
			resolved = true;
			break;
		}

		delete udpClient;
	}
	while(ntohs(response.header.ANCOUNT) <= 0);

	if(resolved)
	{
		return response;
	}
	else
	{
		throw "Could not be resolved \n";
	}
}

string Resolver::nextNameServer(Response response)
{
	string nameserver = "";
	for(int i=0;i<ntohs(response.header.ARCOUNT);i++)
	{
		if(ntohs(response.additionalRR[i].info.TYPE) == 1)
		{
			nameserver = resolveRdataValue(ntohs(response.additionalRR[i].info.TYPE), response.additionalRR[i].RDATA);
		}
	}

	return nameserver;
}

char* Resolver::resolveRdataValue(uint16_t type, char* RDATA)
{

	if(type == 1)
	{
		long* ptr = (long*)RDATA;
		sockaddr_in addr;
		addr.sin_addr.s_addr = (*ptr);
		return inet_ntoa(addr.sin_addr);
	}

	return RDATA;
}


