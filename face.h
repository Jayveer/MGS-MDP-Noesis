#pragma once
#include "mat.h"

inline
void makeFaceBuffer(uint32_t numFaceIndex, MdpFace* face, noeRAPI_t* rapi, BYTE* fileBuffer, CArrayList<noesisMaterial_t*>& matList, CArrayList<noesisTex_t*>& texList) {
    std::vector<uint16_t> faceBuffer;
    int m = 0;
    int pos = 0;
    for (int i = 0; i < numFaceIndex; i++) {
        int j = 0;
        bool flip = 0;
        for (; j < face[i].faceBufferSize - 2; j++) {
            if (!flip) {
                faceBuffer.push_back(m); faceBuffer.push_back(m + 1); faceBuffer.push_back(m + 2);
            } else {
                faceBuffer.push_back(m); faceBuffer.push_back(m + 2); faceBuffer.push_back(m + 1);
            }
            flip = !flip;
            m++;
        }

        int s = j * 3;
        bindMat(&face[i], fileBuffer, rapi, matList, texList);
        rapi->rpgCommitTrianglesSafe(&faceBuffer[pos], RPGEODATA_SHORT, s, RPGEO_TRIANGLE, false);
        pos += s;
        m = m + 2;

    }
}

inline
void bindFace(MdpMesh* mesh, BYTE* fileBuffer, noeRAPI_t* rapi, CArrayList<noesisMaterial_t*>& matList, CArrayList<noesisTex_t*>& texList) {
    MdpFace* face = (MdpFace*)&fileBuffer[mesh->faceIndexOffset];
    uint8_t flag = (mesh->flag >> 8);

    makeFaceBuffer(mesh->numFaceIndex, face, rapi, fileBuffer, matList, texList);
}