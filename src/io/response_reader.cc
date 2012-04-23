#include "../common/response_reader.h"

Response ResponseReader::read(char* response)
{
	Response message;

	memcpy(&message.header, response, sizeof(Header));

	int i=0;

	while(*(response + sizeof(Header) + i) != '\0')
	{
		message.QNAME[i] = *((response + sizeof(Header) + i));
		i++;
	}

	message.QNAME[i] = '\0';

	memcpy(&message.query, (response + sizeof(Header) + i+1), sizeof(Question));

	offset = sizeof(Header) + i + 1 + sizeof(Question) + 1;

	for(int i=0;i<ntohs(message.header.ANCOUNT);i++)
	{
		//info("answer \n");
	}

	readAuthoritativeAnswer(response, &message);
	readAdditionalAnswer(response, &message);

	return message;
}

void ResponseReader::readAuthoritativeAnswer(char* response, Response* message)
{
	for(int i=0;i<ntohs(message->header.ARCOUNT);i++)
	{
		int j=0;

		while(*(response + offset) != '\0')
		{
			message->authorityRR[i].NAME[j] = *(response + offset);
			offset += 1;
			j++;
		}

		message->authorityRR[i].NAME[j] = '\0';

		memcpy(&(message->authorityRR[i].info), (response + offset), sizeof(RR_Info));
		offset += sizeof(RR_Info);

		for(j=0;j<ntohs(message->authorityRR[i].info.RDLENGTH);j++)
		{
			message->authorityRR[i].RDATA[j] = *(response + offset);
			offset++;
		}

		message->authorityRR[i].RDATA[j] = '\0';
		offset += 1;
	}
}

void ResponseReader::readAdditionalAnswer(char* response, Response* message)
{
	for(int i=0;i<ntohs(message->header.NSCOUNT);i++)
	{
		int j=0;

		while(*(response + offset) != '\0')
		{
			message->additionalRR[i].NAME[j] = *(response + offset);
			offset += 1;
			j++;
		}

		message->additionalRR[i].NAME[j] = '\0';

		memcpy(&(message->additionalRR[i].info), (response + offset), sizeof(RR_Info));
		offset += sizeof(RR_Info);

		for(j=0;j<ntohs(message->additionalRR[i].info.RDLENGTH);j++)
		{
			message->additionalRR[i].RDATA[j] = *(response + offset);
			offset++;
		}

		message->additionalRR[i].RDATA[j] = '\0';
		offset += 1;
	}
}





