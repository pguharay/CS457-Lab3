#include <stdlib.h>
#include <fstream>

void showUsageandExit()
{
	printf("\nUsage:  dns_resolver <URL>\n");
	exit(-1);
}

void printResults(char* hostname, int numAddresses, char** addresses)
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
	char** entries;

	// == parse command line arguments ==
	if (argc != 2)
		showUsageandExit();

	hostName = argv[1];

	// ==> do some checking to verify valid hostname - in query processor?



	// ==> start dns lookup...





	// for testing...
	entries = new char*[2];
	nAddresses++;
	entries[0] = "this is a test";
	nAddresses++;
	entries[1] = "this is a test too";

	// == print results ==
	printResults(hostName, nAddresses, entries);

	return 1;
}



