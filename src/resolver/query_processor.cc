/*
 * query_processor.cc
 *
 *  Created on: Mar 31, 2012
 *      Author: jon
 */
#include <vector>
#include "../common/io.h"
#include "../common/dns.h"
#include "../common/query_processor.h"


QueryProcessor::QueryProcessor() {}


Request* QueryProcessor::getDnsQuery(char* hostAddress){
	Request* request = (Request*)malloc(sizeof(Request));
	Header* header = (Header*)malloc(sizeof(Header));
	Question* question;

	header->ID = getRandomId();
	header->QR = 0;
	//header.RD = 0;
	header->RCODE =0;
	header->QDCOUNT = htons(1);
	header->reserved_Z = 0;


	question->QTYPE = htons(28);
	question->QCLASS =htons(1);

	request->header = header;
	request->query = question;
	return request;
}


uint16_t QueryProcessor::getRandomId(){
	return rand();
}


char* QueryProcessor::getDnsNameFormat(char* hostAddress){
	  string hostParts[255];
	  int i=0;
      char * pch;
      pch= strtok (hostAddress,".");
      hostParts[i] = string(pch);
      i++;
	  while (pch != NULL)
	  {
	    pch = strtok (NULL, ".");
	    if(pch != NULL){
	    	hostParts[i] = string(pch);
	    	i++;
	    }
	  }
}




