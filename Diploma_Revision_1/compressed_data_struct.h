#ifndef COMPREDDESDATA
#define COMPRESSEDDATA

#include <vector>

typedef union
{
	struct
	{
		unsigned char bit1 : 1;
		unsigned char bit2 : 1;
		unsigned char bit3 : 1;
		unsigned char bit4 : 1;
		unsigned char bit5 : 1;
		unsigned char bit6 : 1;
		unsigned char bit7 : 1;
		unsigned char bit8 : 1;
	}Bits;
	char memory;
}Bite;

struct CompressedData
{
	std::vector<char> notEqualToMaxElement_In_4096_Bytes;
	char maxElement;

	std::vector<char> notNullElements_In_512_Bytes;
	std::vector<char> notNullElements_In_64_Bytes;

	Bite map[8];
};


#endif
