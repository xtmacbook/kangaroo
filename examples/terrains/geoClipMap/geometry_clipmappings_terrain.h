#include <vector>

#include "footPrints.h"

using namespace math;
class Shader;
class CameraBase;
//The degenerate triangles which connect level N and N + 1 are drawn using vertices from level N. 
//Trim regions which connect level N and N + 1 are drawn using vertices from level N + 1.

/************************************************************************/
/*  shadow
	blend
	frustum culling
	terrain compression
	terrain synthesis (noise)

	mask weights (¨C1/16,
	9/16, 9/16, ¨C1/16) [Dyn et al 1987]. This upsampling
*/
/************************************************************************/

class MapLayerManager;
class TerrainUpdater;

struct RenderShaderIndex
{
	int	uView_;
	int uProj_;
	int uMode_;

	int ulevelOffset_;

	int ulocalOffset_;
	int uScale_;


	int heightMap_;
	int diffMap_;
};



class ClipmapTerrain
{
public:

	ClipmapTerrain(int cmapSize, int level);
	virtual ~ClipmapTerrain();

	bool  init( );
	void  update(const CameraBase*);
	void  render(const CameraBase*);

private:
	bool initShader();

	/*inline float getTrueHeightAtPoint(int iX, int iZ)
	{
		return ((float*)heightMap_.data_)[iZ * heightMap_.width_ + iX];
	}*/

	Shader* baseShader_ = nullptr;
	RenderShaderIndex		state_;
	FootPrints				footprints_;
	TerrainUpdater* terrainUpdater_ = nullptr;
	MapLayerManager* layerManager_;
	int						cmapSize_;
	int						level_;
};
