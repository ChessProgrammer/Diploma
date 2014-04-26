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

		if (blockInfo->storage == MEM_PRIVATE && !(blockInfo->protection & PAGE_GUARD))
		{
			blockInfo->data = new char[blockInfo->size];
			bool k = ReadProcessMemory(handleOfProcess_, blockInfo->baseAddress, blockInfo->data, blockInfo->size, 0);
			std::cout << k << "   " << blockInfo->protection << "    ";
			std::cout << std::string(blockInfo->data) << std::endl;
			blockInfo->data[blockInfo->size] = '\0';

			compressBlock(*blockInfo);
		}
		else
			blockInfo->data = NULL;

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

void AddressSpaceInfo::compressBlock(BlockInfo& blockInfo)
{
	const size_t countOfPages = blockInfo.size / SIZE_OF_PAGE;

	char* ptr = blockInfo.data;
	for (int i = 0; i < countOfPages; i++)
	{
		CompressedPage* cPage = compressPage(ptr);
		blockInfo.compressedData.push_back(cPage);
		ptr = ptr + SIZE_OF_PAGE;
	}
}

CompressedPage* AddressSpaceInfo::compressPage(const char data[SIZE_OF_PAGE])
{
	std::map<char, size_t> map;
	char max;
	size_t count = 0;

	for (size_t i = 0; i < SIZE_OF_PAGE; ++i)
	{
		map[data[i]] = 0;
	}

	for (int i = 0; i < SIZE_OF_PAGE; ++i)
	{
		++map[data[i]];

		if (map[data[i]] > count)
		{
			max = data[i];
			count++;
		}
	}

	Bite mapOfNotEqualElements[SIZE_OF_PAGE / 8];

	CompressedPage* cPage = new CompressedPage();
	cPage->maxElement = max;
	for (int i = 0; i < SIZE_OF_PAGE; ++i)
	{
		if (data[i] != max)
		{
			cPage->notEqualToMaxElement_In_4096_Bytes.push_back(data[i]);
			mapOfNotEqualElements[i / 8].set(i % 8 + 1);
		}
	}

	compress_512_To_64(mapOfNotEqualElements, cPage);

	return cPage;
}

void AddressSpaceInfo::compress_512_To_64(Bite data[SIZE_OF_PAGE / 64], CompressedPage* cPage)
{
	Bite mapOfNotNullElements[SIZE_OF_PAGE / 64];
	for (int i = 0; i < SIZE_OF_PAGE / 64; ++i)
	{
		if (data[i].memory != 0)
		{
			cPage->notNullElements_In_512_Bytes.push_back(data[i].memory);
			mapOfNotNullElements[i / 8].set(i % 8 + 1);
		}
	}

	compress_64_To_8(mapOfNotNullElements, cPage);
}

void AddressSpaceInfo::compress_64_To_8(Bite data[SIZE_OF_PAGE / 512], CompressedPage* cPage)
{
	for (int i = 0; i < SIZE_OF_PAGE / 64; ++i)
	{
		if (data[i].memory != 0)
		{
			cPage->notNullElements_In_64_Bytes.push_back(data[i].memory);
			cPage->map[i / 8].set(i % 8 + 1);
		}
	}
}

void AddressSpaceInfo::extract(CompressedPage& data)
{

}

void AddressSpaceInfo::print()
{
	for (size_t i = 0; i < vectorOfRegionInfos_.size(); ++i)
	{
		vectorOfRegionInfos_[i]->print();
	}
}

