#ifndef ZSPACE_LX_TYPES_H
#define ZSPACE_LX_TYPES_H

#include "engine_types.h"
#include "Meshconvert\Mesh.h"


class LX {

private:
	unsigned int cnt_meshes;
	unsigned int cnt_materials;
	Mesh* meshes[200];
	Mesh::Material* materials[300];
	unsigned int attr_offsets[300];

public:
	LX();
	~LX();
	void loadFile(const wchar_t* szFilename);
	void cleanUp();
	void clear();
	z_size_t getObjectCount() const;
	Mesh* getObject(z_size_t idx) const;
	z_size_t getTextureCount() const;
	z_size_t getAttrOffset(z_size_t lim) const;
	Mesh::Material* getTexture(z_size_t idx) const;
};


#endif  /* ZSPACE_LX_TYPES_H */