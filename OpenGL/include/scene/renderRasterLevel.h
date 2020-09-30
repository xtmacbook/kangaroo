/************************************************************************/
/* 2020.08.26 19:21 xiangtao                                                                     */
/************************************************************************/

#ifndef ____RENDER_RASTER_LEVEL__
#define ____RENDER_RASTER_LEVEL__

#include "rasterExtent.h"
#include "type.h"

using namespace math;

namespace scene
{
	class RasterLevel;

	class LIBENIGHT_EXPORT RenderRasterLevel
	{
	public:
		RenderRasterLevel();

		virtual ~RenderRasterLevel();

		const RasterExtent&		nextExtend()const;
		const V2i&				originTexture()const;

		void					setRasterLevel(RasterLevel*);

		RasterLevel*			getRaster();

		void					setFiner(const RenderRasterLevel*);
		void					setCoarser(const RenderRasterLevel*);

		const RenderRasterLevel*	finerLevel()const;
		const RenderRasterLevel*	coarserLevel()const;

		virtual	void			updateExtentAndTextureOrigin() = 0;
		virtual	void			updateTextureOrigin(int xstrip,int ystrip);

	protected:
		const  RenderRasterLevel *		finerLevel_ = nullptr;
		const  RenderRasterLevel *		coarserLevel_ = nullptr;

		RasterLevel*			raster_ = nullptr;
		RasterExtent			currentExtent_ = {1,0,0,1};
		RasterExtent			nextExtent_;

		V2i						originInTextures_;
	};
}

#endif
