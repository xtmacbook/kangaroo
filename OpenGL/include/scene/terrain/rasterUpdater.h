/************************************************************************/
/* 2020.08.26 19:21 xiangtao                                                                     */
/************************************************************************/

#ifndef ____RASTER_UPDATER__
#define ____RASTER_UPDATER__

#include "decl.h"
#include "synchron.h"
#include "dataQueue.h"
#include "messQue.h"
#include "rasterTile.h"
#include "BaseObject.h"
#include "smartPointer.h"
#include <map>
#include <list>
#include <vector>

namespace scene
{
	class RasterResource;
	class RasterLevel;
	class RasterTile;
	class TerrainRasterLevel;
	class RasterLoader;


	class LIBENIGHT_EXPORT TileLoadRequest
	{
	public:

		TileLoadRequest(TerrainRasterLevel* , RasterTile *);

		~TileLoadRequest();

		RasterTile *			title_ = nullptr;
		::base::SmartPointer<TileData>				data_;
		TerrainRasterLevel*		level_ = nullptr;
		bool					isNode_ = false; //is one node of list(be reference by one list/queue)


		bool operator==(const TileLoadRequest*other)const
		{
			return (this->title_->identifier() == other->title_->identifier());
		}
		bool operator!=(const TileLoadRequest*other)const
		{
			return (this->title_->identifier() != other->title_->identifier());
		}
	};

	struct LIBENIGHT_EXPORT RasterDataQeq
	{
	public:

		typedef std::map <const RasterTileIdentifier*, TileLoadRequest*, TileIdentifierComp> TILE_LOAD_REQUEST;
		typedef std::map <const RasterTileIdentifier*, ::base::SmartPointer<TileData>, TileIdentifierComp> TITLE_DATA_LOAD;


		typedef TILE_LOAD_REQUEST::const_iterator LOADINGCITER;
		typedef TITLE_DATA_LOAD::const_iterator TEXTUREITER;

		TILE_LOAD_REQUEST			loadingTiles_;
		TITLE_DATA_LOAD				loadedTiles_;
		base::DataQequest<TileLoadRequest> requestList_;
	
		bool					  stopRequest_ = false;

		base::SameProcessMutex	  requestListMutex_;
		base::ConditionVar        requestCondition_;

		base::MessageQueues<int, TileLoadRequest> doneQue_;
	};

	class LIBENIGHT_EXPORT RasterUpdater :public base::BaseObject
	{
	public:
		RasterUpdater();

		virtual ~RasterUpdater();

		virtual void	requestTileResidency(TerrainRasterLevel * ,RasterLevel*,const RasterExtent*);
		virtual void	requestTileLoad(TerrainRasterLevel * ,RasterTile * );
		virtual void	applyNewData();
		virtual void	applyNewTile(RasterTile * title, TerrainRasterLevel*) = 0;
		void			stopUpdater();
		int				getDoneReqs(TileLoadRequest * rq, std::vector<TileLoadRequest*>*doneQ);
		void			applyIfNotLoaded(TerrainRasterLevel *, RasterTile*);

	protected:
		RasterDataQeq		detail_;
		RasterLoader *		loader_ = nullptr;
	private:
		RasterResource *	resource_ = nullptr;
	};
}

#endif
