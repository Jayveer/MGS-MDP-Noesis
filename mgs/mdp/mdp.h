#pragma once
#include <fstream>
#include <filesystem>

#include "../common/util.h"

struct vec4 {
	float x;
	float y;
	float z;
	float w;
};

struct vec2Short {
	uint16_t x;
	uint16_t y;
};

struct vec3Short {
	uint16_t x;
	uint16_t y;
	uint16_t z;
};

struct vec4Short {
	uint16_t x;
	uint16_t y;
	uint16_t z;
	uint16_t w;
};

struct vec4Byte {
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t w;
};

struct MdpHeader {
	uint32_t magic;
	uint32_t numBones;
	uint32_t numGroups;
	uint32_t numMeshes;
	uint32_t strcode;
	uint32_t boneOffset;
	uint32_t groupOffset;
	uint32_t meshOffset;
	uint32_t unknown;
        //"pad0" used as a pointer to an internal GE Command on the MDP of the m870
	uint32_t internalGeCMDOffset;
	uint32_t pad1;
	uint32_t pad2;
	vec4 max;
	vec4 min;
};

struct MdpBone {
	uint32_t strcode;
	uint32_t y0;
	int32_t parent;
	uint32_t w0;
	vec4 parentPos;
	vec4 worldPos;
	vec4 max;
	vec4 min;
};

struct MdpGroup {
	uint32_t strcode;
	uint32_t flag;
	uint32_t parent;
	uint32_t pad0;
};

struct MdpMesh {
	uint32_t strcode;
	uint32_t flag;
	uint32_t numFaceIndex;
	uint32_t faceIndexOffset;
	uint32_t vertexBufferOffset;
	uint32_t skinOffset;
	uint32_t numVertexBuffer;
	uint32_t pad0;
	vec4 max;
	vec4 min;
	vec4 pos;
};

struct MdpFace {
	uint16_t unknown;
	uint16_t faceBufferSize;
	uint32_t faceBufferOffset;
	uint32_t matOffset;
};

struct MdpMat {
	uint32_t strcode;
	uint32_t unknown;
	uint32_t r;
	uint32_t g;
	uint32_t b;
	float specularPower;
};

struct MdpSkin {
	uint16_t pad;
	uint16_t numBones;
	uint8_t  boneID[];
};

struct MdpVertexBuffer01 {
	uint8_t weightA;
	uint8_t weightB;
	vec2Short uv;
	vec3Short normal;
	vec3Short vertex;
};

struct MdpVertexBuffer02 {
	uint8_t weightA;
	uint8_t weightB;
	uint8_t weightC;
	uint8_t weightD;
	vec2Short uv;
	vec3Short normal;
	vec3Short vertex;
};

struct MdpVertexBuffer03 {
	uint8_t weightA;
	uint8_t weightB;
	uint8_t weightC;
	uint8_t weightD;
	uint8_t weightE;
	uint8_t weightF;
	uint8_t weightG;
	uint8_t weightH;
	vec2Short uv;
	vec3Short normal;
	vec3Short vertex;
};

struct MdpVertexBuffer04 {
	vec2Short uv;
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
	vec3Short normal;
	vec3Short vertex;
};

struct MdpVertexBuffer09 {
	uint8_t weightA;
	uint8_t weightB;
	vec2Short uv;
	vec4Byte normal;
	vec3Short vertex;
};

struct MdpVertexBuffer0A {
	uint8_t weightA;
	uint8_t weightB;
	uint8_t weightC;
	uint8_t weightD;
	vec2Short uv;
	vec4Byte normal;
	vec3Short vertex;
};

struct MdpVertexBuffer0B {
	uint8_t weightA;
	uint8_t weightB;
	uint8_t weightC;
	uint8_t weightD;
	uint8_t weightE;
	uint8_t weightF;
	uint8_t weightG;
	uint8_t weightH;
	vec2Short uv;
	vec4Byte normal;
	vec3Short vertex;
};

struct MdpVertexBuffer0C {
	uint8_t weightA;
	uint8_t weightB;
	vec2Short uv;
	uint16_t unknown;
	vec4Byte normal;
	vec3Short vertex;
};

struct MdpVertexBuffer0D {
	uint8_t weightA;
	uint8_t weightB;
	uint8_t weightC;
	uint8_t weightD;
	vec2Short uv;
	uint16_t unknown;
	vec4Byte normal;
	vec3Short vertex;
};

struct MdpVertexBuffer0E {
	uint8_t weightA;
	uint8_t weightB;
	uint8_t weightC;
	uint8_t weightD;
	uint8_t weightE;
	uint8_t weightF;
	uint8_t weightG;
	uint8_t weightH;
	vec2Short uv;
	uint16_t unknown;
	vec4Byte normal;
	vec3Short vertex;
};

class Mdp {
public:
	Mdp(std::string filename);
	~Mdp();

	void open();
private:
		std::string filename;
};
