#include "renderRasterLevel.h"

namespace scene
{
	RenderRasterLevel::RenderRasterLevel():originInTextures_(0.0)
	{

	}


	RenderRasterLevel::~RenderRasterLevel()
	{

	}

	const RasterExtent& RenderRasterLevel::nextExtend() const
	{
		return nextExtent_;
	}

	const V2i& RenderRasterLevel::originTexture() const
	{
		return originInTextures_;
	}

	void RenderRasterLevel::setRasterLevel(RasterLevel*level)
	{
		raster_ = level;
	}

	scene::RasterLevel* RenderRasterLevel::getRaster()
	{
		return raster_;
	}

	void RenderRasterLevel::setFiner(const RenderRasterLevel*finer)
	{
		finerLevel_ = finer;
	}

	void RenderRasterLevel::setCoarser(const RenderRasterLevel*coarser)
	{
		coarserLevel_ = coarser;
	}

	const RenderRasterLevel* RenderRasterLevel::finerLevel() const
	{
		return finerLevel_;
	}

	const RenderRasterLevel* RenderRasterLevel::coarserLevel() const
	{
		return coarserLevel_;
	}

	void RenderRasterLevel::updateTextureOrigin(int xstrip, int ystrip)
	{
		int deltaX = nextExtent_.left_ - currentExtent_.left_;
		int deltaY = nextExtent_.buttom_ - currentExtent_.buttom_;
		if (deltaX == 0 && deltaY == 0)
			return;

		if (currentExtent_.left_ > currentExtent_.right_ ||  // initial update
			std::abs(deltaX) >= xstrip || std::abs(deltaX) >= ystrip)      // complete update
		{
			originInTextures_ = V2i(0, 0);
		}
		else
		{
			int newOriginX = (originInTextures_.x + deltaX) % xstrip;
			if (newOriginX < 0)
				newOriginX += xstrip;
			int newOriginY = (originInTextures_.y + deltaY) % ystrip;
			if (newOriginY < 0)
				newOriginY += ystrip;
			originInTextures_ = V2i(newOriginX, newOriginY);
		}
	}

}

