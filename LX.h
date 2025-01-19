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
	zp_size_t getObjectCount() const;
	Mesh* getObject(zp_size_t idx) const;
	zp_size_t getTextureCount() const;
	zp_size_t getAttrOffset(zp_size_t lim) const;
	Mesh::Material* getTexture(zp_size_t idx) const;
};


#endif  /* ZSPACE_LX_TYPES_H */