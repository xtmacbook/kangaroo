
#include "layer.h"
#include "footPrints.h"
#include <iostream>
#include <cameraBase.h>
#include <shader.h>
#include "terrainUpdater.h"
#include "rasterLevel.h"
#include "rasterLoader.h"
/*
	TerrainMapLayer
*/


TerrainClipLevel::TerrainClipLevel(MapLayerManager*m):manager_(m)
{}

TerrainClipLevel::~TerrainClipLevel() {}

void TerrainClipLevel::init(const V3f&center)
{
}

void TerrainClipLevel::render(Shader *, FootPrints*) {}


void TerrainClipLevel::updateExtentAndTextureOrigin()
{
	//level_center_ to level xindex

	V2d centerIndex;

	centerIndex.x = raster_->xtoIndex(level_center_.x);
	centerIndex.y = raster_->ytoIndex(level_center_.y);

	int n = manager_->getCmapSize();
	int half = (n - 1) / 2;
	nextExtent_.left_ = centerIndex.x - half;
	nextExtent_.right_ = centerIndex.x + half;
	nextExtent_.top_ = centerIndex.y + half;
	nextExtent_.buttom_ = centerIndex.y - half;

	updateTextureOrigin(n,n);
}

void TerrainClipLevel::setLevel(int level)
{
	level_ = level;
}

void TerrainClipLevel::setCenter(const V2f&center)
{
	level_center_ = center;
}

void TerrainClipLevel::setScale(int scale)
{
	scale_ = scale;
}

void TerrainClipLevel::setLevelOffset(const V2f&lf)
{
	level_offset_ = lf;
}


math::V2i TerrainClipLevel::levelSize() const
{
	int edgeLen = pow(2, level_)  * (manager_->getCmapSize() - 1);
	return V2i(edgeLen, edgeLen);
}

math::V2f TerrainClipLevel::leftTop() const
{
	V2i size = levelSize();
	return (level_center_ - V2f(size.x * 0.5f, size.y * 0.5f));
}

math::V2f TerrainClipLevel::center() const
{
	return level_center_;
}

void TerrainClipLevel::updateCenter(const V3f&eye, int *rows_to_update, int *cols_to_update)
{
	*rows_to_update = 0;
	*cols_to_update = 0;

	int ex = floor(eye.x);
	int ez = floor(eye.z);

	int scale = (int)pow(2, level_);

	// if the viewpoint has moved outside the center box, shift the box until it
	// contains the viewpoint, keeping track of how many rows/cols to update
	while (ex >= level_center_.x + scale)
	{
		level_center_.x += 2 * scale;
		*cols_to_update += 2;
	}
	while (ez >= level_center_.y + scale)
	{
		level_center_.y += 2 * scale;
		*rows_to_update += 2;
	}
	while (ex < level_center_.x - scale)
	{
		level_center_.x -= 2 * scale;
		*cols_to_update -= 2;
	}
	while (ez < level_center_.y - scale)
	{
		level_center_.y -= 2 * scale;
		*rows_to_update -= 2;
	}
}


/*
TerrainCMapLayer
*/

TerrainClipOutLevel::TerrainClipOutLevel(MapLayerManager*m) :TerrainClipLevel(m)
{

}

TerrainClipOutLevel::~TerrainClipOutLevel()
{

}

void TerrainClipOutLevel::renderBlocks(Shader *shader, FootPrints*footPrint) 
{
	if (!blockModelState_.empty())
	{
		footPrint->renderBlocks(shader, &(blockModelState_[0]), blockModelState_.size());
	}

}
void TerrainClipOutLevel::renderFixUps(Shader *shader, FootPrints*footPrint) 
{
	//fixups
	if (!fixUPHorizontalModelState_.empty() || !fixUPVerticalModelState_.empty())
	{
		footPrint->preRenderFixUP();
		if (!fixUPHorizontalModelState_.empty())
			footPrint->renderFixUP(shader, &(fixUPHorizontalModelState_[0]), fixUPHorizontalModelState_.size(), false);
		if (!fixUPVerticalModelState_.empty())
			footPrint->renderFixUP(shader, &(fixUPVerticalModelState_[0]), fixUPVerticalModelState_.size(), true);
		footPrint->postRenderFixUP();
	}
}
void TerrainClipOutLevel::renderTrims(Shader *shader, FootPrints*footPrint) 
{
	//InteriorTrims
	if (!interiorVerticalTrimModelState_.empty() || !interiorHorizontalTrimModelState_.empty())
	{
		footPrint->preRenderTrim();
		if (!interiorVerticalTrimModelState_.empty())
			footPrint->renderTrim(shader, &(interiorVerticalTrimModelState_[0]), true);
		if (!interiorHorizontalTrimModelState_.empty())
			footPrint->renderTrim(shader, &(interiorHorizontalTrimModelState_[0]), false);
		footPrint->postRenderTrim();
	}

}
void TerrainClipOutLevel::renderDegenerateTriangels(Shader *shader, FootPrints*footPrint) 
{

	if (!outTopDegenerateTrianglesmModelState_.empty() ||
		!outRightDegenerateTrianglesmModelState_.empty() ||
		!outButtomDegenerateTrianglesmModelState_.empty() ||
		!outLeftDegenerateTrianglesmModelState_.empty())
	{
		footPrint->preRenderDegenerateTriangle();
		if (!outTopDegenerateTrianglesmModelState_.empty())
		{
			footPrint->renderDegenerateTriangle(shader, &outTopDegenerateTrianglesmModelState_[0], 0);
		}
		if (!outRightDegenerateTrianglesmModelState_.empty())
		{
			footPrint->renderDegenerateTriangle(shader, &outRightDegenerateTrianglesmModelState_[0], 1);
		}
		if (!outButtomDegenerateTrianglesmModelState_.empty())
		{
			footPrint->renderDegenerateTriangle(shader, &outButtomDegenerateTrianglesmModelState_[0], 2);
		}
		if (!outLeftDegenerateTrianglesmModelState_.empty())
		{
			footPrint->renderDegenerateTriangle(shader, &outLeftDegenerateTrianglesmModelState_[0], 3);
		}
		footPrint->postRenderDegenerateTrianglem();
	}
}

void TerrainClipOutLevel::update(const CameraBase* camera, scene::RasterUpdater*updater)
{
	updateBlocks(camera);
	updateFixUps(camera);
	updateTrims(camera);
	if (level_ != (manager_->getLevelNum() - 1)) updateDegenerateTriangels(camera);
}

void TerrainClipOutLevel::updateBlocks(const CameraBase* camera)
{
	if (level_ == 0) return;

	blockModelState_.clear();

	for (int z = 0; z < 4; z++)
	{
		for (int x = 0; x < 4; x++)
		{

			V2i offset_from_center_unit;
			// draw a 4x4 set of tiles. cut out the middle 2x2 unless we're at the finest level

			if ((level_ != 0) && (x == 1 || x == 2) && (z == 1 || z == 2))
				continue;
			offset_from_center_unit = TerrainBlock::block_offset_from_level_center_[z * 4 + x];


		/*	V2f offset_from_center = offset_from_center_unit * scale_;
			V2f block_top_left_WorldPos = level_center_ + offset_from_center;
			V2f minXZ = block_top_left_WorldPos;
			V2f maxXZ = V2f(blockSize_.x, blockSize_.y) + block_top_left_WorldPos;

			CameraBase::FRUSTUMINBOXSTATE state = camera->aabbFrustumTest(V3f(minXZ.x, -100.0, minXZ.y), V3f(maxXZ.x, 100.0, maxXZ.y));*/

			//if (state == CameraBase::INSIDE || state == CameraBase::INTERSECT)
			{
				BlockRenderState state{ offset_from_center_unit,scale_ };
				blockModelState_.push_back(state);
			}
			//else
			{
				int a = 3;
			}

		}
	}
}

void TerrainClipOutLevel::updateFixUps(const CameraBase* eye)
{
	if (level_ == 0) return;

	V2f localOffset;
	//fixups
	fixUPHorizontalModelState_.clear();
	fixUPVerticalModelState_.clear();
	//vertical
	fixUPVerticalModelState_.push_back({ FixUp::local_offset_[0] ,(float)scale_ });
	fixUPVerticalModelState_.push_back({ FixUp::local_offset_[1] ,(float)scale_ });
	//Horizontal
	fixUPHorizontalModelState_.push_back({ FixUp::local_offset_[2] ,(float)scale_ });
	fixUPHorizontalModelState_.push_back({ FixUp::local_offset_[3] ,(float)scale_ });
}

void TerrainClipOutLevel::updateTrims(const CameraBase* eye)
{
	if (level_ == 0) return;

	V2f localOffset;
	interiorVerticalTrimModelState_.clear();
	interiorHorizontalTrimModelState_.clear();

	int index = 0;

	if (level_offset_.x > 0 && level_offset_.y > 0) index = 1;
	else if (level_offset_.x > 0 && level_offset_.y < 0) index = 3;
	else if (level_offset_.x < 0 && level_offset_.y > 0) index = 0;
	else if (level_offset_.x < 0 && level_offset_.y < 0) index = 2;

	V4i offset = InteriorTrim::local_offset_[index];
	interiorVerticalTrimModelState_.push_back({ V2i(offset.x, offset.y) ,(float)scale_ });
	interiorHorizontalTrimModelState_.push_back({ V2i(offset.z, offset.w),(float)scale_ });
}

void TerrainClipOutLevel::updateDegenerateTriangels(const CameraBase* eye)
{
	V2f localOffset;
	//DegenerateTriangles
	outTopDegenerateTrianglesmModelState_.clear();
	outRightDegenerateTrianglesmModelState_.clear();
	outButtomDegenerateTrianglesmModelState_.clear();
	outLeftDegenerateTrianglesmModelState_.clear();

	outTopDegenerateTrianglesmModelState_.push_back({  V2f(0.0,0.0) ,(float)scale_ });
	outRightDegenerateTrianglesmModelState_.push_back({ V2f(0.0,0.0)  ,(float)scale_ });
	outButtomDegenerateTrianglesmModelState_.push_back({ V2f(0.0,0.0)  ,(float)scale_ });
	outLeftDegenerateTrianglesmModelState_.push_back({ V2f(0.0,0.0) ,(float)scale_ });
}

/*
TerrainInnermostLayer
*/
TerrainClipInnermostLevel::TerrainClipInnermostLevel(MapLayerManager*m) :TerrainClipLevel(m)
{

}

TerrainClipInnermostLevel::~TerrainClipInnermostLevel()
{

}


void TerrainClipInnermostLevel::update(const CameraBase* camera, scene::RasterUpdater*updater)
{
	state_.offset_ = V2f(0.0,0.0);
	state_.scale_ = scale_;
}


/*
MapLayerManager
*/
MapLayerManager::MapLayerManager(int cmapSize, int level,scene::RasterResource *r) :cmapSize_(cmapSize),
resource_(r),num_level_(level)
{
}

MapLayerManager::~MapLayerManager()
{
	if (layers_)
	{
		for (int i = 0; i < num_level_ - 1; i++)
		{
			TerrainClipOutLevel * layer = layers_[i];
			delete layer;
		}
		delete[] layers_;
	}
	if (innermostLayer_)
		delete innermostLayer_;
}


void MapLayerManager::setShaderLocations(const RenderShaderIndex*index)
{
	shaderIndex_ = index;
}

/************************************************************************/
/* the terrain plane is xz plane

---------->x
|
|
|
|
|
\ /
z

*/
/************************************************************************/
void MapLayerManager::init()
{
	V3f center(0.0f, 0.0f, 0.0f);

	layers_ = new TerrainClipOutLevel*[num_level_ - 1]; //0->num_level_-1

	for (int i = 0; i < num_level_ - 1; i++)
		layers_[i] = new TerrainClipOutLevel(this);

	innermostLayer_ = new TerrainClipInnermostLevel(this);

	TerrainClipLevel * layer;

	scene::TerrainResource * resource = dynamic_cast<scene::TerrainResource*>(resource_);
	for (int i = 0; i < num_level_; i++)
	{
		layer = getLevel(i);
		layer->setRasterLevel(resource->level(i));
		if (i != 0) layer->setFiner(getLevel(i - 1));
		if (i != num_level_ - 1) layer->setCoarser(getLevel(i + 1));
	}

	V2f* level_centers = new V2f[num_level_];

	int sx = (int)floor(center.x);
	int sz = (int)floor(center.z);

	for (int level = 0; level < num_level_; level++)
	{

		layer = getLevel(level);
		layer->setLevel(level);
		layer->setScale(pow(2, level));

		int mod;
		if (level % 2 == 0) mod = 1;
		else mod = -1;

		if (level == 0)
		{
			level_centers[level].x = sx;
			level_centers[level].y = sz;
		}
		else
		{
			int scale = (int)pow(2, level - 1);
			level_centers[level].x = level_centers[level - 1].x + mod *  scale;
			level_centers[level].y = level_centers[level - 1].y - mod *  scale;
		}

		layer->setCenter(level_centers[level]);

		V2f level_off(0.0f, 0.0f);
		if (level != 0)
		{
			level_off.x = 0.5*(level_centers[level].x - level_centers[level - 1].x);
			level_off.y = 0.5*(level_centers[level].y - level_centers[level - 1].y);
		}
		layer->setLevelOffset(level_off);
	}


	for (int level = 0; level < num_level_; level++)
		getLevel(level)->init(center);

	delete[] level_centers;
}

void MapLayerManager::render(Shader*shader, FootPrints * footPrint)
{
	TerrainClipLevel * layer;
	for (int level = 0; level < num_level_; level++)
	{
		getLevel(level)->render(shader, footPrint);
	}
}

void MapLayerManager::update(const CameraBase* camera, scene::RasterUpdater * updater)
{
	TerrainClipLevel * layer;

	//udpate center level
	for (int level = 0; level < num_level_; level++)
	{
		int rows_update, cols_update;
		getLevel(level)->updateCenter(camera->getPosition(), &rows_update, &cols_update);
	}

	//update level offset
	for (int level = 1; level < num_level_; level++)
	{
		V2f level_off(0.0f, 0.0f);
		level_off = getLevel(level)->center() - getLevel(level - 1)->center() ;
		getLevel(level)->setLevelOffset(level_off);
	}

	//update visiable FootPrints
	for (int level = 0; level < num_level_; level++)
	{
		getLevel(level)->update(camera,updater);
	}

	//update extent and texture origin
	for (int level = 0; level < num_level_; level++)
	{
		getLevel(level)->updateExtentAndTextureOrigin();
	}

	updater->loader_->run();

	//apply new raster Datas
	updater->applyNewData();


	//new required and update textures
	for (int level = 0; level < num_level_; level++)
	{
		//require new data
		//getLevel(level)->requestTileResidency(layer,layer->getRaster(),&layer->nextExtend());
		//update textures
		
	}
}


int MapLayerManager::getCmapSize() const
{
	return cmapSize_;
}


int MapLayerManager::getLevelNum() const
{
	return num_level_;
}


void MapLayerManager::initCorsestLevel(scene::RasterUpdater *updater)
{
	scene::RenderRasterLevel * renderRasterLevel = getLevel(num_level_ - 1);
	scene::RasterLevel * rl = renderRasterLevel->getRaster();
	scene::RasterExtent extent{0,rl->rasterXNum(),rl->rasterYNum(),0};
	updater->requestTileResidency(renderRasterLevel, renderRasterLevel->getRaster(),&extent);
}


void MapLayerManager::updateBlockMinMaxVal()
{

}


