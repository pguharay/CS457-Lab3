#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <string>
#include <fstream>

using namespace std;

#ifndef DNS_H_
#define DNS_H_

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

typedef struct __attribute__((packed)) dnssec_rr
{
	unsigned short flags;
	unsigned char  protocol;
	unsigned char  algorithm;
	string 		   public_key;
}DNSSEC;

typedef struct __attribute__((packed)) rrsig_rr
{
	unsigned short  type_covered;
	unsigned char   algorithm;
	unsigned char   labels;
	uint32_t 		original_TTL;
	uint32_t 		signature_expiration;
	uint32_t 		signature_inception;
	uint16_t        key_tag;
	string 			signer;
	string 			signature;
}RRSIG;


#endif
