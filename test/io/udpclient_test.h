#include <stdio.h>
#include "../../src/common/io.h"
#include "../../src/common/response_reader.h"
#include "../../src/common/resolver.h"

#ifndef UDPCLIENT_TEST_H_
#define UDPCLIENT_TEST_H_

string formatDNSName(string domainName);
Message prepareRequest(string domainName);
void printResponse(Response message);
string readDNSName(char* dnsName);
char* resolveRdataValue(uint16_t type, char* RDATA);

#endif
