#include "resolver.h"

#ifndef PARSER_H_
#define PARSER_H_

#define error(msg) fprintf(stderr,"%s",msg)
#define info(msg) fprintf(stderr,"%s",msg)
#define debug(msg, ...) fprintf(stdout, msg, __VA_ARGS__)

#define SUCCESS 0
#define FAILURE -1

using namespace std;

class Formatter
{
	public:
		Request  static createRequest(string domainName);
		Response static formatAsResponse(char* buffer);
};


#endif
