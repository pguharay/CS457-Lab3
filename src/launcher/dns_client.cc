#include <stdlib.h>
#include <fstream>
#include <string>

#include "../common/query_processor.h"
#include "../common/io.h"
#include "../common/util.h"
#include "../common/resolver.h"
#include "../common/codec.h"
#include "../common/referral_tracker.h"

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
	string name("");
	int i=0, length = 0;


	while(*(dnsName + i) != 0)
	{
		length++;i++;
	}
	i = 0;

	while(*(dnsName + i) != 0)
	{
		int count = static_cast<unsigned char>(*(dnsName + i));

		if((i+count) > length || count <= 0)
		{
			break;
		}

		for(int j=1;j<=count;j++)
		{
			name.push_back(*(dnsName + i + j));
		}

		name.push_back('.');
		i =  i + count + 1;
	}

	return name.substr(0, name.length() -1 );
}

string getQClassName(int qClassNum)
{
	if (qClassNum == 1)
		return string("IN");
	else
		return string("Class" + qClassNum);
}

string getTypeName(int typeNum)
{
	if (typeNum == AAAA_QTYPE)
		return string("AAAA");
	else if (typeNum == RRSIG_QTYPE)
		return string("RRSIG");
	else if (typeNum == NSEC_QTYPE)
		return string("NSEC");
	else if (typeNum == CNAME_QTYPE)
		return string("CNAME");
	else
		return string("TYPE" + typeNum);
}

typedef struct __attribute__((packed)) ipv6_address {
        uint16_t  segments[8];  // total 128 bits
}IPv6Address;


void printRRheader(RR rr)
{
	printf("\n");
	printf("%s ", const_cast<char*>(rr.NAME) );
	printf("%i ", ntohl(rr.info.TTL));
	printf("%s ", getQClassName(ntohs(rr.info.CLASS)).c_str() );
	printf("%s ", getTypeName(ntohs(rr.info.TYPE)).c_str() );
}

void printAAAA(RR rrAAAA)
{
	printRRheader(rrAAAA);

	IPv6Address address;
	memcpy(&address, &rrAAAA.RDATA, sizeof(IPv6Address));

	for (int i=0; i<8; i++)
	{
		printf("%x", ntohs(address.segments[i]));
		if (i < 7)
			printf(":");
	}
}

void printRRSIG(RR rrRRSIG, RRSIG rrsig)
{
	printRRheader(rrRRSIG);

	// print RDATA
	printf("%s ", getTypeName(ntohs(rrsig.fixed_length_data.type_covered)).c_str());
	printf("%i ", rrsig.fixed_length_data.algorithm);
	printf("%i ", rrsig.fixed_length_data.labels);
	printf("%i ", ntohl(rrsig.fixed_length_data.original_TTL));
	printf("%i ", ntohl(rrsig.fixed_length_data.signature_expiration));
	printf("%i ", ntohl(rrsig.fixed_length_data.signature_inception));
	printf("%i ", ntohs(rrsig.fixed_length_data.key_tag));
	printf("%s ", readDNSName(rrsig.signer).c_str());

	string signature(rrsig.signature);

	printf("%s ", encode(rrsig.signature, 128).c_str());
}

void printCNAME(RR rr)
{
	printRRheader(rr);

	string cname = readDNSName(rr.RDATA);
	printf("%s ", cname.c_str());
}

bool publishRRData(char* hostname, Response message, bool printDebugMsg)
{
	int numAnswers = ntohs(message.header.ANCOUNT);

	int iAAAA = -1;
	int iRRSIG_AAAA = -1;
	RRSIG rrsigAAAA;

	int iCNAME = -1;
	int iRRSIG_CNAME = -1;
	RRSIG rrsigCNAME;


	if (printDebugMsg) printf("\nProcessing %i answers\n",numAnswers);

	for(int i=0;i<numAnswers;i++)
	{
		if (ntohs(message.answerRR[i].info.TYPE) == AAAA_QTYPE)
		{
			if (printDebugMsg) printf("\n(Answer %i is AAAA, rdata length = %i)", i, ntohs(message.answerRR[i].info.RDLENGTH));

			// for debug - can there be one more than one AAAA record?
			if (iAAAA != -1)
				printf("More that one AAAA record found!");

			iAAAA = i;
		}
		else if (ntohs(message.answerRR[i].info.TYPE) == RRSIG_QTYPE)
		{
			RRSIG_Fixed rrsigFixedLengthField;
			RRSIG rrsigData;

			memcpy(&rrsigFixedLengthField, message.answerRR[i].RDATA, sizeof(RRSIG_Fixed));

			int offset = sizeof(RRSIG_Fixed);
			int j= 0;

			while(*(message.answerRR[i].RDATA + offset) != '\0')
			{
				rrsigData.signer[j++] = *(message.answerRR[i].RDATA + offset);
				offset++;
			}

			rrsigData.signer[j] = '\0';
			offset++;

			j= 0;

			//debug("\n offset %d \n", offset);
			//while(*(message.answerRR[i].RDATA + offset) != '\0')
			while(j<128)
			{
				rrsigData.signature[j++] = *(message.answerRR[i].RDATA + offset);
				offset++;
			}
			rrsigData.signature[j] = '\0';

			rrsigData.fixed_length_data = rrsigFixedLengthField;

			if (printDebugMsg) printf("\n(Answer %i is RRSIG, Type covered =  %i, rdata length = %i)", i, ntohs(rrsigData.fixed_length_data.type_covered), ntohs(message.answerRR[i].info.RDLENGTH));

			// Type covered must match AAAA or CNAME
			if (ntohs(rrsigData.fixed_length_data.type_covered) == AAAA_QTYPE)
			{
				iRRSIG_AAAA = i;
				memcpy(&rrsigAAAA,&rrsigData,sizeof(RRSIG));
			}
			else if (ntohs(rrsigData.fixed_length_data.type_covered) == CNAME_QTYPE)
			{
				iRRSIG_CNAME = i;
				memcpy(&rrsigCNAME,&rrsigData,sizeof(RRSIG));
			}
		}
		else if (ntohs(message.answerRR[i].info.TYPE) == CNAME_QTYPE)
		{
			if (printDebugMsg) printf("\n(Answer %i is CNAME,  rdata length = %i)", i, ntohs(message.answerRR[i].info.RDLENGTH));
			iCNAME = i;
		}
		else
		{
			if (printDebugMsg) printf("\n(Answer %i is Type %i,  rdata length = %i)", i, ntohs(message.answerRR[i].info.TYPE), ntohs(message.answerRR[i].info.RDLENGTH));
		}
	}

	// now print the results in order and with comments

	if (iAAAA != -1){
		if (iRRSIG_AAAA != -1)
		{
			printf("\nIPv6 address and signature found for %s:\n\n", hostname);
			printAAAA(message.answerRR[iAAAA]);
			printRRSIG(message.answerRR[iRRSIG_AAAA], rrsigAAAA);
		}
		else
		{
			printf("\nIPv6 address found for %s with no signature:\n\n", hostname);
			printAAAA(message.answerRR[iAAAA]);
		}
		printf("\n");
		return true;
	}
	else if (iCNAME != -1)
	{
		printf("\nNo IPv6 address found for %s\n", hostname);

		if (iRRSIG_CNAME != -1)
		{
			printf("CNAME and signature for %s are:\n", hostname);
			printCNAME(message.answerRR[iCNAME]);
			printRRSIG(message.answerRR[iRRSIG_CNAME], rrsigCNAME);
		}
		else
		{
			printf("CNAME for %s is:\n", hostname);
			printCNAME(message.answerRR[iCNAME]);
		}
		printf("\n");
		return true;
	}

	return false;  // answers not AAAA nor CNAME
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
		debug("\t Class = %u, Type = %u, Length = %u, TTL = %u NAME = %s DATA = %s\n",
					ntohs(message.additionalRR[i].info.CLASS),
					ntohs(message.additionalRR[i].info.TYPE),
					ntohs(message.additionalRR[i].info.RDLENGTH),
					ntohl(message.additionalRR[i].info.TTL),
					const_cast<char*>(message.additionalRR[i].NAME),
					resolveRdataValue(ntohs(message.additionalRR[i].info.TYPE), message.additionalRR[i].RDATA));
	}
	info("] \n");
}

bool getResponse(char* queryHostName, Message query, string serverIP, ReferralTracker* referralTracker, bool debug)
{
	UDPClient* client;
	ResponseReader* reader;
	Response message;

	int maxTimeouts = 3;
	int numTimeouts = 0;
	bool responseReceived = false;

	do
	{
		// == set up UDP client ==
		try
		{
			if (debug) printf("\n >>> Sending query to: %s\n", serverIP.c_str());

			client = new UDPClient(serverIP);
		}
		catch (...)
		{
			if (debug) printf("\n\nCould not connect to server.");
			return false;
		}

		// == send request ==
		client->sendRequest(query);

		// == get Response ==
		reader = new ResponseReader();
		try
		{
			message = client->receiveResponse(reader);
		}
		catch (int status)
		{
			// if timeout, try again
			if (status == FAILURE)
			{
				numTimeouts++;
				if (numTimeouts == maxTimeouts)  // go to next root server
				{
					delete reader;
					delete client;
					if (debug) printf("\n\nServer timed out.");
					return false;
				}
			}
		}
		catch (...)
		{
			printf("Error reading response message.");

			if (debug) printf("\n\nError reading response message.");
			return false;
		}

		responseReceived = true;

		delete reader;
		delete client;

	} while (!responseReceived);

	if (debug) printResponse(message);


	// == process Response ==

	// check if message is authoritative
	bool queryCompleted = false;
	if (message.header.AA == 0)  // not authoritative so get referral IP
	{
		for(int i=0;i<ntohs(message.header.ARCOUNT);i++)
		{
			if (ntohs(message.additionalRR[i].info.TYPE) == 1)
			{
				long* ptr = (long*)message.additionalRR[i].RDATA;
				sockaddr_in addr;
				addr.sin_addr.s_addr = (*ptr);
				string referralServer = inet_ntoa(addr.sin_addr);

				if ( referralTracker->containsIPAddress(referralServer) )
				{
					if (debug) printf("\n referral to previously referred server\n");
					continue;
				}
				else
				{
					try
					{
						referralTracker->addIPAddress(referralServer);
					}
					catch(...)
					{
						// an error here mean over 30 (arbitrarily set high) referred servers has been attempted
						// try next name server
						return false;
					}
				}


				if (debug) printf("\nNot Authoritative Server - referred to: %s\n", referralServer.c_str());

				queryCompleted = getResponse(queryHostName, query, referralServer, referralTracker, debug);

				if (queryCompleted)
					return true;
			}
		}

		if (debug) printf("\nError finding referral server\n");
		return false;
	}
	else
	{
		if (debug)
		{
			printf("\n\n%s Is Authoritative Server\n", serverIP.c_str());
			printf("\n============================================\n");
		}

		// check answer RRs
		if (ntohs(message.header.ANCOUNT) > 0)
			queryCompleted = publishRRData(queryHostName, message, debug);


		if (!queryCompleted)
		{
			// if you get to this point, then there were no answers or the publish found no applicable answers
			// try the next root server
			if (debug)
			{
				printf("\nNo applicable answers found\n");
				printf("\n============================================\n");
			}

			// advance to next root server
			if (debug) printf("\n\nNo data found.");
			return false;
		}
		else
		{
			if (debug) printf("\n============================================\n");
			return true;
		}
	}
}

int main(int argc, char** argv)
{
	char* hostName = NULL;

	// list of root servers on humboldt using dig . NS
	int numServers = 9;
	string rootServer[9] = {  "192.112.36.4", "198.41.0.4", "192.228.79.201",   "192.33.4.12",
	                         "128.8.10.90", "192.203.230.10",   "192.5.5.241",
	                           "128.63.2.53", "192.36.148.17"};

	// == parse command line arguments ==
	bool debug = false;
	if (argc == 3 && strcmp(argv[2],"-debug")==0)
		debug = true;
	else
		if (argc != 2)
			showUsageandExit();

	hostName = argv[1];

	// == create query ==
	QueryProcessor* qp = (QueryProcessor*)malloc(sizeof(QueryProcessor));
	Message queryMessage = qp->getDnsQuery(hostName);

	// ==> start dns lookup...
	int rootServerIndex = 0;
	string currentServer = rootServer[rootServerIndex];
//	bool getNextRootServer = false;

	ReferralTracker* referralTracker = new ReferralTracker();

	while (!getResponse(hostName,queryMessage,currentServer, referralTracker, debug))
	{
			if (debug) printf("\nTrying next root server...\n");
			rootServerIndex++;
			if (rootServerIndex == numServers)
			{
				printf("\nUnable to find IPv6 address or CNAME for %s\n", hostName);
				break;
			}
			currentServer = rootServer[rootServerIndex];
//			getNextRootServer = false;
	}

	if (debug) printf("\n# referred servers = %i\n", referralTracker->getNumberOfAddresses());

	return 1;
}

