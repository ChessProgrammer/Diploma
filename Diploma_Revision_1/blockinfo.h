#ifndef BLOCKINFO
#define BLOCKINFO

#include <Windows.h>
#include <fstream>
#include <iomanip>
#include <string>

#include "compressed_data_struct.h"

const size_t SIZE_OF_PAGE = 4096;

// information about block
struct BlockInfo
{
	void* baseAddress;
	size_t size;
	DWORD protection;
	long storage;

	char* data;
	std::vector<CompressedData*> compressedData;

	BlockInfo()
		: baseAddress(NULL)
		, size(0)
		, protection(0)
		, storage(0)
		, data(NULL)
	{ }

	void print()
	{
		std::fstream fs;
		fs.open("memoryMap.txt", std::fstream::in | std::fstream::out | std::fstream::app);

		std::string storageText, protectionText;

		switch (storage)
		{
		case MEM_FREE:    storageText = "Free   "; break;
		case MEM_RESERVE: storageText = "Reserve"; break;
		case MEM_IMAGE:   storageText = "Image  "; break;
		case MEM_MAPPED:  storageText = "Mapped "; break;
		case MEM_PRIVATE: storageText = "Private"; break;

		default:          storageText = "Unknown";
		}

		if (storage != MEM_FREE)
		{
			switch (protection & ~(PAGE_GUARD | PAGE_NOCACHE | PAGE_WRITECOMBINE))
			{
			case PAGE_READONLY:          protectionText = "-R--"; break;
			case PAGE_READWRITE:         protectionText = "-RW-"; break;
			case PAGE_WRITECOPY:         protectionText = "-RWC"; break;
			case PAGE_EXECUTE:           protectionText = "E---"; break;
			case PAGE_EXECUTE_READ:      protectionText = "ER--"; break;
			case PAGE_EXECUTE_READWRITE: protectionText = "ERW-"; break;
			case PAGE_EXECUTE_WRITECOPY: protectionText = "ERWC"; break;
			case PAGE_NOACCESS:          protectionText = "----"; break;

			default:                     protectionText = "----";
			}
			protectionText += "  ";

			protectionText += (protection & PAGE_GUARD) ? "G" : "-";
			protectionText += (protection & PAGE_NOCACHE) ? "N" : "-";
			protectionText += (protection & PAGE_WRITECOMBINE) ? "W" : "-";
		}
		else
			protectionText = "";



		fs << "    " << std::hex << std::setw(10) << baseAddress << "   ";
		fs << std::setw(10) << storageText << "   ";
		fs << std::dec << std::setw(10) << size << "   ";
		fs << std::setw(10) << protectionText << "   " << std::endl;

		fs.close();

	}
};

#endif
