#include "../common/response_reader.h"
#include "../common/util.h"

Response ResponseReader::read(char* response)
{
	Response message;
	// Populate Header data
	memcpy(&message.header, response, sizeof(Header));

	// Populate Question data
	int i=0;
	while(*(response + sizeof(Header) + i) != '\0')
	{
		message.QNAME[i] = *((response + sizeof(Header) + i));
		i++;
	}

	message.QNAME[i] = '\0';

	memcpy(&message.query, (response + sizeof(Header) + i+1), sizeof(Question));

	offset = sizeof(Header) + i + 1 + sizeof(Question) + 1;

	readAnswer(response, &message);
	readAuthoritativeAnswer(response, &message);
	readAdditionalAnswer(response, &message);

	return message;
}

void ResponseReader::readAnswer(char* response, Response* message)
{
	for(int i=0;i<ntohs(message->header.ANCOUNT);i++)
	{
		int j=0;

		while(*(response + offset) != 0)
		{
			message->answerRR[i].NAME[j] = *(response + offset);
			offset += 1;
			j++;
		}

		unsigned char jump = message->answerRR[i].NAME[j-1];
		int value = jump;
		string data;

		j=j-1;

		while(*(response + value) != 0)
		{
			message->answerRR[i].NAME[j++] = (*(response + value));
			value++;
		}

		message->answerRR[i].NAME[j] = '\0';
		//offset += 1;

		memcpy(&(message->answerRR[i].info), (response + offset), sizeof(RR_Info));
		offset += sizeof(RR_Info);

		for(j=0;j<ntohs(message->answerRR[i].info.RDLENGTH);j++)
		{
			message->answerRR[i].RDATA[j] = *(response + offset);
			offset++;
		}

		if(ntohs(message->answerRR[i].info.TYPE) == 5)
		{
			unsigned char jump = message->answerRR[i].RDATA[j-1];
			int value = jump;

			string data;

			j=j-2;

			while(*(response + value) != 0)
			{
				message->answerRR[i].RDATA[j++] = (*(response + value));
				value++;
			}
		}

		message->answerRR[i].RDATA[j] = '\0';
		offset += 1;
	}
}

void ResponseReader::readAuthoritativeAnswer(char* response, Response* message)
{
	for(int i=0;i<ntohs(message->header.NSCOUNT);i++)
	{
		int j=0;

		while(*(response + offset) != '\0')
		{
			message->authorityRR[i].NAME[j++] = *(response + offset);
			offset += 1;
		}

		message->authorityRR[i].NAME[j] = '\0';

		debug("NAME = %s \n", message->authorityRR[i].NAME);

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
	for(int i=0;i<ntohs(message->header.ARCOUNT);i++)
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





