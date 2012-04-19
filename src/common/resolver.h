#include <iostream>

using namespace std;

#ifndef RESOLVER_H_
#define RESOLVER_H_

typedef struct __attribute__((packed)) request_header
{
	unsigned	short ID; 			 // A 16 bit identifier assigned by the program that generates any kind of query

	unsigned	char  QR :4;		  //A one bit field that specifies whether this message is a query
	unsigned	char  OPCODE :4;      // A four bit field that specifies kind of query in this message
	unsigned	char  AA :1;         //Authoritative Answer
	unsigned	char  TC :1;		 //TrunCation
	unsigned	char  RD :1;		 //Recursion Desired
	unsigned	char  RA :1;		  //Recursion Available
	unsigned	char  reserved_Z :3;  //Reserved for future use

	unsigned	char  RCODE :4;		//Response code

	unsigned    short QDCOUNT;		//an unsigned 16 bit integer specifying the number of entries in the question section
	unsigned	short ANCOUNT;		//an unsigned 16 bit integer specifying the number of resource records in the answer section
	unsigned	short ARCOUNT;		//an unsigned 16 bit integer specifying the number of name server resource records in the authority records section
	unsigned	short NSCOUNT;		//an unsigned 16 bit integer specifying the number of resource records in the additional records section
}Header;

typedef struct __attribute__((packed)) query
{
	string 			QNAME;
	unsigned short  QTYPE;
	unsigned short 	QCLASS;
}Question;

typedef struct __attribute__((packed)) resolver_request
{
	Header* 	header;
	Question* 	query;
}Request;

typedef struct __attribute__((packed)) resource_record
{
	string 			NAME;
	unsigned short  TYPE;
	unsigned short  CLASS;
	unsigned int    TTL;
	unsigned short  RDLENGTH;
	string			RDATA;
}RR;

typedef struct __attribute__((packed)) resolver_response
{
	struct RR* record;
}Response;

class Resolver
{
	public:
		Resolver(string rootServer[5]);
		Response queryNameServer(string nameserver, Request req);
};

#endif
