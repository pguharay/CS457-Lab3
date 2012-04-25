#include <stdlib.h>
#include <fstream>
#include <string>

#include "../common/query_processor.h"
#include "../common/io.h"
#include "../common/util.h"
#include "../common/resolver.h"
#include "../common/codec.h"

using namespace std;

#define AAAA_QTYPE 28
#define RRSIG_QTYPE 46
#define CNAME_QTYPE 5
#define NSEC_QTYPE 47


void showUsageandExit()
{
	printf("\nUsage:  dns_resolver <URL>\n");
	exit(-1);
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

typedef struct __attribute__((packed)) ipv6_address {
        uint16_t  segments[8];  // total 128 bits
}IPv6Address;

void printAAAA(char* name, int TTL, int qclass, char* ipv6AddressData)
{
	printf("\n");
	printf("%s ", name);
	printf("%i ", TTL);
	if (qclass == 1)
		printf("IN ");
	else
		printf("Class%i ", qclass);
	printf("AAAA ");

	IPv6Address address;
	memcpy(&address, ipv6AddressData, sizeof(IPv6Address));

	for (int i=0; i<8; i++)
	{
		printf("%x", ntohs(address.segments[i]));
		if (i < 7)
			printf(":");
	}
}

void printRRSIG(char* name, int TTL, int qclass, RRSIG rrsig)
{
	printf("\n");
	printf("%s ", name);
	printf("%i ", TTL);
	if (qclass == 1)
		printf("IN ");
	else
		printf("Class%i ", qclass);
	printf("RRSIG ");

	// print RDATA
	int type_covered = ntohs(rrsig.type_covered);
	if (type_covered == AAAA_QTYPE)
		printf("AAAA ");
	else if (type_covered == NSEC_QTYPE)
		printf("NSEC ");
	else
		printf("TYPE%i ", type_covered);
	printf("%i ", rrsig.algorithm);
	printf("%i ", rrsig.labels);
	printf("%i ", ntohl(rrsig.original_TTL));
	printf("%i ", ntohl(rrsig.signature_expiration));
	printf("%i ", ntohl(rrsig.signature_inception));
	printf("%i ", ntohs(rrsig.key_tag));
	printf("%s ", readDNSName(rrsig.signer).c_str());

	string signature(rrsig.signature);

	printf("%s ", base64_encode(reinterpret_cast<const unsigned char*>(signature.c_str()), signature.length()).c_str());
}

void printCNAME(RR rr)
{
	printf("\n");
	printf("%s ", rr.NAME);
	printf("%i ", ntohl(rr.info.TTL));

	int qclass = ntohs(rr.info.CLASS);
	if ( qclass == 1)
		printf("IN ");
	else
		printf("Class%i ", qclass);

	printf("CNAME ");
	printf("%s ", readDNSName(rr.RDATA).c_str());

}

void publishRRData(Response message)
{
	int numAnswers = ntohs(message.header.ANCOUNT);
	// for debug
	printf("\nProcessing %i answers\n",numAnswers);

	for(int i=0;i<numAnswers;i++)
	{
		if (ntohs(message.answerRR[i].info.TYPE) == AAAA_QTYPE)
		{
			// for debug
			printf("\n(Answer %i is AAAA, rdata length = %i)", i, ntohs(message.answerRR[i].info.RDLENGTH));

			printAAAA(message.answerRR[i].NAME, ntohl(message.answerRR[i].info.TTL), ntohs(message.answerRR[i].info.CLASS), message.answerRR[i].RDATA);
		}
		else if (ntohs(message.answerRR[i].info.TYPE) == RRSIG_QTYPE)
		{
			RRSIG rrsigData;
			memcpy(&rrsigData, message.answerRR[i].RDATA, sizeof(RRSIG));

			// for debug
			printf("\n(Answer %i is RRSIG, Type covered =  %i, rdata length = %i)", i, ntohs(rrsigData.type_covered), ntohs(message.answerRR[i].info.RDLENGTH));

			// Type covered must match AAAA
			if (ntohs(rrsigData.type_covered) == AAAA_QTYPE)
				printRRSIG(message.answerRR[i].NAME, ntohl(message.answerRR[i].info.TTL), ntohs(message.answerRR[i].info.CLASS), rrsigData);
		}
		else if (ntohs(message.answerRR[i].info.TYPE) == CNAME_QTYPE)
		{
			// for debug
			printf("\n(Answer %i is CNAME,  rdata length = %i)", i, ntohs(message.answerRR[i].info.RDLENGTH));

			printCNAME(message.answerRR[i]);
		}
		else
		{
			// for debug
			printf("\n(Answer %i is Type %i,  rdata length = %i)", i, ntohs(message.answerRR[i].info.TYPE), ntohs(message.answerRR[i].info.RDLENGTH));
		}

	}
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
	int nAddresses = 0;
	const char** entries;

	// list of root servers on humboldt using dig . NS
	int numServers = 9;
	string rootServer[9] = {  "198.41.0.4", "192.228.79.201",   "192.33.4.12",
	                         "128.8.10.90", "192.203.230.10",   "192.5.5.241",
	                        "192.112.36.4",    "128.63.2.53", "192.36.148.17"};


	// == parse command line arguments ==
	if (argc != 2)
		showUsageandExit();

	hostName = argv[1];

	// == create query ==
	QueryProcessor* qp = (QueryProcessor*)malloc(sizeof(QueryProcessor));
	Message queryMessage = qp->getDnsQuery(hostName);

	// ==> start dns lookup...
	UDPClient* client;
	ResponseReader* reader;
	int rootServerIndex = 0;
	string currentServer = rootServer[rootServerIndex];
	int maxTimeouts = 3;
	int numTimeouts = 0;
	bool queryCompleted = false;

	while (!queryCompleted)
	{
		// == set up UDP client ==
		try
		{
			// if exists, need to destroy old client first??
			printf("\n >>> Sending query to: %s\n", currentServer.c_str());

			client = new UDPClient(currentServer);
			reader = new ResponseReader();
		}
		catch (...)
		{
			rootServerIndex++;
			if (rootServerIndex == numServers)
				break;
			else
				currentServer = rootServer[rootServerIndex];

			continue;
		}

		// == send request ==
		client->sendRequest(queryMessage);


		// == get Response ==
		Response message;
		try
		{
			message = client->receiveResponse(reader);
		}
		catch (int status)
		{
			delete reader;
			delete client;

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
					else
						currentServer = rootServer[rootServerIndex];
				}
				continue;
			}
		}
		catch (...)
		{
			printf("Error reading message.");
		}
		delete reader;
		delete client;

		// for debug
		printResponse(message);


		// == process Response ==

		// check if message is authoritative
		bool referralFound = false;
		if (message.header.AA == 0)  // not authoritative so get referral IP
		{
			for(int i=0;i<ntohs(message.header.NSCOUNT);i++)
			{
				if (ntohs(message.additionalRR[i].info.TYPE) == 1)
				{
					long* ptr = (long*)message.additionalRR[i].RDATA;
					sockaddr_in addr;
					addr.sin_addr.s_addr = (*ptr);
					currentServer = inet_ntoa(addr.sin_addr);

					// for debug
					printf("\nNot Authoritative Server - referred to: %s\n", currentServer.c_str());
					referralFound = true;
//					break;
				}
			}

			if (referralFound)
				continue;
			else
				printf("\nError finding referral server\n");
		}
		else
		{
			// for debug
			printf("\n\n%s Is Authoritative Server\n", currentServer.c_str());
			printf("\n============================================\n");

			// check answer RRs
			int numAnswers = ntohs(message.header.ANCOUNT);
			if (numAnswers > 0)
			{
				publishRRData(message);
			}
			else
			{
				// for debug
				printf("\nNo Answers found\n");
				printf("\n============================================\n");

				// try next root server
				rootServerIndex++;
				if (rootServerIndex == numServers)
					break;
				else
					currentServer = rootServer[rootServerIndex];

				continue;
			}

			// for debug
			printf("\n============================================\n");
		}


		//==> submit new queries if needed

		// for test, let's hit all the servers
		//rootServerIndex++;
		//if (rootServerIndex == numServers)
			queryCompleted = true;  // set if no further queries
		//else
		//	currentServer = rootServer[rootServerIndex];
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
	return 1;
}
