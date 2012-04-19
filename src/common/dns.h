#include <iostream>
#include <stdio.h>
#include <stdint.h>

#ifndef DNS_H_
#define DNS_H_

using namespace std;

typedef struct __attribute__((packed)) request_header
{
	uint16_t 		  ID; 			 // A 16 bit identifier assigned by the program that generates any kind of query

	unsigned	char  QR :4;		  //A one bit field that specifies whether this message is a query
	unsigned	char  OPCODE :4;      // A four bit field that specifies kind of query in this message
	unsigned	char  AA :1;         //Authoritative Answer
	unsigned	char  TC :1;		 //TrunCation
	unsigned	char  RD :1;		 //Recursion Desired
	unsigned	char  RA :1;		  //Recursion Available
	unsigned	char  reserved_Z :3;  //Reserved for future use

	unsigned	char  RCODE :4;		//Response code

	uint16_t 		  QDCOUNT;		//an unsigned 16 bit integer specifying the number of entries in the question section
	uint16_t 		  ANCOUNT;		//an unsigned 16 bit integer specifying the number of resource records in the answer section
	uint16_t 		  ARCOUNT;		//an unsigned 16 bit integer specifying the number of name server resource records in the authority records section
	uint16_t 		  NSCOUNT;		//an unsigned 16 bit integer specifying the number of resource records in the additional records section
}Header;

typedef struct __attribute__((packed)) query
{
	string 			QNAME;
	uint16_t  		QTYPE;
	uint16_t 		QCLASS;
}Question;

typedef struct __attribute__((packed)) resolver_request
{
	Header* 		header;
	Question* 		query;
}Request;

typedef struct __attribute__((packed)) resource_record
{
	string 			NAME;
	uint16_t  		TYPE;
	uint16_t  		CLASS;
	uint16_t   		TTL;
	uint16_t  		RDLENGTH;
	string			RDATA;
}RR;

typedef struct __attribute__((packed)) resolver_response
{
	struct RR* record;
}Response;

typedef struct __attribute__((packed)) dnssec_rr
{
	uint16_t 	flags;
	uint8_t  	protocol;
	uint8_t  	algorithm;
	string 		public_key;
}DNSSEC;

typedef struct __attribute__((packed)) rrsig_rr
{
	uint16_t 		type_covered;
	uint8_t   		algorithm;
	uint8_t   		labels;
	uint32_t 		original_TTL;
	uint32_t 		signature_expiration;
	uint32_t 		signature_inception;
	uint16_t        key_tag;
	string 			signer;
	string 			signature;
}RRSIG;


#endif
