/*
 * query_processor_test.cc
 *
 *  Created on: Mar 31, 2012
 *      Author: jon
 */

#include "../../src/common/io.h"
#include "../../src/common/query_processor.h"



int main(int argc, char** argv)
{
	string host = "www.google.com";
	QueryProcessor* qp = (QueryProcessor*)malloc(sizeof(QueryProcessor));
	qp->getDnsNameFormat(host);
}
