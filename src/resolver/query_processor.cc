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
	  char * hostParts[255];
      char * pch;
      char * qName;
      char * intBuf;
      int i=0;

      pch= strtok(hostAddress,".");
      hostParts[i] = pch;
      i++;
	  while (pch != 0x0)
	  {
	    pch = strtok (NULL, ".");
	    if(pch != 0x0){
	    	hostParts[i] = pch;
	    	i++;
	    }
	  }

	  for(int j=0;j<=255;j++){
		  //get the length first...
		  int partLen = len(hostParts[0]);

	  }
}






