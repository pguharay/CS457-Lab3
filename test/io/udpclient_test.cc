#include "udpclient_test.h"

using namespace std;

int main(int argc, char** argv)
{
	string serverHost = *(argv + 1);
	string domainName = *(argv + 2);

	if(argc <2)
	{
		error("Usage: dns_client <name server> <domain name> \n");
		exit(1);
	}

	//UDPClient* client = new UDPClient(serverHost);
	ResponseReader* reader = new ResponseReader();
	Resolver* resolver = new Resolver(reader);
	Message request = prepareRequest(domainName);

	Response message = resolver->queryNameServer(serverHost, request);
	//client->sendRequest(request);

	//Response message = client->receiveResponse(reader);

	printResponse(message);

	delete reader;
	delete resolver;
}

void printResponse(Response message)
{
	debug("ID = %u \n", ntohs(message.header.ID));
	debug("QR = 0x%x \n", message.header.QR);
	debug("OPCODE = 0x%x \n", message.header.OPCODE);
	debug("AA = 0x%x \n", message.header.AA);
	debug("TC = 0x%x \n", message.header.TC);
	debug("RD = 0x%x \n", message.header.RD);
	debug("RA = 0x%x \n", message.header.RA);
	debug("RCODE = 0x%x \n", message.header.RCODE);
	debug("QDCount = %u \n", ntohs(message.header.QDCOUNT));
	debug("ANCount = %u \n", ntohs(message.header.ANCOUNT));
	debug("NSCount = %u \n", ntohs(message.header.NSCOUNT));
	debug("ARCount = %u \n", ntohs(message.header.ARCOUNT));
	debug("QNAME = %s \n", readDNSName(message.QNAME).c_str());
	debug("QTYPE = %u \n", ntohs(message.query.QTYPE));
	debug("QCLASS = %u \n", ntohs(message.query.QCLASS));

	info("Answer = [ \n");

	for(int i=0;i<ntohs(message.header.ANCOUNT);i++)
	{
		debug("\t Class = %u, Type = %u, Length = %u, TTL = %u NAME = %s\n",
					ntohs(message.answerRR[i].info.CLASS),
					ntohs(message.answerRR[i].info.TYPE),
					ntohs(message.answerRR[i].info.RDLENGTH),
					ntohl(message.answerRR[i].info.TTL),
					message.answerRR[i].NAME);
	}

	info("]\n");

	info("Authority answer = [ \n");

	for(int i=0;i<ntohs(message.header.NSCOUNT);i++)
	{
		debug("\t Class = %u, Type = %u, Length = %u, TTL = %u NAME = %s\n",
					ntohs(message.authorityRR[i].info.CLASS),
					ntohs(message.authorityRR[i].info.TYPE),
					ntohs(message.authorityRR[i].info.RDLENGTH),
					ntohl(message.authorityRR[i].info.TTL),
					readDNSName(message.authorityRR[i].RDATA).c_str());
	}

	info("] \n");

	info("Additional Answer = [ \n");
	for(int i=0;i<ntohs(message.header.ARCOUNT);i++)
	{
		debug("\t Class = %u, Type = %u, Length = %u, TTL = %u NAME = %s\n",
					ntohs(message.additionalRR[i].info.CLASS),
					ntohs(message.additionalRR[i].info.TYPE),
					ntohs(message.additionalRR[i].info.RDLENGTH),
					ntohl(message.additionalRR[i].info.TTL),
					resolveRdataValue(ntohs(message.additionalRR[i].info.TYPE), message.additionalRR[i].RDATA));
	}
	info("] \n");
}

char* resolveRdataValue(uint16_t type, char* RDATA)
{

	if(type == 1)
	{
		long* ptr = (long*)RDATA;
		sockaddr_in addr;
		addr.sin_addr.s_addr = (*ptr);
		return inet_ntoa(addr.sin_addr);
	}
	else if (type == 0x1c)
	{
		unsigned char data[16];
		char str[INET6_ADDRSTRLEN];
		strcpy((char*)data, RDATA);
		int i = 0;
		for (;i<16;i++)
		{
			data[i] = (char)*(RDATA + i);
		}
		sockaddr_in6 addr;
		memcpy(&addr.sin6_addr.s6_addr, RDATA, 16);
		inet_ntop(AF_INET6, &(addr.sin6_addr), str, INET6_ADDRSTRLEN);
		return str;
	}

	return RDATA;
}

Message prepareRequest(string domainName)
{
	uint16_t randomID = time(NULL) % 100;

    Header header;
    header.ID = htons(randomID);
	header.QR = 0;
	header.OPCODE = 0;
	header.AA = 0;
	header.TC = 0;
	header.RD = htons(1);
	header.RA = 0;
	header.reserved_1  = 0;
	header.reserved_2  = 0;
	header.reserved_3  = 0;
	header.RCODE = 0;
	header.QDCOUNT = htons(1);
	header.ANCOUNT  = 0;
	header.NSCOUNT = 0;
	header.ARCOUNT  = 0;

	string qname = formatDNSName(domainName);

	Question question;
	question.QTYPE = htons(255);
	question.QCLASS = htons(1);

	Message request;
	request.header = header;

	memcpy(request.QNAME, qname.c_str(), qname.length()+1);
	request.QNAME[qname.length() + 1] = '\0';

	request.query = question;

	return request;
}

string readDNSName(char* dnsName)
{
	string name;
	int i=0;

	while(*(dnsName + i) != '\0')
	{
		int count = *(dnsName + i);

		for(int j=1;j<=count;j++)
		{
			name.push_back(*(dnsName + i + j));
		}

		name.push_back('.');
		i =  i + count + 1;
	}

	return name.substr(0, name.length()-1);
}

string formatDNSName(string domainName)
{
	string dnsformat;
	int startIndex = 0;
	int i=0;
	char length;

	for(;i < static_cast<int>(domainName.size());i++)
	{
		if(*(domainName.c_str() +i)  == '.')
		{
			length = i-startIndex;
			dnsformat.push_back(length);
			dnsformat.append(domainName.substr(startIndex, i-startIndex));
			startIndex = i+1;
		}
	}

	length = i-startIndex;
	dnsformat.push_back(length);
	dnsformat.append(domainName.substr(startIndex, length));

	return dnsformat;
}
