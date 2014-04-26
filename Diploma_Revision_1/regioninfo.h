#ifndef REGIONINFO
#define REGIONINFO

#include "blockinfo.h"

struct RegionInfo
{
	typedef std::vector<BlockInfo*> VectorOfBlockInfos;
	VectorOfBlockInfos blockInfos;

	size_t size;
	long regionStorage;
	long regionProtection;
	size_t blockCount;
	std::string description;

	void* allocationBase;

	RegionInfo()
		: size(0)
		, regionProtection(0)
		, regionStorage(0)
		, blockCount(0)
		, description("")
		, allocationBase(NULL)
	{ }

	void print()
	{
		std::fstream fs;
		fs.open("memoryMap.txt", std::fstream::in | std::fstream::out | std::fstream::app);

		std::string storageText, protectionText;

		switch (regionStorage)
		{
		case MEM_FREE:    storageText = "Free   "; break;
		case MEM_RESERVE: storageText = "Reserve"; break;
		case MEM_IMAGE:   storageText = "Image  "; break;
		case MEM_MAPPED:  storageText = "Mapped "; break;
		case MEM_PRIVATE: storageText = "Private"; break;

		default:          storageText = "Unknown";
		}

		if (regionStorage != MEM_FREE)
		{
			switch (regionProtection & ~(PAGE_GUARD | PAGE_NOCACHE | PAGE_WRITECOMBINE))
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

			protectionText += (regionProtection & PAGE_GUARD) ? "G" : "-";
			protectionText += (regionProtection & PAGE_NOCACHE) ? "N" : "-";
			protectionText += (regionProtection & PAGE_WRITECOMBINE) ? "W" : "-";
		}
		else
			protectionText = "";

		fs << std::hex << std::setw(10) << allocationBase << "       ";
		fs << std::setw(10) << storageText << "   ";
		fs << std::dec << std::setw(10) << size << "   ";
		fs << std::setw(10) << protectionText << "   " << std::endl;

		for (size_t i = 0; i < blockInfos.size(); ++i)
		{
			blockInfos[i]->print();
		}

		fs.close();
	}

};

#endif