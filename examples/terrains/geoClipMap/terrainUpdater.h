/************************************************************************/
/* 2020.08.26 19:21 xiangtao                                                                     */
/************************************************************************/
#include "rasterTile.h"
#include "rasterSource.h"
#include "rasterUpdater.h"

class Texture;
class TerrainLoader;

struct HeightMap
{
	unsigned int width_;
	unsigned int height_;
	void * data_;

	Texture	*	heightTextureObj_ = nullptr;
};

class TerrainUpdater :public scene::RasterUpdater
{
public:
	TerrainUpdater(int w, int h);

	virtual ~TerrainUpdater();

	bool	loadTerrainFile(const char*file);
	void	initTextureObj();
	void	bindHeightMap(bool flag);
	bool	initDiffMap(const char* file);

	HeightMap				heightMap_;
	Texture*				diffuseTexture_ = nullptr;
	bool					need_update_ = true;

};
