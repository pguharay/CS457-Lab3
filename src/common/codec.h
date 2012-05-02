#include <iostream>

using namespace std;

#ifndef CODEC_H_
#define CODEC_H_

string encode(char* rawBinaryData, unsigned int dataLength);
void mapBinaryToAscii(unsigned char* binaryBlock,  unsigned char* asciiBlock);

#endif
