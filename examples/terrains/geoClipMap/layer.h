
#include <type.h>
#include <vector>

#include <renderRasterLevel.h>

using namespace math;

struct BlockRenderState
{
	V2f	  offset_;
	float scale_;
};

class FootPrints;
class Shader;
class CameraBase;
struct RenderShaderIndex;

namespace scene
{
	class RasterUpdater;
	class RasterResource;
}

class MapLayerManager;

class TerrainClipLevel :public scene::RenderRasterLevel
{
public:
	TerrainClipLevel(MapLayerManager * );
	virtual ~TerrainClipLevel();

	virtual void init(const V3f&center) ;
	virtual void render(Shader *, FootPrints*);
	virtual void update(const CameraBase* camera, scene::RasterUpdater *)  = 0;
	virtual	void updateExtentAndTextureOrigin();
	void		setLevel(int level);
	void		setCenter(const V2f&);
	void		setScale(int);
	void		setLevelOffset(const V2f&);

	V2i			levelSize()const;
	V2f			leftTop()const; //origin for local level
	V2f			center()const;
	void		updateCenter(const V3f&eye, int *rows_to_update, int *cols_to_update);
protected:

	V2f			level_center_;
	int			level_;			//current lay level
	int			scale_;			//rect  //current lay scale
	V2f			level_offset_; // from the level-1

	MapLayerManager *		manager_ = nullptr;
};

class TerrainClipOutLevel :public TerrainClipLevel
{
public:

	TerrainClipOutLevel(MapLayerManager*);

	virtual ~TerrainClipOutLevel();

	virtual void render(Shader *, FootPrints*);

	virtual void update(const CameraBase* camera, scene::RasterUpdater *);

private:
	void updateBlocks(const CameraBase* eye);
	void updateFixUps(const CameraBase* eye);
	void updateTrims(const CameraBase* eye);
	void updateDegenerateTriangels(const CameraBase* eye);

	virtual void renderBlocks(Shader *, FootPrints*);
	virtual void renderFixUps(Shader *, FootPrints*);
	virtual void renderTrims(Shader *, FootPrints*);
	virtual void renderDegenerateTriangels(Shader *, FootPrints*);

private:

	std::vector<BlockRenderState>   blockModelState_;

	std::vector<BlockRenderState>   fixUPVerticalModelState_;
	std::vector<BlockRenderState>   fixUPHorizontalModelState_;

	std::vector<BlockRenderState>	interiorVerticalTrimModelState_;
	std::vector<BlockRenderState>	interiorHorizontalTrimModelState_;

	std::vector<BlockRenderState>	outTopDegenerateTrianglesmModelState_;
	std::vector<BlockRenderState>	outRightDegenerateTrianglesmModelState_;
	std::vector<BlockRenderState>	outButtomDegenerateTrianglesmModelState_;
	std::vector<BlockRenderState>	outLeftDegenerateTrianglesmModelState_;

};

class TerrainClipInnermostLevel : public TerrainClipLevel
{
	public:
		TerrainClipInnermostLevel(MapLayerManager*);
		virtual ~TerrainClipInnermostLevel();

		virtual void update(const CameraBase* camera, scene::RasterUpdater*);
		virtual void render(Shader *, FootPrints*);
private:
	BlockRenderState   state_;

};
class MapLayerManager
{
public:

	MapLayerManager(int cmapSize, int level,scene::RasterResource*);

	virtual ~MapLayerManager();

	void setShaderLocations(const RenderShaderIndex*);

	void init();

	void render(Shader*, FootPrints *);

	void update(const CameraBase* eye, scene::RasterUpdater * );

	int getCmapSize()const;

	int getLevelNum()const;

	inline const RenderShaderIndex* getShaderIndex()const { return shaderIndex_; }

	void  initCorsestLevel(scene::RasterUpdater *);

	inline TerrainClipLevel*		getLevel(const unsigned int& level)
	{
		if (level == 0)
			return innermostLayer_;
		else
			return layers_[level - 1];
	}

private:
	void updateBlockMinMaxVal();

private:
	int								num_level_;
	int								cmapSize_;

	TerrainClipOutLevel **			layers_ = nullptr;
	TerrainClipInnermostLevel *		innermostLayer_ = nullptr;
	const RenderShaderIndex*		shaderIndex_ = nullptr;

	scene::RasterResource*					resource_ = nullptr;
};
