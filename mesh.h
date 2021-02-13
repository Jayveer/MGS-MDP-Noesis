#pragma once
#include "mgs/mdp/mdp.h"
#include "noesis/plugin/pluginshare.h"

inline
void bindSkin(MdpMesh* mesh, BYTE* fileBuffer, noeRAPI_t* rapi, int numWeights, int stride, std::vector<float>& weights, std::vector<uint8_t>& bones) {
    MdpSkin* skin = (MdpSkin*)&fileBuffer[mesh->skinOffset];

    for (int i = 0; i < mesh->numVertexBuffer; i++) {
        for (int j = 0; j < skin->numBones; j++) {
            bones.push_back(skin->boneID[j]);
        }
    }

    for (int i = 0; i < mesh->numVertexBuffer; i++) {
        int pos = stride * i;
        uint8_t* wOffset = &fileBuffer[mesh->vertexBufferOffset + pos];

        for (int j = 0; j < numWeights; j++) {
            uint8_t weight = (wOffset[j]);
            float s = weight / 128.0;
            weights.push_back(s);
        }
    }

    int wStride = numWeights * sizeof(float);
    rapi->rpgBindBoneIndexBuffer(&bones[0], RPGEODATA_UBYTE, skin->numBones, skin->numBones);
    rapi->rpgBindBoneWeightBuffer(&weights[0], RPGEODATA_FLOAT, wStride, numWeights);
}

//need to find a more elegant way to do this
inline
void bindMesh(MdpMesh* mesh, BYTE* fileBuffer, noeRAPI_t* rapi, std::vector<float>& weights, std::vector<uint8_t>& bones, std::vector<float>& uvs, std::vector<float>& vertices, float scl) {
    std::string name = intToHexString(mesh->strcode);
    rapi->rpgSetName((char*)name.c_str());

    int stride = 0;
    int vOffset = 0;
    int nOffset = 0;
    int uOffset = 0;
    int wOffset = 0;
    int numWeights = 0;
    rpgeoDataType_e nType = RPGEODATA_BYTE;

    switch (mesh->flag & 0x0F) {
    case 0x01:
        numWeights = 2;
        stride = sizeof(MdpVertexBuffer01);
        uOffset = offsetof(MdpVertexBuffer01, uv);
        nOffset = offsetof(MdpVertexBuffer01, normal);
        vOffset = offsetof(MdpVertexBuffer01, vertex);
        nType = RPGEODATA_SHORT;
        break;
    case 0x02:
        numWeights = 4;
        stride = sizeof(MdpVertexBuffer02);
        uOffset = offsetof(MdpVertexBuffer02, uv);
        nOffset = offsetof(MdpVertexBuffer02, normal);
        vOffset = offsetof(MdpVertexBuffer02, vertex);
        nType = RPGEODATA_SHORT;
        break;
    case 0x03:
        numWeights = 8;
        stride = sizeof(MdpVertexBuffer03);
        uOffset = offsetof(MdpVertexBuffer03, uv);
        nOffset = offsetof(MdpVertexBuffer03, normal);
        vOffset = offsetof(MdpVertexBuffer03, vertex);
        nType = RPGEODATA_SHORT;
        break;
    case 0x04:
        stride = sizeof(MdpVertexBuffer04);
        uOffset = offsetof(MdpVertexBuffer04, uv);
        nOffset = offsetof(MdpVertexBuffer04, normal);
        vOffset = offsetof(MdpVertexBuffer04, vertex);
        nType = RPGEODATA_SHORT;
        break;
    case 0x09:
        numWeights = 2;
        stride = sizeof(MdpVertexBuffer09);
        uOffset = offsetof(MdpVertexBuffer09, uv);
        nOffset = offsetof(MdpVertexBuffer09, normal);
        vOffset = offsetof(MdpVertexBuffer09, vertex);
        break;
    case 0x0A:
        numWeights = 4;
        stride = sizeof(MdpVertexBuffer0A);
        uOffset = offsetof(MdpVertexBuffer0A, uv);
        nOffset = offsetof(MdpVertexBuffer0A, normal);
        vOffset = offsetof(MdpVertexBuffer0A, vertex);
        break;
    case 0x0B:
        numWeights = 8;
        stride = sizeof(MdpVertexBuffer0B);
        uOffset = offsetof(MdpVertexBuffer0B, uv);
        nOffset = offsetof(MdpVertexBuffer0B, normal);
        vOffset = offsetof(MdpVertexBuffer0B, vertex);
        break;
    case 0x0C:
        numWeights = 2;
        stride = sizeof(MdpVertexBuffer0C);
        uOffset = offsetof(MdpVertexBuffer0C, uv);
        nOffset = offsetof(MdpVertexBuffer0C, normal);
        vOffset = offsetof(MdpVertexBuffer0C, vertex);
        break;
    case 0x0D:
        numWeights = 4;
        stride = sizeof(MdpVertexBuffer0D);
        uOffset = offsetof(MdpVertexBuffer0D, uv);
        nOffset = offsetof(MdpVertexBuffer0D, normal);
        vOffset = offsetof(MdpVertexBuffer0D, vertex);
        break;
    case 0x0E:
        numWeights = 8;
        stride = sizeof(MdpVertexBuffer0E);
        uOffset = offsetof(MdpVertexBuffer0E, uv);
        nOffset = offsetof(MdpVertexBuffer0E, normal);
        vOffset = offsetof(MdpVertexBuffer0E, vertex);
        break;
    default:
        stride = -1;
    }

    for (int i = 0; i < mesh->numVertexBuffer; i++) {
        int pos = stride * i;
        int uPos = pos + uOffset;
        uint16_t* uvOffset = (uint16_t*)&fileBuffer[mesh->vertexBufferOffset + uPos];

        uvs.push_back(uvOffset[0] * 0.000251);
        uvs.push_back(uvOffset[1] * 0.000251);

        int vPos = pos + vOffset;
        int16_t* vOffset = (int16_t*)&fileBuffer[mesh->vertexBufferOffset + vPos];

        vertices.push_back(vOffset[0] / scl);
        vertices.push_back(vOffset[1] / scl);
        vertices.push_back(vOffset[2] / scl);
    }

    int size = stride * mesh->numVertexBuffer;
    if (numWeights) bindSkin(mesh, fileBuffer, rapi, numWeights, stride, weights, bones);
    rapi->rpgBindUV1BufferSafe(&uvs[0], RPGEODATA_FLOAT, 8, uvs.size() * 4);
    rapi->rpgBindPositionBufferSafe(&vertices[0], RPGEODATA_FLOAT, 12, vertices.size() * 4);
    rapi->rpgBindNormalBufferSafe(&fileBuffer[mesh->vertexBufferOffset + nOffset], nType, stride, size - nOffset);
}