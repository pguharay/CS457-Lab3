#include <iostream>
#include "dns.h"

using namespace std;

#ifndef RESOLVER_H_
#define RESOLVER_H_

class Resolver
{
	public:
		Resolver(string rootServer[5]);
		Response queryNameServer(string nameserver, Request req);
};

#endif
