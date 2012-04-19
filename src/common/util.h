#include "resolver.h"

#ifndef PARSER_H_
#define PARSER_H_


using namespace std;

class Formatter
{
	public:
		struct Request  static createRequest(string domainName);
		struct Response static formatAsResponse(char* buffer);
};


#endif
