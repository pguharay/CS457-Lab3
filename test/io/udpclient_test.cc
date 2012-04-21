#include "../../src/common/io.h"


int main(int argc, char** argv)
{
	string serverHost = *(argv + 1);
	string domainName = *(argv + 2);
	srand(time(NULL));
}


Request* prepareRequest(string domainName)
{
	Header* header = (Header*)malloc(sizeof(Header));

	header->ID = rand();
	header->QR = 0;
	header->OPCODE = 0;
	header->AA = 0;
	header->TC = 0;
	header->RD = 1;
	header->RA = 0;
	header->reserved_Z  = 0;
	header->RCODE = 0;
	header->QDCOUNT = htons(1);
	header->ANCOUNT  = 0;
	header->ARCOUNT = 0;
	header->NSCOUNT  = 0;

	Question* question = (Question)malloc(sizeof(Question));
	question->QTYPE = htons(28);
	question->QCLASS = htons(1);

	return NULL;
}

char* formatDNSName(string domainName)
{
	const char* domain = domainName.c_str();

	string dnsformat(domainName);

	for(int i=0;i < static_cast<int>(domainName.size());i++)
	{
		if(domainName[i] == '.')
		{

		}
	}

	return NULL;
}
