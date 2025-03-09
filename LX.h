#ifndef ZSPACE_LX_TYPES_H
#define ZSPACE_LX_TYPES_H

#include <vector>
#include <map>
#include "z_types.h"
#include "Meshconvert\Mesh.h"


class LX {

private:
	std::vector<Mesh*> meshes;
	std::vector <Mesh::Material*> materials;
	std::map<size_t, size_t> attr_offsets;

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