
#include "geometry_clipmappings_terrain.h"
#include "layer.h"
#include "terrainUpdater.h"

#include <camera.h>
#include <helpF.h>
#include <resource.h>
#include <cameraBase.h>
/************************************************************************/
/* https://arm-software.github.io/opengl-es-sdk-for-android/terrain.html
https://mikejsavage.co.uk/blog/geometry-clipmaps.html */
/************************************************************************/
using namespace math;

ClipmapTerrain::ClipmapTerrain(int cmapSize, int level) :cmapSize_(cmapSize), 
														level_(level)
{

}

ClipmapTerrain::~ClipmapTerrain()
{
	if (layerManager_)
	{
		delete layerManager_;
	}
	if (baseShader_)
	{
		delete baseShader_;
		baseShader_ = nullptr;
	}
	if (terrainUpdater_)
	{
		delete terrainUpdater_;
		terrainUpdater_ = nullptr;
	}
}

bool ClipmapTerrain::init()
{
	
	scene::RasterResource* rasterResource = new scene::TerrainResource;
	rasterResource->setNumLevel(level_);
	rasterResource->setTileWH(256, 256);
	rasterResource->setlevelZeroDeltaRasterWH(20.0, 20.0);
	scene::GeodeticExtent extent{-180.0,180.0,90.0,-90.0};
	rasterResource->setExtent(extent);
	if (!rasterResource->init())
	{
		printf("rasterResorce init fail!...\n");
		return false;
	}

	layerManager_ = new MapLayerManager(cmapSize_, level_, rasterResource);

	TerrainBlock::initOffset(cmapSize_);
	FixUp::initOffset(cmapSize_);
	InteriorTrim::initOffset(cmapSize_);
	footprints_.init(cmapSize_);

	layerManager_->init();

	initShader();
	
	terrainUpdater_ = new TerrainUpdater(cmapSize_,cmapSize_);

	//init coarsest Level
	layerManager_->initCorsestLevel( terrainUpdater_ );

	terrainUpdater_->loadTerrainFile((get_media_BasePath() + "heightmap/ps_height_4k.png").c_str());
	terrainUpdater_->initDiffMap((get_media_BasePath() + "heightmap/ps_texture_4k.png").c_str());
	terrainUpdater_->initTextureObj();

	return true;
}

void ClipmapTerrain::update(const CameraBase*camera)
{
	layerManager_->update(camera,terrainUpdater_);
}


