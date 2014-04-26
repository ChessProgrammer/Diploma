#include<Windows.h>
#include<vector>

#include "regionInfo.h"

class AddressSpaceInfo
{
	bool processRegion(void* address, RegionInfo* regionInfo);

	void AddressSpaceInfo::compressPage(const char source[SIZE_OF_PAGE], CompressedData* compressedData);
	//void compressData(BlockInfo& blockInfo);
	//void compressData1(Bite source[SIZE_OF_PAGE / 64], CompressedData* compressed);
	//void compressData2(Bite source[SIZE_OF_PAGE / 512], CompressedData* compressed);

	//void extract(CompressedData& compressed);

public:
	AddressSpaceInfo(long processId);

	void processAllAddressSpace();
	void print();

private:
	typedef std::vector<RegionInfo*> VectorOfRegionInfos;
	VectorOfRegionInfos vectorOfRegionInfos_;

	HANDLE handleOfProcess_;
};


