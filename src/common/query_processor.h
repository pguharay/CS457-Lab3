#include <iostream>
#include "dns.h"

using namespace std;

#ifndef QUERY_PROCESSOR_H_
#define QUERY_PROCESSOR_H_

class QueryProcessor
{
	public:
		QueryProcessor();
		Message getDnsQuery(char* hostAddress);

	private:
		Question 	createQuestion();
		string 		createQName(string domainName);
		Header 		createMessageHeader();
		string 		formatDNSName(string domainName);
};

#endif
