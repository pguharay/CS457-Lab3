#include <stdio.h>
#include "../../src/common/io.h"
#include "../../src/common/response_reader.h"

#ifndef UDPCLIENT_TEST_H_
#define UDPCLIENT_TEST_H_

string formatDNSName(string domainName);
Message prepareRequest(string domainName);
void printResponse(Response message);
string readDNSName(char* dnsName);

#endif
