#include "resolver.h"

#ifndef PARSER_H_
#define PARSER_H_

#define error(msg) fprintf(stderr,"%s",msg)
#define info(msg) fprintf(stderr,"%s",msg)
#define debug(msg, ...) fprintf(stdout, msg, __VA_ARGS__)

using namespace std;

class Formatter
{
	public:
		struct Request  static createRequest(string domainName);
		struct Response static formatAsResponse(char* buffer);
};


#endif
