#include "txp.h"

Txp::Txp(std::string filename) {
	std::ifstream fs;
	int size = std::filesystem::file_size(filename);

	fs.open(filename, std::ios::binary);
	uint8_t* p = new uint8_t[size];
	fs.read((char*)p, size);
	this->txpData = p;
	fs.close();

	this->header = (TxpHeader*)txpData;
}

Txp::~Txp() {
	delete[] txpData;
}

int Txp::getNumInfo() {
	return header->numInfo;
}

bool Txp::containsTexture(uint32_t strcode) {
	return getIdx(strcode) > -1;
}

uint32_t Txp::getStrcodeAtIndex(int idx) {
	TxpInfo* info = (TxpInfo*)&txpData[header->infoOffset];

	return idx < header->numInfo ? info[idx].strcode : 0;
}

void Txp::getAllTextures() {
	for (int i = 0; i < header->numInfo; i++) {
		int size, bpp;
		uint8_t* texture = getTextureIndexed(i, size, bpp);
		delete[] texture;
	}
}

int Txp::getIdx(uint32_t strcode) {
	TxpInfo* info = (TxpInfo*)&txpData[header->infoOffset];

	for (int i = 0; i < header->numInfo; i++) {
		if (info[i].strcode == strcode)
			return i;
	}

	return -1;
}

uint8_t* Txp::getTexture(uint32_t strcode, int& size, int& bpp) {
	int idx = getIdx(strcode);
	if (idx == -1) return NULL;
	return getTextureIndexed(idx, size, bpp);
}

int getBpp(uint16_t bpp) {
	switch (bpp & 0x0F) {
	case 5:
		return 8;
	case 4:
		return 4;
	default:
		bpp = bpp;
	}

	return bpp;
}

void unswizzle4(uint8_t* src, uint8_t* dst, int width, int height) {
	int pos = 0;

	for (int y = 0; y < height; y++) {
		int yc0 = y * 16;
		int yc1 = y / 8 * (width * 4 - 128);

		for (int x = 0; x < width / 2; x++) {
			int xc0 = x / 16 * 16;
			int xc1 = x / 16 * 128;
			int pixelPos = x - xc0 + xc1 + yc0 + yc1;

			dst[pos] = src[pixelPos] & 0xF;
			pos++;
			pixelPos = x - xc0 + xc1 + yc0 + yc1;
			dst[pos] = src[pixelPos] >> 4;
			pos++;
		}

	}
}

void unswizzle8(uint8_t* src, uint8_t* dst, int width, int height) {
	int pos = 0;

	for (int y = 0; y < height; y++) {
		int yc0 = y * 16;
		int yc1 = y / 8 * (width * 8 - 128);

		for (int x = 0; x < width; x++) {
			int xc0 = x / 16 * 16;
			int xc1 = x / 16 * 128;

			dst[pos] = src[x - xc0 + xc1 + yc0 + yc1];
			pos++;
		}

	}
}

uint8_t* decompress(uint8_t* src, int decompressedSize) {
	noeRAPI_t rapi = noeRAPI_t();
	void* ctx = rapi.rpgCreateContext();
	uint32_t compressedSize = *(uint32_t*)src;

	uint8_t* compressedData = src + 4;
	uint8_t* decompressedData = new uint8_t[decompressedSize];

	rapi.Decomp_Inflate(src, decompressedData, compressedSize, decompressedSize);

	rapi.rpgDestroyContext(ctx);
	return decompressedData;	
}


uint8_t* Txp::unswizzle(TxpImage* image, int& size) {
	int width         = image->width & 0xFFF;
	int height        = image->height & 0xFFF;
	int bpp           = image->bitsPerPixelFlag & 0x0F;
	bool isCompressed = image->bitsPerPixelFlag & 0xF0;

	size = width * height;

	uint8_t* unswizzledPixels = new uint8_t[size];
	uint8_t* pixels = isCompressed ? &txpData[image->zOffset] : &txpData[image->pixelOffset];

	if (isCompressed) pixels = decompress(pixels, size);

	switch (bpp) {
	case 4:
		unswizzle4(pixels, unswizzledPixels, width, height);
		break;
	case 5:
		unswizzle8(pixels, unswizzledPixels, width, height);
		break;
	default:
		printf("unknown bpp flag %x", bpp);
	}

	if (isCompressed) delete[] pixels;

	return unswizzledPixels;
}

uint8_t* Txp::paintPixels(TxpColour* clut, uint8_t* pixels, int width, int height, int maxWidth, int& size, int16_t xOffset, int16_t yOffset) {
	size = width * height * 4;
	uint8_t* texture = new uint8_t[size];
	int i = 0;

	for (int y = yOffset; y < yOffset + height; y++) {

		for (int x = xOffset; x < xOffset + width; x++) {
			int pixelPos = x + y * maxWidth;
			int pos = i * 4;

			texture[pos + 0] = clut[pixels[pixelPos]].b;
			texture[pos + 1] = clut[pixels[pixelPos]].g;
			texture[pos + 2] = clut[pixels[pixelPos]].r;
			texture[pos + 3] = clut[pixels[pixelPos]].a;
			i++;
		}

	}
	return texture;
}

uint8_t* makeTGA(uint8_t* data, int dataSize, int16_t width, int16_t height) {
	uint32_t pad    = 0;
	uint16_t twenty = 0x2020;
	uint32_t magic  = 0x20000;
	uint8_t* tga = new uint8_t[0x12 + dataSize];

	memcpy(tga + 0x00, &magic,  4);
	memcpy(tga + 0x04, &pad,    4);
	memcpy(tga + 0x08, &pad,    4);
	memcpy(tga + 0x0C, &width,  2);
	memcpy(tga + 0x0E, &height, 2);
	memcpy(tga + 0x10, &twenty, 2);
	memcpy(tga + 0x12, data,   dataSize);

	return tga;
}

uint8_t* Txp::getTextureIndexed(int idx, int& size, int& bpp) {
	TxpHeader* header = (TxpHeader*)txpData;
	TxpInfo* info = (TxpInfo*)&txpData[header->infoOffset];
	TxpImage* image = (TxpImage*)&txpData[info[idx].imageOffset];
	TxpColour* clut = (TxpColour*)&txpData[info[idx].clutOffset];

	if (!image->pixelOffset) return NULL;
	bpp = image->bitsPerPixelFlag & 0x0F;

	if (bpp == 6) {
		size = (image->width & 0xFFF) * (image->height & 0xFFF) + 0x80;
		return &txpData[info[idx].clutOffset];
	}

	int pixSize, texSize;
	uint8_t* pixels = unswizzle(image, size);
	uint8_t* texture = paintPixels(clut, pixels, info[idx].width, info[idx].height, (image->width & 0xFFF), texSize, info[idx].xOffset, info[idx].yOffset);
	delete[] pixels;

	uint8_t* tga = makeTGA(texture, texSize, info[idx].width, info[idx].height);
	size = texSize + 0x12;
	delete[] texture;

	return tga;
}