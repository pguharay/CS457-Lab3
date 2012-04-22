/*
 * query_processor.cc
 *
 *  Created on: Mar 31, 2012
 *      Author: jon
 */
#include "../common/io.h"
#include "../common/dns.h"
#include "../common/query_processor.h"


QueryProcessor::QueryProcessor() {}


Message* QueryProcessor::getDnsQuery(char* hostAddress){
	Message* request = (Message*)malloc(sizeof(Message));
	Header header;
	Question question;

	header.ID = getRandomId();
	header.QR = 0;
	//header.RD = 0;
	header.RCODE =0;
	header.QDCOUNT = htons(1);
	header.reserved_1 = 0;


	question.QTYPE = htons(28);
	question.QCLASS =htons(1);

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
      pch= strtok(hostAddress,".");
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

	  return NULL;
}






