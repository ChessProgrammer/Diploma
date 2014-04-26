#include <iostream>

#include "addressspaceinfo.h"

int main()
{
	AddressSpaceInfo addressSpaceInfo(1116);
	addressSpaceInfo.processAllAddressSpace();
	addressSpaceInfo.print();

	return(0);
}