#include <stdio.h>
#include "../../src/common/io.h"

#ifndef UDPCLIENT_TEST_H_
#define UDPCLIENT_TEST_H_

string formatDNSName(string domainName);
Request prepareRequest(string domainName);

#endif
