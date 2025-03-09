#include "LX.h"
#include <memory>
//#include "DirectXMesh.h"



HRESULT LoadFromOBJ(const wchar_t* szFilename,
	std::unique_ptr<Mesh>& inMesh, std::vector<Mesh::Material>& inMaterial,
	bool ccw, bool dds);


LX::LX() {
	attr_offsets.clear();
	materials.clear();
	meshes.clear();
}

LX::~LX() {
	/*
	for (z_size_t i = 0; i < cnt_meshes; ++i) {
	//	delete meshes[i];
	}
	for (z_size_t i = 0; i < cnt_materials; ++i) {
	//	if (materials[i]) delete materials[i];
	}
	*/
}

void LX::loadFile(const wchar_t* szFilename)
{
	std::unique_ptr<Mesh> inMesh;
	std::vector<Mesh::Material> inMaterial;
	HRESULT hr = LoadFromOBJ(szFilename, inMesh, inMaterial, false, false);

	attr_offsets[meshes.size()] = (z_size_t)inMaterial.size();
	meshes.push_back(inMesh.release());

	for (const auto& mat : inMaterial)
	{
		materials.push_back(new Mesh::Material(mat));

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
	return meshes.size();
}

Mesh* LX::getObject(z_size_t idx) const
{
	return meshes[idx];
}

z_size_t LX::getTextureCount() const
{
	return materials.size();
}

z_size_t LX::getAttrOffset(z_size_t lim) const
{
	z_size_t n = 0;
	z_size_t idx = 0;
	while (idx < lim)
	{
		n += attr_offsets.at(idx);
		idx++;
	}
	return n;
}

Mesh::Material* LX::getTexture(z_size_t idx) const
{
	return materials[idx];
}

