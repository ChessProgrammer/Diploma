#ifndef COMPREDDESDATA
#define COMPRESSEDDATA

#include <vector>

union Bite
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

	Bite()
	{
		memory = 0;
		Bits.bit1 = 0;
		Bits.bit2 = 0;
		Bits.bit3 = 0;
		Bits.bit4 = 0;
		Bits.bit5 = 0;
		Bits.bit6 = 0;
		Bits.bit7 = 0;
		Bits.bit8 = 0;
	}

	void set(size_t i)
	{
		switch(i)
		{
		case 1:
			Bits.bit1 = 1;
		case 2:
			Bits.bit2 = 1;
		case 3:
			Bits.bit3 = 1;
		case 4:
			Bits.bit4 = 1;
		case 5:
			Bits.bit5 = 1;
		case 6:
			Bits.bit6 = 1;
		case 7:
			Bits.bit7 = 1;
		case 8:
			Bits.bit8 = 1;
		}
	}
};

struct CompressedPage
{
	std::vector<char> notEqualToMaxElement_In_4096_Bytes;
	char maxElement;

	std::vector<char> notNullElements_In_512_Bytes;
	std::vector<char> notNullElements_In_64_Bytes;

	Bite map[8];
};


#endif
