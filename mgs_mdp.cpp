#include "bone.h"
#include "face.h"
#include "mesh.h"
#include "motion.h"

bool g_mgspwMtarPrompt;
const char* g_pPluginName = "mgs_mdp";
const char* g_pPluginDesc = "Metal Gear Solid MDP handler by Jayveer.";

bool checkMDP(BYTE* fileBuffer, int bufferLen, noeRAPI_t* rapi) {
    uint32_t magic = *(uint32_t*)fileBuffer;
    return magic == 0x2050444D;
}

noesisModel_t* loadMDP(BYTE* fileBuffer, int bufferLen, int& numMdl, noeRAPI_t* rapi) {
    void* ctx = rapi->rpgCreateContext();

    MdpHeader*  header  = (MdpHeader*) fileBuffer;
    MdpBone*    bones   = (MdpBone*  )&fileBuffer[header->boneOffset ];
    MdpGroup*   groups  = (MdpGroup* )&fileBuffer[header->groupOffset];
    MdpMesh*    mesh    = (MdpMesh*  )&fileBuffer[header->meshOffset ];

    modelBone_t* noeBones = (header->numBones) ? bindBones(bones, header->numBones, rapi) : NULL;

    CArrayList<noesisTex_t*>      texList;
    CArrayList<noesisMaterial_t*> matList;

    for (int i = 0; i < header->numMeshes; i++) {
        MdpFace* face = (MdpFace*)&fileBuffer[mesh[i].faceIndexOffset];
        MdpSkin* skin = (MdpSkin*)&fileBuffer[mesh[i].skinOffset];

        std::vector<float> vertices;
        std::vector<float>  weights;
        std::vector<uint8_t>  bones;
        std::vector<float>      uvs;

        if (!mesh[i].vertexBufferOffset) {
            g_nfn->NPAPI_MessagePrompt(L"Vertex offset is 0, unknown");
            return NULL;
        }

        float scl = 38000 / header->max.x + 1.0;
        bindMesh(&mesh[i], fileBuffer, rapi, weights, bones, uvs, vertices, scl);
        bindFace(&mesh[i], fileBuffer, rapi, matList, texList);

        rapi->rpgClearBufferBinds();
    }

    noesisMatData_t* md = rapi->Noesis_GetMatDataFromLists(matList, texList);
    rapi->rpgSetExData_Materials(md);

    if (g_mgspwMtarPrompt && header->numBones) {
        BYTE * motionFile = openMotion(rapi);
        if (motionFile) loadMotion(rapi, motionFile, noeBones, header->numBones);
    }

    noesisModel_t* mdl = rapi->rpgConstructModel();
    if (mdl) numMdl = 1;

    rapi->rpgDestroyContext(ctx);
    return mdl;
}

int mgspw_anim_prompt(int toolIdx, void* user_data) {
    g_mgspwMtarPrompt = !g_mgspwMtarPrompt;
    g_nfn->NPAPI_CheckToolMenuItem(toolIdx, g_mgspwMtarPrompt);
    return 1;
}

bool NPAPI_InitLocal(void) {
    int fh = g_nfn->NPAPI_Register("Metal Gear Solid: Peace Walker", ".mdp");
    if (fh < 0) return false;
    
    g_nfn->NPAPI_SetTypeHandler_TypeCheck(fh, checkMDP);
    g_nfn->NPAPI_SetTypeHandler_LoadModel(fh, loadMDP);

    int handle = g_nfn->NPAPI_RegisterTool("Prompt for Motion Archive", mgspw_anim_prompt, NULL);  
    g_nfn->NPAPI_SetToolSubMenuName(handle, "Metal Gear Solid: Peace Walker");
    g_nfn->NPAPI_SetToolHelpText(handle, "Prompt for Animation?");

    return true;
}


void NPAPI_ShutdownLocal(void) {
    
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}