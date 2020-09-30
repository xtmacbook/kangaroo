#include "rasterTile.h"
#include "rasterSource.h"
#include "rasterUpdater.h"
#include "rasterLevel.h"
#include "renderRasterLevel.h"
#include "helpF.h"
#include "rasterLoader.h"
#include "shader.h"
#include "texture.h"

#include <algorithm>

namespace scene
{

	RasterUpdater::RasterUpdater()
	{
		
	}

	RasterUpdater::~RasterUpdater()
	{

	}

	RasterUpdater::RasterUpdater( int w, int h):loader_(new RasterLoader(&detail_))
	{
		updatePass_ = new internal::UpdatePass();
		normalPass_ = new internal::ComputeNormalPass;
		samplePass_ = new internal::SamplePass;

		updatePass_->width_ = w;
		updatePass_->height_ = h;

		normalPass_->width_ = w;
		normalPass_->height_ = h;

		samplePass_->width_ = w;
		samplePass_->height_ = h;

		updatePass_->initStateAndObjs("terrain/clipmappings.glsl");
		normalPass_->initStateAndObjs("terrain/clipmapComputeNormals.glsl");
		samplePass_->initStateAndObjs("terrain/clipmapUpsample.glsl");

		//makecurrent
		Base::source_location l("rasterUpdate","rasterUpdate",27);
		//loader_->start(l);
	}

	void RasterUpdater::requestTileResidency(RenderRasterLevel * rl, RasterLevel* level, const RasterExtent* nextExtent)
	{
		std::vector<RasterTileRegion> rasterTileRegions;
		level->getTilesInExtent(nextExtent->left_, nextExtent->buttom_, nextExtent->right_, nextExtent->top_, rasterTileRegions);

		for (int i = 0; i < rasterTileRegions.size(); i++)
		{
			if (detail_.loadedTiles_.find(rasterTileRegions[i].title()->identifier()) == detail_.loadedTiles_.end())
			{
				requestTileLoad(rl, rasterTileRegions[i].title());
			}
		}
	}

	void RasterUpdater::requestTileLoad(RenderRasterLevel *rl, RasterTile *title)
	{
		TileLoadRequest* request = nullptr;
		RasterDataQeq::LOADINGCITER iter =  detail_.loadingTiles_.find(title->identifier());
		
		bool noExist = (iter == detail_.loadingTiles_.cend());
		if (noExist)
		{
			request = new TileLoadRequest();
			request->level_ = rl;
			request->title_ = title;
		}
		
		detail_.requestListMutex_.lock();
		
		if (!noExist)
		{
			if (!request->isNode_)
			{
				return;//now is loading this request by other thread
			}
			detail_.requestList_.removeRequest(request);
		}
		
		detail_.requestList_.addRequest(request);

		detail_.requestListMutex_.unlock();
		detail_.requestCondition_.notify_all();

		detail_.loadingTiles_[title->identifier()] = request;

	}

	int RasterUpdater::getDoneReqs(TileLoadRequest * rq, std::vector<TileLoadRequest*>*doneQ)
	{
		detail_.loadedTiles_[rq->title_->identifier()] = rq->data_;
		detail_.loadingTiles_.erase(rq->title_->identifier());
		doneQ->push_back(rq);
		return 0;
	}

	void RasterUpdater::renderTileToLevelTexture(const RasterTileRegion*region, RenderRasterLevel *level, const TileData*data)
	{
		const RasterExtent*nextExtent =  &level->nextExtend();

		int clipmapSize = (nextExtent->right_ - nextExtent->left_ + 1);
		V2i originInTextures = level->originTexture();

		int destWest = (originInTextures.x + (region->title()->extent()->left_ + region->left() - nextExtent->left_)) % clipmapSize;
		int destSouth = (originInTextures.y + (region->title()->extent()->buttom_ + region->buttom() - nextExtent->buttom_)) % clipmapSize;

		int width = region->right() - region->left()+ 1;
		int height = region->top() - region->buttom() + 1;

		updatePass_->shader_->turnOn();
		updatePass_->shader_->setFloat2(updatePass_->updateSize_, width,height);
		updatePass_->shader_->setFloat2(updatePass_->sourceOrigin_, region->left(), region->buttom());
		updatePass_->shader_->setFloat2(updatePass_->destinationOffset_, destWest, destSouth);

		updatePass_->draw(data->texture_);

		updatePass_->shader_->turnOff();

	}

	void RasterUpdater::applyIfNotLoaded(RenderRasterLevel *level, RasterTile*tile)
	{
		RasterDataQeq::TEXTUREITER loadTexture = detail_.loadedTiles_.find(tile->identifier());
		if (loadTexture != detail_.loadedTiles_.end() || loadTexture->second)
		{
			const TileData* data = loadTexture->second;
			applyNewTile(tile,level);
		}
	}

	void RasterUpdater::applyNewData()
	{
		std::vector<TileLoadRequest*> doneQueue;
		detail_.doneQue_.queueCallback_ =  Base::NewPermanentCallback(this,&RasterUpdater::getDoneReqs,&doneQueue);
		detail_.doneQue_.processQueue();
		for each (TileLoadRequest* req in doneQueue)
		{
			applyNewTile(req->title_, req->level_);
		}
		delete detail_.doneQue_.queueCallback_;
	}

	void RasterUpdater::applyNewTile(RasterTile * title, RenderRasterLevel*level)
	{
		const RasterExtent& nextExtent = level->nextExtend();
		
		const RasterLevel* rasterLevel = title->level();

		RasterExtent intersection;

		intersection.left_= math::Max(nextExtent.left_,title->extent()->left_);
		intersection.buttom_ = math::Max(nextExtent.buttom_, title->extent()->buttom_);
		intersection.right_= math::Min(nextExtent.right_, title->extent()->right_);
		intersection.top_ = math::Min(nextExtent.top_, title->extent()->top_);

		if (intersection.right_ - intersection.left_ > 0 && intersection.top_ - intersection.buttom_ > 0)
		{
			update(&intersection, level, rasterLevel);

			// Recurse on child tiles if they're NOT loaded.  Unloaded children will use data from this tile.
			const RenderRasterLevel* finer = level->finerLevel();
			if (finer != nullptr)
			{
				/*applyIfNotLoaded(finer, tile->);
				applyIfNotLoaded(finer, tile->SoutheastChild);
				applyIfNotLoaded(finer, tile->NorthwestChild);
				applyIfNotLoaded(finer, tile->NortheastChild);*/
			}
		}
	}

	void RasterUpdater::update(const RasterExtent* update, RenderRasterLevel*rlevel, const RasterLevel*level)
	{
		std::vector<RasterExtent> extents;
		splitUpdateToAvoidWrapping(update, rlevel, extents);

		for (int i = 0; i < extents.size(); i++)
		{
			const RasterExtent&extent = extents[i];
			std::vector<RasterTileRegion> tileRegions;
			level->getTilesInExtent(extent.left_, extent.buttom_, extent.right_, extent.top_, tileRegions);

			for (int j = 0; j <  tileRegions.size(); j++)
			{
				const RasterTileRegion&region = tileRegions[j];

				RasterDataQeq::TEXTUREITER loadTexture = detail_.loadedTiles_.find(region.title()->identifier());
				if (loadTexture != detail_.loadedTiles_.end())
				{
					const TileData* data = loadTexture->second;
					renderTileToLevelTexture(&region,rlevel,data);
				}
			}
		}
	}

	void RasterUpdater::stopUpdater()
	{
		detail_.requestListMutex_.lock();
		detail_.stopRequest_ = true;
		detail_.requestListMutex_.unlock();
	}

	void RasterUpdater::splitUpdateToAvoidWrapping(const RasterExtent* update, const RenderRasterLevel* rl, std::vector<RasterExtent>&extents)
	{
		const V2i& origin = rl->originTexture();
		const RasterExtent& extent = rl->nextExtend();

		int clipmapSizeX = extent.right_ - extent.left_ + 1;
		int clipmapSizeY = extent.top_ - extent.buttom_ + 1;

		int left = (origin.x + (update->left_ - extent.left_)) % clipmapSizeX;
		int right = (origin.x + (update->right_ - extent.right_)) % clipmapSizeX;
		int buttom = (origin.y + (update->buttom_ - extent.buttom_)) % clipmapSizeY;
		int top = (origin.y + (update->top_ - extent.top_)) % clipmapSizeY;

		if (right < left && top < buttom)
		{
			// Horizontal AND vertical wrap
			extents.push_back({update->left_,
				extent.left_ + (clipmapSizeX - origin.x - 1),
				extent.buttom_ + (clipmapSizeY - origin.y - 1),
				update->top_
			});

			extents.push_back({
				extent.left_ + clipmapSizeX - origin.x,
				update->right_,
				extent.buttom_ + (clipmapSizeY - origin.y - 1),
				update->buttom_,
			});

			extents.push_back({
				update->left_,
				extent.left_ + (clipmapSizeX - origin.x - 1),
				update->top_,
				extent.buttom_ + clipmapSizeY - origin.y
			});
 
			extents.push_back({
				extent.left_ + clipmapSizeX - origin.x,
				update->right_,
				update->top_,
				extent.buttom_ + clipmapSizeY - origin.y
			});
		}
		else if (right < left)
		{
			// Horizontal wrap
			extents.push_back({
				update->left_,
				extent.left_ + (clipmapSizeX - origin.x - 1),
				update->top_,
				update->buttom_
			
			});
			extents.push_back({
				extent.left_ + clipmapSizeX - origin.x,
				update->right_,
				update->top_,
				update->buttom_
			});
		}
		else if (top < buttom)
		{
			// Vertical wrap
			extents.push_back({
				update->left_,
				update->right_,
				extent.buttom_ + (clipmapSizeY - origin.y - 1),
				update->buttom_
			});
			extents.push_back({
				update->left_,
				update->right_,
				update->top_,
				extent.buttom_ + clipmapSizeY - origin.y
			});
		}
		else
		{
			extents.push_back(*update);
		}
	}


}
