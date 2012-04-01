#include <iostream>
#include "dns.h"

using namespace std;

#ifndef QUERY_PROCESSOR_H_
#define QUERY_PROCESSOR_H_

class QueryProcessor
{
	public:
		QueryProcessor();
		Request getDnsQuery(string hostAddress);
		const string getDnsNameFormat(const string hostAddress);

	private:
		uint16_t getRandomId();
};

#endif
