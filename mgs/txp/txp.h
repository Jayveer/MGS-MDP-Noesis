#pragma once
#include "../common/util.h"
#include "../../noesis/plugin/pluginshare.h"

#include <vector>
#include <String>
#include <fstream>
#include <filesystem>


struct TxpHeader {
	uint32_t flag; //for type
	uint32_t strcode;
	uint32_t numImage;
	uint32_t numInfo;
	uint32_t numColour;
	uint32_t imageOffset;
	uint32_t infoOffset;
	uint32_t clutOffset;
};

struct TxpImage {
	uint16_t bitsPerPixelFlag;
	uint16_t width;   //3 bytes width 1 unknown;
	uint16_t height;  //3 bytes height 1 unknown;
	uint16_t pad;
	uint32_t pad0;
	uint32_t pixelOffset;
	uint32_t zOffset;
};

struct TxpInfo {
	uint32_t flag;
	uint32_t strcode;
	uint32_t imageOffset;
	uint32_t clutOffset;
	float uScale;
	float vScale;
	float uOffset;
	float vOffset;
	int16_t width;
	int16_t height;
	int16_t xOffset;
	int16_t yOffset;
};

struct TxpColour {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

class Txp {
public:
	Txp(std::string filename);
	~Txp();
	int getNumInfo();
	void getAllTextures();
	void setRapi(noeRAPI_t* rapi);
	uint32_t getStrcodeAtIndex(int idx);
	bool containsTexture(uint32_t strcode);
	uint8_t* getTextureIndexed(int idx, int& size, int& bpp);
	uint8_t* getTexture(uint32_t strcode, int& size, int& bpp);
private:
	noeRAPI_t* rapi;
	uint8_t* txpData;
	TxpHeader* header;

	int getIdx(uint32_t strcode);
	uint8_t* unswizzle(TxpImage* image, int& size);
	uint8_t* decompress(uint8_t* src, int decompressedSize);
	uint8_t* paintPixels(TxpColour* clut, uint8_t* pixels, int width, int height, int maxWidth, int& size, int16_t xOffset, int16_t yOffset);
};