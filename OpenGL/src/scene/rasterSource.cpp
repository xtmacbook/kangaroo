#include "rasterTile.h"
#include "rasterSource.h"
#include "rasterLevel.h"
#include "stream.h"
#include "mesh.h"
#include "engineLoad.h"
#include <string>
#include <math.h>

namespace scene
{
	 
	RasterResource::~RasterResource()
	{

	}

	const scene::GeodeticExtent* RasterResource::extend() const
	{
		return &extend_;
	}

	int RasterResource::getLevelsNum() const
	{
		return num_levels_;
	}

	int RasterResource::getTitleWidth() const
	{
		return tile_with_;
	}

	int RasterResource::getTileHeigh() const
	{
		return tile_hight_;
	}

	RasterTile * RasterResource::getTile(const RasterTileIdentifier* identifier)
	{
		//conside to delete 
		if (activeTiles_.find(identifier) == activeTiles_.end())
		{
			RasterTile * tile = new RasterTile(this, identifier);
			activeTiles_[tile->identifier()] = tile;
		}
		return activeTiles_[identifier];
	}


	void RasterResource::setNumLevel(int num)
	{
		num_levels_ = num;
	}


	void RasterResource::setTileWH(int w, int h)
	{
		tile_with_ = w;
		tile_hight_ = h;
	}


	void RasterResource::setlevelZeroDeltaRasterWH(int w, int h)
	{
		levelZeroDeltaRasterW_ = w;
		levelZeroDeltaRasterH_ = h;
	}


	void RasterResource::setExtent(const GeodeticExtent& extent)
	{
		extend_ = extent;
	}

	TerrainResource::TerrainResource()
	{
	}

	TerrainResource::~TerrainResource()
	{
		for (int i = 0; i < num_levels_; i++)
		{
			delete levels_[i];
		}
	}

	RasterLevel * TerrainResource::level(int idx)
	{
		return levels_[idx];
	}

	TileData * TerrainResource::loadTileData(const RasterTileIdentifier* identifier)
	{
		std::string cachePath = "d:/temp/terrain_png/" + std::to_string(0 + 2);//the test file need this plus tow
		cachePath += "/";
		cachePath += std::to_string(identifier->x_);

		std::string cacheFilename = cachePath + "/" +  std::to_string(identifier->y_) + ".png";

		if (!IO::File::existDirectory(cachePath.c_str()))
		{
			IO::File::existDirectory(cachePath.c_str());
		}
		
		::IO::EngineLoad::loadImage(cacheFilename.c_str());

		return nullptr;
	}

	bool TerrainResource::init()
	{
		//server_ = new ArcServer;

		//if (!server_->init()) return false;

		double deltaX = levelZeroDeltaRasterW_;
		double deltaY = levelZeroDeltaRasterH_;

		double width = extend_.right_ - extend_.left_;
		double heigh = extend_.top_ - extend_.buttom_;

		for (int i = 0; i < num_levels_; i++)
		{
			int xNum = (int)std::round(width / deltaX) * tile_with_ + 1;
			int yNum = (int)std::round(heigh / deltaY) * tile_hight_ + 1;
			
			levels_[num_levels_ - i - 1] = new RasterLevel(this, num_levels_ - i - 1, extend_, xNum, yNum, tile_with_, tile_hight_);

			deltaX /= 2.0;
			deltaY /= 2.0;
		}

		return true;
	}
}

