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


Request QueryProcessor::getDnsQuery(string hostAddress){
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

}


uint16_t QueryProcessor::getRandomId(){
	return rand();
}


const string QueryProcessor::getDnsNameFormat(const string hostAddress){



}






