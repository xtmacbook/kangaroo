/************************************************************************/
/* 2020.08.26 19:21 xiangtao                                                                     */
/************************************************************************/

#ifndef ____RASTER_UPDATER__
#define ____RASTER_UPDATER__

#include "decl.h"
#include "synchron.h"
#include "dataQequest.h"
#include "messQue.h"

#include <map>
#include <list>
#include <vector>


class Quad;
class Shader;
class Texture;
class FrameBufferObject;

namespace scene
{
	class RasterResource;
	class RasterLevel;
	class RasterTile;
	class RenderRasterLevel;
	class RasterLoader;

	struct TileData;

	struct LIBENIGHT_EXPORT TileLoadRequest
	{
		RasterTile *			title_ = nullptr;
		TileData    *			data_ = nullptr;
		RenderRasterLevel*		level_ = nullptr;
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
		std::map <const RasterTileIdentifier*, TileLoadRequest*, TileIdentifierComp> loadingTiles_;
		std::map <const RasterTileIdentifier*, TileData*, TileIdentifierComp> loadedTiles_;
		DataQequest<TileLoadRequest> requestList_;
		
		typedef std::map <const RasterTileIdentifier*, TileLoadRequest*, TileIdentifierComp>::const_iterator LOADINGCITER;
		typedef std::map <const RasterTileIdentifier*, TileData*, TileIdentifierComp>::const_iterator TEXTUREITER;

		bool					  stopRequest_ = false;

		Base::SameProcessMutex	  requestListMutex_;
		Base::ConditionVar        requestCondition_;

		Base::MessageQueues<int, TileLoadRequest> doneQue_;
	};


	namespace internal
	{

		class Pass
		{
		public:

			Pass();

			virtual ~Pass();

			virtual void initStateAndObjs(const char*shde);

			virtual void initUniform() = 0;

			void	 setQuad(Quad*);

			virtual void draw(Texture *);

			Shader *	shader_;
			Quad*		quad_;


			int texture0_;
			int viewportOrthographicMatrix_;

			FrameBufferObject* fbuffer_;

			int width_;
			int height_;
		};


		class UpdatePass :public Pass
		{
		public:

			UpdatePass();

			virtual void initUniform();

			int sourceOrigin_;
			int updateSize_;
			int destinationOffset_;
		};


		class SamplePass :public Pass
		{
		public:

			SamplePass();

			virtual void initUniform();

			int sourceOrigin_;
			int updateSize_;
			int destinationOffset_;
			int oneOverTextureSize_;

		};

		class ComputeNormalPass :public Pass
		{
		public:

			ComputeNormalPass();

			virtual void initUniform();

			int updateSize_;
			int origin_;
			int oneOverHeightMapSize_;
			int  heightExaggeration_;
			int  postDelta_;
		};
	}

	class LIBENIGHT_EXPORT RasterUpdater
	{
	public:
		RasterUpdater();

		RasterUpdater(int w,int h);

		virtual ~RasterUpdater();

		void requestTileResidency(RenderRasterLevel * ,RasterLevel*,const RasterExtent*);

		void requestTileLoad(RenderRasterLevel * ,RasterTile * );

		void	applyNewData();

		void	applyNewTile(RasterTile * title, RenderRasterLevel*);
		
		void	update(const RasterExtent* update,RenderRasterLevel*, const RasterLevel*);

		void	stopUpdater();

		int		getDoneReqs(TileLoadRequest * rq, std::vector<TileLoadRequest*>*doneQ);

		void	renderTileToLevelTexture(const RasterTileRegion*, RenderRasterLevel *, const TileData*);
	//private:
		void	applyIfNotLoaded(RenderRasterLevel *, RasterTile*);

		void	splitUpdateToAvoidWrapping(const RasterExtent*, const RenderRasterLevel*,std::vector<RasterExtent>&);

		RasterResource * resource_ = nullptr;

		RasterDataQeq		detail_;

		RasterLoader *		loader_ = nullptr;

		internal::UpdatePass*		updatePass_ = nullptr;
		internal::SamplePass*		samplePass_ = nullptr;
		internal::ComputeNormalPass*		normalPass_ = nullptr;
	};
}

#endif
