#include "rasterTile.h"
#include "rasterSource.h"
#include "rasterLevel.h"
#include "stream.h"
#include "engineLoad.h"
#include <string>
#include <math.h>
#include "texture.h"
#include "gls.h"
#include "log.h"

namespace scene
{
	 
	RasterResource::~RasterResource()
	{
		for (int i = 0; i < num_levels_; i++)
		{
			SAFTE_DELETE(levels_[i]);
		}
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

	RasterLevel * RasterResource::level(int idx)
	{
		return levels_[idx];
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
	}

	TileDataRef TerrainResource::loadTileData(const RasterTileIdentifier* identifier)
	{
		std::string cachePath = "d:/temp/terrain/";
		cachePath += std::to_string(identifier->level_) + "/";
		cachePath += std::to_string(identifier->x_);

		std::string cacheFilename = cachePath + "/" +  std::to_string(identifier->y_) + ".png";

		if (!IO::File::existDirectory(cachePath.c_str()))
		{
			LOGE("file %s does not exist!\n", cacheFilename);
			return nullptr;
		}
 
		TileDataRef tileData = new TileData;
		tileData->texture_ = new Texture(cacheFilename.c_str());
		tileData->texture_->target_ = GL_TEXTURE_2D;
		
		if (tileData->texture_->loadData())
		{
			tileData->texture_->createObj();
			tileData->texture_->bind();
			tileData->texture_->mirrorRepeat();
			tileData->texture_->filterLinear();
			if (!tileData->texture_->context(NULL))
			{
			}
			CHECK_GL_ERROR;
			return tileData;
		}

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

	TileData::~TileData()
	{
		 
	}

}

