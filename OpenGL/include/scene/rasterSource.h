/************************************************************************/
/* 2020.08.28 15:21 xiangtao                                                                     */
/************************************************************************/


#ifndef ____RASTER_SOURCE__
#define ____RASTER_SOURCE__

#include "decl.h"
#include <map>

#include "rasterExtent.h"

namespace scene
{
	class RasterLoader;
	class RasterLevel;
	class Texture;
	struct TileData
	{
		Texture * texture_;
	};

	
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

		virtual TileData *				loadTileData(const RasterTileIdentifier*) = 0;

		void							setNumLevel(int num);
		void							setTileWH(int w, int h);
		void							setlevelZeroDeltaRasterWH(int w, int h);
		void							setExtent(const GeodeticExtent&);

		std::map<const RasterTileIdentifier*, RasterTile*, TileIdentifierComp>	activeTiles_;
		int								num_levels_;
		int								tile_with_;
		int								tile_hight_;

		double							levelZeroDeltaRasterW_ = 0.0;
		double							levelZeroDeltaRasterH_ = 0.0;

		GeodeticExtent					extend_;

	};

	class LIBENIGHT_EXPORT TerrainResource :public RasterResource
	{
	public:

		TerrainResource();

		virtual ~TerrainResource();
		virtual		bool				init();
		RasterLevel *					level(int);

		virtual TileData *				loadTileData(const RasterTileIdentifier*);
	private:
		std::map<int,RasterLevel*>	levels_;
	};
}

#endif
