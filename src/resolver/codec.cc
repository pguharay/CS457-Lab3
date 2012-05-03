#include "../common/codec.h"
#include "../common/util.h"
using namespace std;

const string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

string encode(char* rawBinaryData, unsigned int dataLength)
{
  string encodedString;
  int i = 0;
  int j = 0;
  unsigned char binaryBlock[3];
  unsigned char asciiBlock[4];

  while (dataLength--)
  {
	binaryBlock[i++] = *(rawBinaryData++);

    if (i == 3)
    {
    	mapBinaryToAscii(binaryBlock, asciiBlock);

		for(i = 0; (i <4) ; i++)
		{
		  encodedString.push_back(base64_chars[asciiBlock[i]]);
		}
		i = 0;

    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
    {
    	binaryBlock[j] = '\0';
    }

    mapBinaryToAscii(binaryBlock, asciiBlock);

    for (j = 0; j < i + 1; j++)
    {
    	encodedString.push_back(base64_chars[asciiBlock[j]]);
    }

    while((i++ < 3))
    {
    	encodedString += '=';
    }
  }

  return encodedString;
}

void mapBinaryToAscii(unsigned char* binaryBlock,  unsigned char* asciiBlock)
{
	*(asciiBlock) = (*(binaryBlock) & 0xfc) >> 2;
	*(asciiBlock + 1) = ((*(binaryBlock) & 0x03) << 4) + ((*(binaryBlock + 1) & 0xf0) >> 4);
	*(asciiBlock + 2) = ((*(binaryBlock +1) & 0x0f) << 2) + ((*(binaryBlock + 2) & 0xc0) >> 6);
	*(asciiBlock+ 3) = *(binaryBlock + 2) & 0x3f;
}

