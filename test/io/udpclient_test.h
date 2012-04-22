#include <stdio.h>
#include "../../src/common/io.h"

#ifndef UDPCLIENT_TEST_H_
#define UDPCLIENT_TEST_H_

string formatDNSName(string domainName);
Message prepareRequest(string domainName);
void printResponse(Message message);

#endif
