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

void UDPClient::sendRequest(Request request)
{
	int totalBytesSend = 0;

	socklen_t addrlen = sizeof(struct sockaddr);

	while (totalBytesSend != sizeof(request))
	{
		int bytes = sendto(socketID, (void*) &request, sizeof(request), 0, serverAddress, addrlen);

		totalBytesSend += bytes;
	}

	debug("Total %d bytes send \n", totalBytesSend);
}

Response UDPClient::receiveResponse()
{
	Response response;
	socklen_t addrlen = sizeof(struct sockaddr);
	int status = poll(&poller, 1, 3000);
	int bytes = -1;

	if (status < 0)
	{
		error("Unable to poll. \n");
		exit(1);
	}
	else if (status == 0)
	{
		error("Timeout occurred. \n");
		exit(1);
	}
	else
	{
		do
		{
			bytes = recvfrom(socketID, &response, sizeof(response), 0, serverAddress, &addrlen);
			debug("Received %u bytes from server \n", bytes);
		}
		while(bytes >= 0);
	}

	return response;
}
