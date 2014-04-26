#include <iostream>
#include <map>

#include "addressspaceinfo.h"

AddressSpaceInfo::AddressSpaceInfo(long processId)
{
	if ((handleOfProcess_ = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId)) == NULL)
	{
		std::cout << "error in OpenProcess function" << std::endl;
		exit(1);
	}
}

bool AddressSpaceInfo::processRegion(void* address, RegionInfo* regionInfo)
{
	const void* baseAddress = address;
	void* currentAddress = address;

	regionInfo->size = 0;
	MEMORY_BASIC_INFORMATION mbi;

	bool ok = (VirtualQueryEx(handleOfProcess_, currentAddress, &mbi, sizeof(mbi)) == sizeof(mbi));

	if (!ok)
		return false;
	if (mbi.State == MEM_FREE)
	{
		regionInfo->allocationBase = mbi.BaseAddress;
		regionInfo->size = mbi.RegionSize;
		regionInfo->regionStorage = MEM_FREE;

		return true;
	}
	else
	{
		regionInfo->allocationBase = mbi.AllocationBase;
		regionInfo->regionProtection = mbi.AllocationProtect;
		regionInfo->regionStorage = mbi.Type; // correct
	}

	for (;;)
	{
		bool ok = (VirtualQueryEx(handleOfProcess_, currentAddress, &mbi, sizeof(mbi)) == sizeof(mbi));

		if (!ok)
			return false;
		else if (mbi.State == MEM_FREE || mbi.AllocationBase != baseAddress)
			break;

		BlockInfo* blockInfo = new BlockInfo();

		blockInfo->baseAddress = mbi.BaseAddress;
		blockInfo->size = mbi.RegionSize;
		blockInfo->protection = mbi.Protect;
		blockInfo->storage = (mbi.State == MEM_COMMIT) ? mbi.Type : mbi.State;

		/*if (blockInfo->storage == MEM_PRIVATE && !(blockInfo->protection & PAGE_GUARD))
		{
			blockInfo->source = new char[blockInfo->size];
			bool k = ReadProcessMemory(handleOfProcess_, blockInfo->baseAddress, blockInfo->source, blockInfo->size, 0);
			//std::cout << k << "   " << blockInfo->protection << "    ";
			//std::cout << std::string(blockInfo->source) << std::endl;
			//blockInfo->source[blockInfo->size] = '\0';

			//compressData(*blockInfo);
		}
		else
		{
			blockInfo->source = NULL;
			//blockInfo->compressedData = NULL;
		}*/

		regionInfo->blockInfos.push_back(blockInfo);
		++(regionInfo->blockCount);
		regionInfo->size += mbi.RegionSize;
		currentAddress = (PVOID)((PBYTE)currentAddress + mbi.RegionSize);
	}

	return true;
}


void AddressSpaceInfo::processAllAddressSpace()
{
	PVOID address = NULL;

	while (true)
	{
		RegionInfo* regionInfo = new RegionInfo();
		if (!processRegion(address, regionInfo))
			break;
		vectorOfRegionInfos_.push_back(new RegionInfo(*regionInfo));
		address = (PVOID)((PBYTE)address + regionInfo->size);
	}
}

/*void AddressSpaceInfo::compressData(BlockInfo& blockInfo)
{
	const size_t countOfPages = blockInfo.size / SIZE_OF_PAGE;

	char* ptr = blockInfo.source;
	for (int i = 0; i < countOfPages; i++)
	{
		CompressedData* data = new CompressedData();
		compressPage(ptr, data);
		blockInfo.compressedData.push_back(data);
		ptr = ptr + SIZE_OF_PAGE;
	}
}

void AddressSpaceInfo::compressPage(const char source[SIZE_OF_PAGE], CompressedData* data)
{
	size_t i = 0;
	std::map<char, size_t> map;
	char max;
	size_t count = 0;

	for (int i = 0; i < SIZE_OF_PAGE; ++i)
	{
		map[source[i]] = 0;
	}

	for (int i = 0; i < SIZE_OF_PAGE; ++i)
	{
		++map[source[i]];

		if (map[source[i]] > count)
		{
			max = source[i];
			count++;
		}
	}


	Bite mapOfNotEqualElements[SIZE_OF_PAGE / 8];
	data->maxElement = max;
	for (int i = 0; i < SIZE_OF_PAGE; ++i)
	{
		if (source[i] != max)
		{
			data->notEqualToMaxElement.push_back(source[i]);
			mapOfNotEqualElements[i / 8].Bits.bit1 = 1;
		}
		else
			mapOfNotEqualElements[i / 8].Bits.bit1 = 0;
		++i;
		if (source[i] != max)
		{
			data->notEqualToMaxElement.push_back(source[i]);
			mapOfNotEqualElements[i / 8].Bits.bit2 = 1;
		}
		else
			mapOfNotEqualElements[i / 8].Bits.bit2 = 0;
		++i;
		if (source[i] != max)
		{
			data->notEqualToMaxElement.push_back(source[i]);
			mapOfNotEqualElements[i / 8].Bits.bit3 = 1;
		}
		else
			mapOfNotEqualElements[i / 8].Bits.bit3 = 0;
		++i;
		if (source[i] != max)
		{
			data->notEqualToMaxElement.push_back(source[i]);
			mapOfNotEqualElements[i / 8].Bits.bit4 = 1;
		}
		else
			mapOfNotEqualElements[i / 8].Bits.bit4 = 0;
		++i;
		if (source[i] != max)
		{
			data->notEqualToMaxElement.push_back(source[i]);
			mapOfNotEqualElements[i / 8].Bits.bit5 = 1;
		}
		else
			mapOfNotEqualElements[i / 8].Bits.bit5 = 0;
		++i;
		if (source[i] != max)
		{
			data->notEqualToMaxElement.push_back(source[i]);
			mapOfNotEqualElements[i / 8].Bits.bit6 = 1;
		}
		else
			mapOfNotEqualElements[i / 8].Bits.bit6 = 0;
		++i;
		if (source[i] != max)
		{
			data->notEqualToMaxElement.push_back(source[i]);
			mapOfNotEqualElements[i / 8].Bits.bit7 = 1;
		}
		else
			mapOfNotEqualElements[i / 8].Bits.bit7 = 0;
		++i;
		if (source[i] != max)
		{
			data->notEqualToMaxElement.push_back(source[i]);
			mapOfNotEqualElements[i / 8].Bits.bit8 = 1;
		}
		else
			mapOfNotEqualElements[i / 8].Bits.bit8 = 0;
	}
}

void AddressSpaceInfo::compressData1(Bite source[SIZE_OF_PAGE / 64], CompressedData* data)
{
	Bite mapOfNotNull[SIZE_OF_PAGE / 64];
	for (int i = 0; i < SIZE_OF_PAGE / 8; ++i)
	{
		if (source[i].memory != 0)
		{
			data->notEqualToMaxElement.push_back(source[i].memory);
			mapOfNotNull[i / 8].Bits.bit1 = 1;
		}
		else
			mapOfNotNull[i / 8].Bits.bit1 = 0;
		++i;
		if (source[i].memory != 0)
		{
			data->notEqualToMaxElement.push_back(source[i].memory);
			mapOfNotNull[i / 8].Bits.bit2 = 1;
		}
		else
			mapOfNotNull[i / 8].Bits.bit2 = 0;
		++i;
		if (source[i].memory != 0)
		{
			data->notEqualToMaxElement.push_back(source[i].memory);
			mapOfNotNull[i / 8].Bits.bit3 = 1;
		}
		else
			mapOfNotNull[i / 8].Bits.bit3 = 0;
		++i;
		if (source[i].memory != 0)
		{
			data->notEqualToMaxElement.push_back(source[i].memory);
			mapOfNotNull[i / 8].Bits.bit4 = 1;
		}
		else
			mapOfNotNull[i / 8].Bits.bit4 = 0;
		++i;
		if (source[i].memory != 0)
		{
			data->notEqualToMaxElement.push_back(source[i].memory);
			mapOfNotNull[i / 8].Bits.bit5 = 1;
		}
		else
			mapOfNotNull[i / 8].Bits.bit5 = 0;
		++i;
		if (source[i].memory != 0)
		{
			data->notEqualToMaxElement.push_back(source[i].memory);
			mapOfNotNull[i / 8].Bits.bit6 = 1;
		}
		else
			mapOfNotNull[i / 8].Bits.bit6 = 0;
		++i;
		if (source[i].memory != 0)
		{
			data->notEqualToMaxElement.push_back(source[i].memory);
			mapOfNotNull[i / 8].Bits.bit7 = 1;
		}
		else
			mapOfNotNull[i / 8].Bits.bit7 = 0;
		++i;
		if (source[i].memory != 0)
		{
			data->notEqualToMaxElement.push_back(source[i].memory);
			mapOfNotNull[i / 8].Bits.bit8 = 1;
		}
		else
			mapOfNotNull[i / 8].Bits.bit8 = 0;
	}
}

void AddressSpaceInfo::compressData2(Bite source[SIZE_OF_PAGE / 512], CompressedData* data)
{
	for (int i = 0; i < SIZE_OF_PAGE / 64; ++i)
	{
		if (source[i].memory != 0)
		{
			data->notEqualToMaxElement.push_back(source[i].memory);
			data->map[i / 8].Bits.bit1 = 1;
		}
		else
			data->map[i / 8].Bits.bit1 = 0;
		++i;
		if (source[i].memory != 0)
		{
			data->notEqualToMaxElement.push_back(source[i].memory);
			data->map[i / 8].Bits.bit2 = 1;
		}
		else
			data->map[i / 8].Bits.bit2 = 0;
		++i;
		if (source[i].memory != 0)
		{
			data->notEqualToMaxElement.push_back(source[i].memory);
			data->map[i / 8].Bits.bit3 = 1;
		}
		else
			data->map[i / 8].Bits.bit3 = 0;
		++i;
		if (source[i].memory != 0)
		{
			data->notEqualToMaxElement.push_back(source[i].memory);
			data->map[i / 8].Bits.bit4 = 1;
		}
		else
			data->map[i / 8].Bits.bit4 = 0;
		++i;
		if (source[i].memory != 0)
		{
			data->notEqualToMaxElement.push_back(source[i].memory);
			data->map[i / 8].Bits.bit5 = 1;
		}
		else
			data->map[i / 8].Bits.bit5 = 0;
		++i;
		if (source[i].memory != 0)
		{
			data->notEqualToMaxElement.push_back(source[i].memory);
			data->map[i / 8].Bits.bit6 = 1;
		}
		else
			data->map[i / 8].Bits.bit6 = 0;
		++i;
		if (source[i].memory != 0)
		{
			data->notEqualToMaxElement.push_back(source[i].memory);
			data->map[i / 8].Bits.bit7 = 1;
		}
		else
			data->map[i / 8].Bits.bit7 = 0;
		++i;
		if (source[i].memory != 0)
		{
			data->notEqualToMaxElement.push_back(source[i].memory);
			data->map[i / 8].Bits.bit8 = 1;
		}
		else
			data->map[i / 8].Bits.bit8 = 0;
	}
}

void AddressSpaceInfo::extract(CompressedData& data)
{

}*/

void AddressSpaceInfo::print()
{
	for (size_t i = 0; i < vectorOfRegionInfos_.size(); ++i)
	{
		vectorOfRegionInfos_[i]->print();
	}
}

