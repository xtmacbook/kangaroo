#include <vector>
#include <type.h>

class Shader;

using namespace math;

struct RenderShaderIndex;
struct BlockRenderState;

struct TerrVertex
{
	V3f position_;
	V3f color_;
};

typedef  int32 INDICESTYPE;

class TerrainElement
{
public:

	virtual ~TerrainElement();

	virtual	void	destroryGlObj();

	virtual	void	init(int m);
	virtual void	initGlObj();

	virtual void	render(Shader*, const RenderShaderIndex*, BlockRenderState*);

	virtual	void	initGeometry(int size);

	virtual void	pushGLData();

	unsigned int	vao_;

	std::vector<TerrVertex>			vertices_;
	std::vector<INDICESTYPE>		indices_;
};

class TerrainBlock :public TerrainElement
{
public:
	virtual	void	initGeometry(int size);
	static  void	initOffset(int cmapsize);

	virtual	void	initGlObj();

	static V2i		block_offset_from_level_center_[16];

};

class FixUp :public TerrainElement
{
public:
	virtual	void	initGeometry(int size);

	static  void	initOffset(int cmapsize);

	virtual void	render(Shader*, const RenderShaderIndex*, BlockRenderState*, bool vertical);

	virtual	void	initGlObj();

	unsigned		int  indices_vbo_[2];

	static			V2i local_offset_[4];
};

class InteriorTrim :public TerrainElement
{
public:
	virtual	void	initGeometry(int size);

	static  void	initOffset(int cmapsize);

	virtual void	render(Shader*, const RenderShaderIndex*, BlockRenderState*, bool vertical);

	virtual	void	initGlObj();

	unsigned		int  indices_vbo_[2];

	int				num_vertices_indices_;

	static V4i		local_offset_[4]; // v4i :

};

class OutDegenerateTriangles :public TerrainElement
{
public:
	virtual	void	initGeometry(int size);

	virtual void	render(Shader*, const RenderShaderIndex*, BlockRenderState*, int flag);

	virtual	void	initGlObj();

	unsigned		int  indices_vbo_[4]; // top right buttom left

	static			V2i local_offset_[4]; // top right buttom left

};

class FinerEstCenter : public TerrainElement
{
public:
	virtual	void	initGeometry(int size);
	virtual	void	initGlObj();

	unsigned		int  indices_vbo_; // top right buttom left

};

class  FootPrints
{
public:
	void init(int cmapSize);

	void renderCenterLevel(Shader*, BlockRenderState*);
	void renderBlocks(Shader*, BlockRenderState*, int num);
	void preRenderFixUP();
	void postRenderFixUP();
	void renderFixUP(Shader*, BlockRenderState*, int num, bool vertical = true);
	void preRenderTrim();
	void postRenderTrim();
	void renderTrim(Shader*, BlockRenderState*, bool vertical = true);
	void preRenderDegenerateTriangle();
	void postRenderDegenerateTrianglem();
	void renderDegenerateTriangle(Shader*, BlockRenderState*, int flag);
	void setShaderLocations(const RenderShaderIndex*);

	TerrainBlock block_;
	FixUp						fixUP_;
	InteriorTrim				trim_;
	OutDegenerateTriangles		degenerates_;
	FinerEstCenter				center_;

	int							cmapSize_;
	const RenderShaderIndex* shader_idx_;
};
