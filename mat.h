#pragma once
#include "mgs/mdp/mdp.h"
#include "mgs/txp/txp.h"
#include "noesis/plugin/pluginshare.h"

inline
int findMaterialIdx(char* matName, CArrayList<noesisMaterial_t*>& matList) {
    for (int i = 0; i < matList.Num(); i++) {
        if (!strcmp(matList[i]->name, matName))
            return i;
    }
    return -1;
}

inline
int findTextureIdx(char* texName, CArrayList<noesisTex_t*>& texList) {
    for (int i = 0; i < texList.Num(); i++) {
        if (!strcmp(texList[i]->filename, texName))
            return i;
    }
    return -1;
}

inline
std::string findTXP(noeRAPI_t* rapi, uint32_t& strcode) {
    std::filesystem::path p{ rapi->Noesis_GetInputName() };
    p = p.parent_path();

    for (const std::filesystem::directory_entry& file : std::filesystem::recursive_directory_iterator(p)) {
        if (file.path().extension() == ".txp") {
            Txp txp = Txp(file.path().u8string());
            if (txp.containsTexture(strcode))
                return file.path().u8string();
        }
    }

    return "";
}

inline
noesisTex_t* loadTexture(noeRAPI_t* rapi, uint32_t& strcode) {
    std::string txpFile = findTXP(rapi, strcode);
    if (txpFile == "") return NULL;

    int size, bpp;

    Txp txp = Txp(txpFile);
    uint8_t* texData = txp.getTexture(strcode, size, bpp);

    noesisTex_t* noeTexture = bpp == 6 ? rapi->Noesis_LoadTexByHandler(texData, size, ".dds")  : rapi->Noesis_LoadTexByHandler(texData, size, ".tga");
    if (bpp != 6) delete[] texData;
    return noeTexture;
}

inline
void bindMat(MdpFace* face, BYTE* fileBuffer, noeRAPI_t* rapi, CArrayList<noesisMaterial_t*>& matList, CArrayList<noesisTex_t*>& texList) {
    MdpMat* mat = (MdpMat*)&fileBuffer[face->matOffset];

    //set mat name
    std::string matStr = std::to_string(face->matOffset);
    char matName[9];
    strcpy_s(matName, matStr.c_str());

    //check if material already exists
    int x = findMaterialIdx(matName, matList);

    //use existing mat if it does
    if (x > -1) {
        rapi->rpgSetMaterial(matName);
        return;
    }

    //create material
    noesisMaterial_t* noeMat = rapi->Noesis_GetMaterialList(1, false);
    noeMat->name = rapi->Noesis_PooledString(matName);

    //set tex name
    std::string texStr = intToHexString(mat->strcode) + ".tga";
    char texName[11];
    strcpy_s(texName, texStr.c_str());

    //check if texture already exists
    int y = findTextureIdx(texName, texList);

    //set tex to mat if it does
    if (y > -1) {
        noeMat->texIdx = y;
        matList.Append(noeMat);
        rapi->rpgSetMaterial(matName);
        return;
    }

    //load texture
    noesisTex_t* noeTexture = loadTexture(rapi, mat->strcode);
    if (!noeTexture) return;
    noeTexture->filename = rapi->Noesis_PooledString(texName);

    //set tex to mat
    noeMat->texIdx = texList.Num();
    matList.Append(noeMat);

    texList.Append(noeTexture);

    //set material
    rapi->rpgSetMaterial(noeMat->name);
}