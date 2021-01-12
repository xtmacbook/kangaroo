#ifndef _DYNAMIC_MESH_H_
#define _DYNAMIC_MESH_H_

#include "geometry.h"
#include "flashBuffer.h"
class LIBENIGHT_EXPORT DynamicMeshGeoemtry :public MeshGeometry
{
public:
	DynamicMeshGeoemtry(int flag, unsigned verticesSize, unsigned indiceSize, IRenderMeshObj_SP);
	virtual ~DynamicMeshGeoemtry();

	virtual void postRender();
	virtual void initGeometry();
	virtual void updateGeometry(const CameraBase* camera);
	virtual void clearMesh();
	virtual void setupMesh();

	inline int& RFVF() { return FVF_; }

	void     cpu2GpuTime(uint64*, uint64*);
	void     swapQueryTime();

	typedef Vertex_PNT  Vertex;

protected:

	FlashBuffer<Vertex>* vertexBuffer_ = NULL;
	FlashBuffer<unsigned int >* indicesBuffer_ = NULL;

	GLuint						vao_;
	int							FVF_ = FVF_XYZ;
	bool						have_indices_ = false;
	int							buffer_mode_;
	unsigned					vertices_buffer_pool_size_;
	unsigned					indices_buffer_pool_size_;

	unsigned					current_v_index_ = 0;
	unsigned					current_i_index_ = 0;
	bool						first_ = true;
};
typedef base::SmartPointer<DynamicMeshGeoemtry>	DMeshGeometry_Sp;

#endif
