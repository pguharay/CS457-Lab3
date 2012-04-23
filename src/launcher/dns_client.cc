#include <stdlib.h>
#include <fstream>
#include <string>

#include "../common/query_processor.h"
#include "../common/io.h"

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


int main(int argc, char** argv)
{
	char* hostName = NULL;
	int nAddresses = 0;
	const char** entries;

	// list of root servers on humboldt using dig . NS
	int numServers = 9;
	string rootServer[numServers] = {  "198.41.0.4", "192.228.79.201",   "192.33.4.12",
									  "128.8.10.90", "192.203.230.10",   "192.5.5.241",
									 "192.112.36.4",    "128.63.2.53", "192.36.148.17"};


	// == parse command line arguments ==
	if (argc != 2)
		showUsageandExit();

	hostName = argv[1];

	//JE: using this code to test...
	QueryProcessor* qp = (QueryProcessor*)malloc(sizeof(QueryProcessor));
	qp->getDnsNameFormat((char*)hostName);


	// ==> do some checking to verify valid hostname - in query processor?

	// == create query ==
	Message queryMessage = qp->getDnsQuery(hostName);

	// ==> start dns lookup...
	UDPClient* client;
	int rootServerIndex = 0;
	int maxTimeouts = 3;
	int numTimeouts = 0;
	bool queryCompleted = false;
	while (!queryCompleted)
	{
		try
		{
			// if exists, need to destroy old client first??

			client = new UDPClient(rootServer[rootServerIndex]);
		}
		catch (...)
		{
			rootServerIndex++;
			if (rootServerIndex == numServers)
				break;

			continue;
		}

		client->sendRequest(queryMessage);

		try
		{
			Message message = client->receiveResponse();
		}
		catch (int status)
		{
			// error message from client
			// if timeout, try again
			if (status == 0)
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
		catch (...)
		{
		}


		//==>parse message


		//==> submit new queries if needed


		queryCompleted = true;  // set if no further queries
	}


	// for testing...
	entries = new const char*[2];
	nAddresses++;
	entries[0] = std::string("this is a test").c_str();
	nAddresses++;
	entries[1] = std::string("this is a test too").c_str();

	// == print results ==
	printResults(hostName, nAddresses, entries);

	return 1;
}



