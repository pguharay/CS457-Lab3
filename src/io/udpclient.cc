#include "../common/io.h"

UDPClient::UDPClient(string serverHost)
{
	memset(&serverAddressQuery, 0x0000, sizeof(serverAddressQuery));
	serverAddressQuery.ai_family = AF_INET;

	serverAddressQuery.ai_socktype = SOCK_DGRAM;

	int status = getaddrinfo(serverHost.c_str(), DNS_PORT, &serverAddressQuery, &serverAddressResultList);

	if (status != SUCCESS)
	{
		error("Unable to get server host information \n");
		exit(1);
	}

	serverAddress = serverAddressResultList->ai_addr;

	socketID = socket(serverAddressResultList->ai_family, serverAddressResultList->ai_socktype, 0);

	if (socket < 0)
	{
		perror("Unable to create socket \n");
		exit(1);
	}

	status = connect(socketID, serverAddressResultList->ai_addr, serverAddressResultList->ai_addrlen);

	if (status == FAILURE)
	{
		perror("unable to connect to server");
		throw "Unable to reach server";
	}

	poller.fd = socketID;
	poller.events = POLLIN;
}

void UDPClient::sendRequest(Message message)
{
	int totalBytesSend = 0;

	socklen_t addrlen = sizeof(struct sockaddr);

	createNetworkDataFromMessage(message);

	while (totalBytesSend != sizeof(message))
	{
		int bytes = sendto(socketID, buffer, sizeof(message), 0, serverAddress, addrlen);

		totalBytesSend += bytes;
	}

	debug("Total %d bytes send \n", totalBytesSend);
}

void UDPClient :: createNetworkDataFromMessage(Message message)
{
	memcpy(&buffer,&message.header, sizeof(Header));

	char* qname = &message.QNAME[0];

	int i=0;

	while(* (qname + i) != '\0')
	{
		buffer[sizeof(Header) + i] = *(qname + i);
		i++;
	}

	buffer[sizeof(Header) + i] = '\0';

	memcpy(&buffer[sizeof(Header) + i+1], &message.query, sizeof(Question));
}

Response UDPClient::receiveResponse(ResponseReader* responseReader)
{
	char  buffer[65536];
	//Message message;
	socklen_t addrlen = sizeof(struct sockaddr);
	int status = poll(&poller, 1, 3000);
	int bytes = -1;

	if (status < 0)
	{
		error("Unable to poll. \n");
		throw FAILURE;
	}
	else if (status == 0)
	{
		error("Timeout occurred. \n");
		throw FAILURE;
	}
	else
	{
		bytes = recvfrom(socketID, buffer, 65536, 0, serverAddress, &addrlen);

		if(bytes < 0)
		{
			perror("Not enough bytes in response");
			throw FAILURE;
		}

		debug("Received %u bytes from server \n", bytes);
	}

	return formatResponseToMessage(buffer, responseReader);
}

Response UDPClient::formatResponseToMessage(char* response, ResponseReader* responseReader)
{
	return responseReader->read(response);
}
