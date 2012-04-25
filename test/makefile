EXECUTABLE = dns_client
DEPENDENCY = dns_client.o udp_client.o response_reader.o resolver.o
CC = g++
CCFLAGS= -Wall -c -g
OBJ_DIR=obj
SRC_DIR=src
EXE_DIR=build

$(EXECUTABLE) : $(DEPENDENCY)
	$(CC) $(DEPENDENCY) -o $(EXECUTABLE)

resolver.o : ./resolver/resolver.cc ../$(SRC_DIR)/common/resolver.h
	$(CC) $(CCFLAGS) ./resolver/resolver.cc -o resolver.o
response_reader.o : ../src/io/response_reader.cc ../src/common/response_reader.h
	$(CC) $(CCFLAGS) ../src/io/response_reader.cc -o response_reader.o
udp_client.o : ../src/io/udpclient.cc ../src/common/io.h
	$(CC) $(CCFLAGS) ../src/io/udpclient.cc -o udp_client.o
dns_client.o : ./io/udpclient_test.cc ../$(SRC_DIR)/common/io.h
	$(CC) $(CCFLAGS) ./io/udpclient_test.cc -o dns_client.o
	
clean:
	rm -f *.o;rm -f dns_client;	