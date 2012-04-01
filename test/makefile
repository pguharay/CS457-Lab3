EXECUTABLE = dns_client
DEPENDENCY = dns_client.o udp_client.o
CC = g++
CCFLAGS= -Wall -c -g
OBJ_DIR=obj
SRC_DIR=src
EXE_DIR=build

$(EXECUTABLE) : $(DEPENDENCY)
	$(CC) $(DEPENDENCY) -o $(EXECUTABLE)

udp_client.o : ../src/io/udpclient.cc ../src/common/io.h
	$(CC) $(CCFLAGS) ../src/io/udpclient.cc -o udp_client.o
dns_client.o : ./io/udpclient_test.cc ../$(SRC_DIR)/common/io.h
	$(CC) $(CCFLAGS) ./io/udpclient_test.cc -o dns_client.o