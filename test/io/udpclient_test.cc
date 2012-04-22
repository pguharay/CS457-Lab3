#include "udpclient_test.h"

using namespace std;

int main(int argc, char** argv)
{
	string serverHost = *(argv + 1);
	string domainName = *(argv + 2);

	srand(time(NULL));

	UDPClient* client = new UDPClient(serverHost);

	Message request = prepareRequest(domainName);

	debug("ID %u \n", ntohs(request.header.ID));

	client->sendRequest(request);

	Message message = client->receiveResponse();

	printResponse(message);
}

void printResponse(Message message)
{
	debug("ID = %u \n", ntohs(message.header.ID));
	debug("QR = %x \n", message.header.QR);
	debug("OPCODE = %x \n", message.header.OPCODE);
	debug("AA = %x \n", message.header.AA);
	debug("TC = %x \n", message.header.TC);
	debug("RD = %x \n", message.header.RD);
	debug("RA = %x \n", message.header.RA);
	debug("RCODE =%x \n", message.header.RCODE);
	debug("QDCount = %u \n", ntohs(message.header.QDCOUNT));
	debug("ANCount = %u \n", ntohs(message.header.ANCOUNT));
	debug("ARCount = %u \n", ntohs(message.header.ARCOUNT));
	debug("NSCount = %u \n", ntohs(message.header.NSCOUNT));
}

Message prepareRequest(string domainName)
{
	Header header;

	header.ID = htons(10);
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
	header.ARCOUNT = 0;
	header.NSCOUNT  = 0;

	string qname = formatDNSName(domainName);

	Question question;
	memcpy(question.QNAME, qname.c_str(), qname.size());
	question.QTYPE = htons(38);
	question.QCLASS = htons(1);

	Message request;
	request.header = header;
	request.query = question;

	return request;
}

string formatDNSName(string domainName)
{
	string dnsformat;
	int startIndex = 0;
	int i=0;

	for(;i < static_cast<int>(domainName.size());i++)
	{
		if(*(domainName.c_str() +i)  == '.')
		{
			dnsformat.push_back(i-startIndex);
			dnsformat.append(domainName.substr(startIndex, i));
			startIndex = i+1;
		}
	}

	dnsformat.push_back(i-startIndex);
	dnsformat.append(domainName.substr(startIndex, i));

	return dnsformat;
}
