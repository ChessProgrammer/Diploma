#include<Windows.h>
#include<vector>

#include "regionInfo.h"

class AddressSpaceInfo
{
	bool processRegion(void* address, RegionInfo* regionInfo);

	void compressBlock(BlockInfo& blockInfo);
	CompressedPage* AddressSpaceInfo::compressPage(const char source[SIZE_OF_PAGE]);

	void compress_512_To_64(Bite source[SIZE_OF_PAGE / 64], CompressedPage* compressedPage);
	void compress_64_To_8(Bite source[SIZE_OF_PAGE / 512], CompressedPage* compressedPage);
	
	void extract(CompressedPage& compressed);

public:
	AddressSpaceInfo(long processId);

	void processAllAddressSpace();
	void print();

private:
	typedef std::vector<RegionInfo*> VectorOfRegionInfos;
	VectorOfRegionInfos vectorOfRegionInfos_;

	HANDLE handleOfProcess_;
};


