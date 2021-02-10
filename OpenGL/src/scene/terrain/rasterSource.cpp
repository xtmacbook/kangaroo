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
#include "flashBuffer.h"
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

	void  TerrainResource::loadTileData(const RasterTileIdentifier* identifier, TileData* data)
	{
		std::string cachePath = "d:/temp/height/height/";
		cachePath += std::to_string(identifier->level_) + "/";
		cachePath += std::to_string(identifier->x_);

		std::string cacheFilename = cachePath + "/" +  std::to_string(identifier->y_) + ".png";

		if (!IO::File::existDirectory(cachePath.c_str()))
		{
			LOGE("file %s does not exist!\n", cacheFilename);
			return;
		}
		
		Image_SP image = ::IO::EngineLoad::loadImage(cacheFilename.c_str());
		data->pullData(image->getLevel(0));
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


	TileData::TileData(unsigned int w, unsigned int h,bool highFile):hight_(highFile)
	{
		pbo_ = new FlashBuffer<uint8>(w * h * 2);//RGB
		pbo_->setTarget(GL_PIXEL_UNPACK_BUFFER);
		pbo_->create_buffers(2);

		texture_ = new Texture();
		texture_->target_ = GL_TEXTURE_RECTANGLE;
		texture_->width_ = w;
		texture_->height_ = h;
		texture_->format_ = GL_RED;
		texture_->internalformat_ = GL_R16F;
		texture_->type_ = GL_UNSIGNED_SHORT;
		texture_->createObj();
		texture_->bind();
		texture_->clampToEdge();
		texture_->filterNearest();
		texture_->contextNULL();
		texture_->unBind();
	}

	TileData::~TileData()
	{
		SAFTE_DELETE(pbo_);
	}


	base::SmartPointer<Texture> TileData::getTexture()
	{
		return texture_;
	}


	void TileData::pullData(void* data)
	{
		math::uint8 * mem = pbo_->getBufferPtr();
		memcpy(mem, data,pbo_->blockSize());
	}


	void TileData::updateTexture()
	{
		pbo_->setDataSize(pbo_->blockSize());
		pbo_->flush_data(false);
		texture_->bind();
		if(hight_) glTexSubImage2D(texture_->target(), 0, 0, 0, 
			texture_->width(), texture_->heigh(), texture_->externFormat(), texture_->type(), 0);
		pbo_->unbind();
		texture_->unBind();

		CHECK_GL_ERROR;
	}

	void TileData::getBuffer()
	{
		pbo_->mapBuffer();
	}

}

