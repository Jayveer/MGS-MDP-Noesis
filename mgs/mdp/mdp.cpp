#include "mdp.h"


Mdp::Mdp(std::string filename) {
	this->filename = filename;
}

Mdp::~Mdp() {

}

void Mdp::open() {
	std::ifstream mdpDat;
	mdpDat.open(filename, std::ios::binary);
	int filesize = std::filesystem::file_size(filename);

	uint8_t* mdpBuffer = new uint8_t[filesize];
	mdpDat.read((char*)mdpBuffer, filesize);
	mdpDat.close();

	MdpHeader* header = (MdpHeader*)mdpBuffer;
	MdpBone* bones = (MdpBone*)&mdpBuffer[header->boneOffset];
	MdpGroup* groups = (MdpGroup*)&mdpBuffer[header->groupOffset];
	MdpMesh* mesh = (MdpMesh*)&mdpBuffer[header->meshOffset];

	for (int i = 0; i < header->numMeshes; i++) {
		MdpFace* face = (MdpFace*)&mdpBuffer[mesh[i].faceIndexOffset];
		MdpSkin* skin = (MdpSkin*)&mdpBuffer[mesh[i].skinOffset];
		MdpMat*  mat  = (MdpMat* )&mdpBuffer[face->matOffset];

		uint8_t vertexType = (mesh[i].flag) & 0x0F;
		MdpVertexBuffer09* vertexBuffer = (MdpVertexBuffer09*)&mdpBuffer[mesh[i].vertexBufferOffset];
	}

}