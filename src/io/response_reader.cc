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

	offset = sizeof(Header) + i + 1 + sizeof(Question);

	readAnswer(response, &message);
	readAuthoritativeAnswer(response, &message);
	readAdditionalAnswer(response, &message);

	return message;
}

char* ResponseReader::readName(char* response)
{
	bool foundPointer = false;
	bool ranOnce = false;
	int i=0;
	char name[32];

	while(*(response + offset) != 0)
	{
		unsigned char testChar = *(response + offset);
		int testValue = testChar;
		if (testValue >= 192)
		{
			foundPointer = true;
			char *ptrName = readPointer(response, offset);
			int j=0;
			if (i==0)
			{
				// Skip Start Of Text character if this is the first character in the name
				j=1;
			}
			while(*(ptrName + j) != '\0')
			{
				name[i++] = *(ptrName + j);
				j++;
			}
			offset += 2;
			break;
		}
		else if (ranOnce) // Skip Start Of Text character if this is the first character in the name
		{
			if (testValue == 3 || testValue == 2)
			{
				name[i++] = '.';
			}
			else
			{
				name[i++] = (*(response + offset));
			}
		}
		offset += 1;
		ranOnce = true;
	}
	name[i] = '\0';

	// if it got to the last char without a pointer in the name
	if(!foundPointer)
	{
		offset += 1;
	}

	return name;
}

char* ResponseReader::readPointer(char* response, int offsetPointer)
{
	int pointer = 0;
	unsigned char first = *(response + offsetPointer);
	int firstValue = first;
	unsigned char second = *(response + offsetPointer + 1);
	int secondValue = second;
	pointer = (firstValue - 192)*256 + secondValue;

	char name[32];
	int i = 0;
	while (*(response + pointer) != '\0')
	{
		unsigned char testChar = *(response + pointer);
		int testValue = testChar;
		if (testValue >= 192)
		{
			char *suffix = this->readPointer(response, pointer);
			int j = 0;
			while (*(suffix + j) != '\0')
			{
				name[i++] = (*(suffix + j));
				j++;
			}
			name[i++] = '\0';
			break;
		}
		else if (testValue == 3 || testValue == 2)
		{
			name[i++] = '.';
		}
		else
		{
			name[i++] = (*(response + pointer));
		}
		pointer += 1;
	}
	if (*(response + pointer) == 0)
	{
		name[i++] = '\0';
	}

	return name;
}

void ResponseReader::readAnswer(char* response, Response* message)
{
	for(int i=0;i<ntohs(message->header.ANCOUNT);i++)
	{
		int j=0;
		char *dname = readName(response);
		while(*(dname + j) != '\0')
		{
			message->answerRR[i].NAME[j] = *(dname + j);
			j++;
		}
		message->answerRR[i].NAME[j] = '\0';

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
	}
}

void ResponseReader::readAuthoritativeAnswer(char* response, Response* message)
{
	for(int i=0;i<ntohs(message->header.NSCOUNT);i++)
	{
		int j=0;
		char *dname = readName(response);
		while(*(dname + j) != '\0')
		{
			message->authorityRR[i].NAME[j] = *(dname + j);
			j++;
		}
		message->authorityRR[i].NAME[j] = '\0';

		memcpy(&(message->authorityRR[i].info), (response + offset), sizeof(RR_Info));

		offset += sizeof(RR_Info);

		// TEST - NS Server Names from RDATA
//		if (ntohs(message->authorityRR[i].info.TYPE) == 2)
//		{
//			int savedOffset = offset;
//			char *rname = readName(response);
//
//			printf("RNAME IS %s\n", rname);
//			offset = savedOffset;
//		}

		for(j=0;j<ntohs(message->authorityRR[i].info.RDLENGTH);j++)
		{
			message->authorityRR[i].RDATA[j] = *(response + offset);
			offset++;
		}

		message->authorityRR[i].RDATA[j] = '\0';
	}
}

void ResponseReader::readAdditionalAnswer(char* response, Response* message)
{
	for(int i=0;i<ntohs(message->header.ARCOUNT);i++)
	{
		int j=0;
		char *dname = readName(response);
		while(*(dname + j) != '\0')
		{
			message->additionalRR[i].NAME[j] = *(dname + j);
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
	}
}





