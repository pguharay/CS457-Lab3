EXECUTABLE = query_processor_test #dns_client
DEPENDENCY = query_processor_test.o #dns_client.o 
CC = g++
CCFLAGS= -Wall -c -g
OBJ_DIR=obj
SRC_DIR=src
EXE_DIR=build

$(EXECUTABLE) : $(DEPENDENCY)
	$(CC) $(DEPENDENCY) -o $(EXECUTABLE)
	
#dns_client.o : ./io/udpclient_test.cc ../$(SRC_DIR)/common/io.h
#	$(CC) $(CCFLAGS) ./io/udpclient_test.cc -o dns_client.o
	
query_processor_test.o : ./resolver/query_processor_test.cc ../$(SRC_DIR)/common/query_processor.h
	$(CC) $(CCFLAGS) ./resolver/query_processor_test.cc -o query_processor_test.o