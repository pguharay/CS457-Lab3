/*
 * query_processor.cc
 *
 *  Created on: Mar 31, 2012
 *      Author: jon
 */
#include <string.h>
#include <ctype.h>
#include "../common/io.h"
#include "../common/dns.h"
#include "../common/query_processor.h"


QueryProcessor::QueryProcessor() {}




Message QueryProcessor::getDnsQuery(char* hostAddress)
{
	validateHostname(hostAddress);
	Message request;

	Header header = createMessageHeader();
	Question question = createQuestion();
	string qname = createQName(hostAddress);

	request.header = header;

	memcpy(request.QNAME, qname.c_str(), qname.length()+1);
	request.QNAME[qname.length() + 1] = '\0';
	request.query = question;

	return request;
}

Question QueryProcessor::createQuestion()
{
	Question question;
	question.QTYPE = htons(255);  //rm:  AAAA = 28, ANY = 255
	question.QCLASS = htons(1);

	return question;
}

string QueryProcessor::createQName(string domainName)
{
	return formatDNSName(domainName);
}

Header QueryProcessor::createMessageHeader()
{
	uint16_t randomID = time(NULL) % 100;

	Header header;
	header.ID = htons(randomID);
	header.QR = 0;
	header.OPCODE = 0;
	header.AA = 0;
	header.TC = 0;
	header.RD = htons(0);  //rm: should not be recursive per assignment directions
	header.RA = 0;
	header.reserved_1  = 0;
	header.reserved_2  = 0;
	header.reserved_3  = 0;
	header.RCODE = 0;
	header.QDCOUNT = htons(1);
	header.ANCOUNT  = 0;
	header.ARCOUNT = 0;
	header.NSCOUNT  = 0;


	return header;
}

string QueryProcessor::formatDNSName(string domainName)
{
	string dnsformat;
	int startIndex = 0;
	int i=0;
	char length;

	for(;i < static_cast<int>(domainName.size());i++)
	{
		if(*(domainName.c_str() +i)  == '.')
		{
			length = i-startIndex;
			dnsformat.push_back(length);
			dnsformat.append(domainName.substr(startIndex, i-startIndex));
			startIndex = i+1;
		}
	}

	length = i-startIndex;
	dnsformat.push_back(length);
	dnsformat.append(domainName.substr(startIndex, length));

	return dnsformat;
}


void QueryProcessor::validateHostname(char* domainName){
	string sName = string(domainName);

	//check if the size is greater then 128
	if(sName.size() > 128){
		perror("Invalid Hostname. Hostname too big.");
		exit(0);
	}

	//check if the host has at least 2 periods...
	int period_count=0;
	bool isInvalid=false;
	for(uint i=0; i < sName.size(); i++){
		if(sName[i] == '.'){
			period_count++;
		}else{
			//check for invalid character...
			if(!isalnum(sName[i])){
				if(sName[i] != '-'){
					isInvalid=true;
				}
			}
		}
	}

	if(period_count < 2 || isInvalid==true){
		perror("Invalid hostname.");
		exit(0);
	}


}

