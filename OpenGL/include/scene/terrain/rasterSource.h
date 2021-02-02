/************************************************************************/
/* 2020.08.28 15:21 xiangtao                                                                     */
/************************************************************************/


#ifndef ____RASTER_SOURCE__
#define ____RASTER_SOURCE__

#include "decl.h"
#include <map>

#include "rasterExtent.h"
#include "smartPointer.h"
#include "BaseObject.h"

class Texture;

namespace scene
{
	class RasterLoader;
	class RasterLevel;

	struct TileData :public ::base::BaseObject
	{
		::base::SmartPointer<Texture>	texture_ ;
		virtual ~TileData();
	};
	
	typedef ::base::SmartPointer<TileData> TileDataRef;

	class LIBENIGHT_EXPORT RasterResource
	{
	public:
		virtual ~RasterResource();

		virtual		bool				init() = 0;

		virtual const GeodeticExtent*	extend()const;

		int								getLevelsNum()const;
		int								getTitleWidth()const;
		int								getTileHeigh()const;
		RasterTile *					getTile(const RasterTileIdentifier*);

		virtual TileDataRef				loadTileData(const RasterTileIdentifier*) = 0;

		void							setNumLevel(int num);
		void							setTileWH(int w, int h);
		void							setlevelZeroDeltaRasterWH(int w, int h);
		void							setExtent(const GeodeticExtent&);

		RasterLevel *					level(int);

	protected:
		int								num_levels_;
		int								tile_with_;
		int								tile_hight_;

		double							levelZeroDeltaRasterW_ = 0.0;
		double							levelZeroDeltaRasterH_ = 0.0;
		GeodeticExtent					extend_;

		std::map<int, RasterLevel*>		levels_;
		std::map<const RasterTileIdentifier*, RasterTile*, TileIdentifierComp>	activeTiles_;

	};

	class LIBENIGHT_EXPORT TerrainResource :public RasterResource
	{
	public:

		TerrainResource();

		virtual ~TerrainResource();
		virtual		bool				init();
		virtual TileDataRef 				loadTileData(const RasterTileIdentifier*);

	};
}

#endif
