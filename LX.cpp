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
	for (z_size_t i = 0; i < cnt_meshes; ++i) {
		if (meshes[i]) delete meshes[i];
	}
	for (z_size_t i = 0; i < cnt_materials; ++i) {
		if (materials[i]) delete materials[i];
	}
}

void LX::loadFile(const wchar_t* szFilename)
{
	std::unique_ptr<Mesh> inMesh;
	std::vector<Mesh::Material> inMaterial;
	HRESULT hr = LoadFromOBJ(szFilename, inMesh, inMaterial, false, false);

	attr_offsets[cnt_meshes] = (z_size_t)inMaterial.size();
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

z_size_t LX::getObjectCount() const
{
	return cnt_meshes;
}

Mesh* LX::getObject(z_size_t idx) const
{
	return meshes[idx];
}

z_size_t LX::getTextureCount() const
{
	return cnt_materials;
}

z_size_t LX::getAttrOffset(z_size_t lim) const
{
	z_size_t n = 0;
	z_size_t idx = 0;
	while (idx < lim)
	{
		n += attr_offsets[idx++];
	}
	return n;
}

Mesh::Material* LX::getTexture(z_size_t idx) const
{
	return materials[idx];
}

