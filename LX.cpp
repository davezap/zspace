#include "LX.h"
#include <memory>
//#include "DirectXMesh.h"



HRESULT LoadFromOBJ(const wchar_t* szFilename,
	std::unique_ptr<Mesh>& inMesh, std::vector<Mesh::Material>& inMaterial,
	bool ccw, bool dds);


LX::LX() {
	cnt_meshes = 0;
	cnt_materials = 0;
	attr_offsets[0] = 0;
	materials[0] = 0;
	meshes[0] = 0;
}

LX::~LX() {
	for (zp_size_t i = 0; i < cnt_meshes; ++i) {
		if (meshes[i]) delete meshes[i];
	}
	for (zp_size_t i = 0; i < cnt_materials; ++i) {
		if (materials[i]) delete materials[i];
	}
}

void LX::loadFile(const wchar_t* szFilename)
{
	std::unique_ptr<Mesh> inMesh;
	std::vector<Mesh::Material> inMaterial;
	HRESULT hr = LoadFromOBJ(szFilename, inMesh, inMaterial, false, false);

	attr_offsets[cnt_meshes] = (zp_size_t)inMaterial.size();
	meshes[cnt_meshes++] = inMesh.release();

	for (const auto& mat : inMaterial)
	{
		materials[cnt_materials++] = new Mesh::Material(mat);

	}
}

void LX::cleanUp()
{

}

void LX::clear()
{

}

zp_size_t LX::getObjectCount() const
{
	return cnt_meshes;
}

Mesh* LX::getObject(zp_size_t idx) const
{
	return meshes[idx];
}

zp_size_t LX::getTextureCount() const
{
	return cnt_materials;
}

zp_size_t LX::getAttrOffset(zp_size_t lim) const
{
	zp_size_t n = 0;
	zp_size_t idx = 0;
	while (idx < lim)
	{
		n += attr_offsets[idx++];
	}
	return n;
}

Mesh::Material* LX::getTexture(zp_size_t idx) const
{
	return materials[idx];
}

