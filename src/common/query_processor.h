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
		char* getDnsNameFormat(char* hostAddress);

	private:
		uint16_t getRandomId();
};

#endif
