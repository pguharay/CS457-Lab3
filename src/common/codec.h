#include <iostream>

using namespace std;

#ifndef CODEC_H_
#define CODEC_H_

string encode(unsigned char const* rawBinaryData, unsigned int dataLength);
void mapBinaryToAscii(unsigned char* binaryBlock,  unsigned char* asciiBlock);

#endif
