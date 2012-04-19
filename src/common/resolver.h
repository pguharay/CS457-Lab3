#include <iostream>
#include "dns.h"

using namespace std;

#ifndef RESOLVER_H_
#define RESOLVER_H_

class Resolver
{
	public:
		Resolver(string rootServer[5]);
		struct Response queryNameServer(string nameserver, struct Request req);
};

#endif
