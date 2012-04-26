#include <iostream>
#include <stdio.h>
#include <stdint.h>

#ifndef DNS_H_
#define DNS_H_

using namespace std;

typedef struct __attribute__((packed)) request_header
{
	uint16_t 		  ID; 			  // A 16 bit identifier assigned by the program that generates any kind of query

	unsigned	char  RD 	:1;		  //Recursion Desired
	unsigned	char  TC 	:1;		  //Truncation
	unsigned	char  AA 	:1;       //Authoritative Answer
	unsigned	char  OPCODE:4;       // A four bit field that specifies kind of query in this message
	unsigned	char  QR	:1;		  //A one bit field that specifies whether this message is a query
	unsigned	char  RCODE :4;		  //Response code
	unsigned	char  reserved_1 :1;  //Reserved for future use
	unsigned	char  reserved_2 :1;  //Reserved for future use
	unsigned	char  reserved_3 :1;  //Reserved for future use
	unsigned	char  RA :1;		  //Recursion Available

	uint16_t 		  QDCOUNT;		//an unsigned 16 bit integer specifying the number of entries in the question section
	uint16_t 		  ANCOUNT;		//an unsigned 16 bit integer specifying the number of resource records in the answer section
	uint16_t 		  NSCOUNT;		//an unsigned 16 bit integer specifying the number of name server resource records in the authority records section
	uint16_t 		  ARCOUNT;		//an unsigned 16 bit integer specifying the number of resource records in the additional records section
}Header;

typedef struct __attribute__((packed)) query
{
	uint16_t  		QTYPE;
	uint16_t 		QCLASS;
}Question;

typedef struct __attribute__((packed)) resource_record_info
{
	uint16_t  		TYPE;
	uint16_t  		CLASS;
	uint32_t   		TTL;
	uint16_t  		RDLENGTH;
}RR_Info;

typedef struct __attribute__((packed)) resource_record
{
	char 			NAME[32];
	RR_Info			info;
	char			RDATA[1024];
}RR;

typedef struct __attribute__((packed)) resolver_request
{
	Header 		header;
	char		QNAME[128];
	Question	query;
}Message;


typedef struct __attribute__((packed)) resolver_response
{
	Header 		header;
	char		QNAME[128];
	Question	query;
	RR 			answerRR[20];
	RR			authorityRR[20];
	RR			additionalRR[20];
}Response;

typedef struct __attribute__((packed)) dnssec_rr
{
	uint16_t 	flags;
	uint8_t  	protocol;
	uint8_t  	algorithm;
	char 		public_key[256];
}DNSSEC;

typedef struct __attribute__((packed)) rrsig_fixed_size_field
{
	uint16_t 		type_covered;
	uint8_t   		algorithm;
	uint8_t   		labels;
	uint32_t 		original_TTL;
	uint32_t 		signature_expiration;
	uint32_t 		signature_inception;
	uint16_t        key_tag;
}RRSIG_Fixed;

typedef struct __attribute__((packed)) rrsig_rr
{
	RRSIG_Fixed fixed_length_data;
	char 			signer[64];
	char 			signature[128];
}RRSIG;


#endif
