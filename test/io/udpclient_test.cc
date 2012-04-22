#include "udpclient_test.h"

using namespace std;

int main(int argc, char** argv)
{
	string serverHost = *(argv + 1);
	string domainName = *(argv + 2);

	srand(time(NULL));

	UDPClient* client = new UDPClient(serverHost);

	Request request = prepareRequest(domainName);

	client->sendRequest(request);

	Response response = client->receiveResponse();
}

Request prepareRequest(string domainName)
{
	Header header;

	header.ID = rand();
	header.QR = 0;
	header.OPCODE = 0;
	header.AA = 0;
	header.TC = 0;
	header.RD = 1;
	header.RA = 0;
	header.reserved_Z  = 0;
	header.RCODE = 0;
	header.QDCOUNT = htons(1);
	header.ANCOUNT  = 0;
	header.ARCOUNT = 0;
	header.NSCOUNT  = 0;

	string qname = formatDNSName(domainName);

	Question question;
	memcpy(question.QNAME, qname.c_str(), qname.size());
	question.QTYPE = htons(28);
	question.QCLASS = htons(1);

	Request request;
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
