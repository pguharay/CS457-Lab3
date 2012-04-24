#include <stdlib.h>
#include <fstream>
#include <string>

#include "../common/query_processor.h"
#include "../common/io.h"
#include "../common/util.h"

using namespace std;

void showUsageandExit()
{
	printf("\nUsage:  dns_resolver <URL>\n");
	exit(-1);
}

void printResults(char* hostname, int numAddresses, const char** addresses)
{
	if (numAddresses == 0)
	{
		printf("\nNo IPv6 addresses found for %s\n\n", hostname);
	}
	else
	{
		printf("\n");
		printf("%i IPv6 address", numAddresses);
		if (numAddresses > 1)
			printf("es");
		printf(" found for %s:\n", hostname);

		for (int i=0; i<numAddresses; i++)
			printf("\n%s", addresses[i]);

		printf("\n");
	}
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

char* resolveRdataValue(uint16_t type, char* RDATA)
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
	debug("ARCount = %u \n", ntohs(message.header.ARCOUNT));
	debug("NSCount = %u \n", ntohs(message.header.NSCOUNT));
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

int main(int argc, char** argv)
{
	char* hostName = NULL;
	//int nAddresses = 0;
	//const char** entries;

	// list of root servers on humboldt using dig . NS
	int numServers = 9;
	string rootServer[9] = {"198.41.0.4", "192.228.79.201",   "192.33.4.12",
									  "128.8.10.90", "192.203.230.10",   "192.5.5.241",
									 "192.112.36.4",    "128.63.2.53", "192.36.148.17"};


	// == parse command line arguments ==
	if (argc != 2)
		showUsageandExit();

	hostName = argv[1];

	//JE: using this code to test...
	QueryProcessor* qp = new QueryProcessor();
	//qp->getDnsNameFormat((char*)hostName);


	// ==> do some checking to verify valid hostname - in query processor?

	// == create query ==
	Message queryMessage = qp->getDnsQuery(hostName);

	// ==> start dns lookup...
	int rootServerIndex = 0;
	//int maxTimeouts = 3;
	//int numTimeouts = 0;
	bool queryCompleted = false;

	UDPClient* client = new UDPClient(rootServer[rootServerIndex]);
	ResponseReader* reader = new ResponseReader();
	Response message;

	while (!queryCompleted)
	{
		try
		{
			// if exists, need to destroy old client first??
			client->sendRequest(queryMessage);
			message = client->receiveResponse(reader);
		}
		catch (...)
		{
			rootServerIndex++;
			if (rootServerIndex == numServers)
				break;

			continue;
		}
		/*catch (int status)
		{
			// error message from client
			// if timeout, try again
			if (status == FAILURE)
			{
				numTimeouts++;
				if (numTimeouts == maxTimeouts)  // go to next root server
				{
					numTimeouts = 0;
					rootServerIndex++;
					if (rootServerIndex == numServers)
						break;
				}

				continue;
			}
		}
		 */

		//==>parse message


		//==> submit new queries if needed


		queryCompleted = true;  // set if no further queries
	}

/*
	// for testing...
	entries = new const char*[2];
	nAddresses++;
	entries[0] = std::string("this is a test").c_str();
	nAddresses++;
	entries[1] = std::string("this is a test too").c_str();

	// == print results ==
	printResults(hostName, nAddresses, entries);
*/
	if(queryCompleted)
	{
		printResponse(message);
	}
	else
	{
		info("Name could not be resolved \n");
	}

	return 1;
}
