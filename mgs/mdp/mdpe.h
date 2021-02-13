#pragma once
#include <inttypes.h>

struct MdpeHeader {
	uint32_t pad0;
	uint32_t numMeshInfo;
	uint32_t numUnknown;
	uint32_t pad1;
	uint32_t pad2;
	uint32_t meshInfoOffset;
	uint32_t unknownOffset;
};