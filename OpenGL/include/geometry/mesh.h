#ifndef mesh_h
#define mesh_h

#include "decl.h"
#include "type.h"
#include "texture.h"
#include "boundingBox.h"
#include "vertex.h"
#include "util.h"
#include <vector>

using namespace math;
using namespace base;

class Shader;
typedef struct
{
	unsigned int  count_;
	unsigned int  instanceCount_;
	unsigned int  firstIndex_;
	unsigned int  baseVertex_;
	unsigned int  baseInstance_;
}DrawElementsIndirectCommand;

typedef  struct {
	uint  count_;
	uint  instanceCount_;
	uint  first_;
	uint  baseInstance_;
} DrawArraysIndirectCommand;


class LIBENIGHT_EXPORT IMesh
{
public:
	virtual  AABB		getBOX()const;
	virtual void		setBox(const AABB&);
	inline AABB&		box() { return box_; }
	virtual void		computeBox() = 0;
protected:
	AABB						box_;
};

class LIBENIGHT_EXPORT Mesh :public BaseObject, public IMesh
{
public:

	Mesh(VERTEX_TYPE vertextype);
	virtual ~Mesh();

	virtual void		computeBox();

	void				addVertex(const Vertex_P*);
	void				addIndices(int32);

	void				createMesh(int vcount);

	inline uint8*		rVertex() { return vertices_; }
	inline const uint8* cVertex()const { return vertices_; }

	inline int32*		rIndice() { return &indices_[0]; }
	inline const int32* cIndice()const { return &indices_[0]; }

	void						clear();
	unsigned int				vSize()const;
	unsigned int				iSize()const;
	static int					getVertexElementSize(VERTEX_TYPE vertex_type);

	uint8*						getVertex(int i)const;
	void						setTextureIndices(int);

	int							textureIndic()const;

	virtual	void				preDraw(Shader*);

private:
	void* createMeshV(int vcount);

	uint8* vertices_ = nullptr;
	std::vector<int32>						indices_;

	int 									vcount_ = 0;

	uint8*									current_ = nullptr;
	VERTEX_TYPE								vertex_type_;

	int										t_indices_ = -1; //texture indices
};


typedef base::SmartPointer<Mesh>	Mesh_SP;

#endif
