/************************************************************************/
/* 2020.08.26 19:21 xiangtao                                                                     */
/************************************************************************/

#ifndef ____RASTER_EXTENT__
#define ____RASTER_EXTENT__

#include "decl.h"

namespace scene
{
	struct GeodeticExtent
	{
		double left_;
		double right_;
		double top_;
		double buttom_;
	};

	struct RasterExtent
	{
		int left_;
		int right_;
		int top_;
		int buttom_;
	};
}

#endif
