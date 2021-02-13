#pragma once
#include <vector>
#include <fstream>
#include <filesystem>

typedef struct MtarData {
	uint32_t mtcmOffset;
	uint32_t mtcmSize;
	uint32_t mtexOffset;
	uint32_t mtexSize;
} MtarData;

typedef struct MtarHeader {
	uint32_t magic;
	uint16_t maxJoint;
	uint16_t maxEffPos;
	uint16_t numBoneName;
	uint16_t numMotion;
	uint32_t flags;
	uint32_t mtcmOffset;
	uint32_t mtexOffset;
	uint32_t boneNameTableOffset;
	uint32_t dataTableOffset;
} MtarHeader;

class Mtar {
public:
	Mtar(std::string filename);
	~Mtar();
private:
	uint8_t* mtarData;
	MtarHeader* mtarHeader;
	bool checkMagic();
};