#include "mesh.h"
#include "log.h"
#include <algorithm>

Mesh::Mesh(VERTEX_TYPE vertextype) :vertex_type_(vertextype)
{

}
Mesh::~Mesh()
{

}

base::AABB IMesh::getBOX() const
{
	return box_;
}

void IMesh::setBox(const AABB& box)
{
	box_ = box;
}

void Mesh::computeBox()
{
	for (int j = 0; j < vcount_; j++)
	{
		Vertex_P* p = (Vertex_P*)getVertex(j);
		box_.expandBy(V4f(p->position_, 1.0));
	}
}

void Mesh::addVertex(const Vertex_P* vertex)
{
	int esize = getVertexElementSize(vertex_type_);
	memcpy(current_, vertex, esize);
	current_ += esize;
}


void Mesh::addIndices(int32 indice)
{
	indices_.push_back(indice);
}

void Mesh::createMesh(int vcount)
{
	vertices_ = (uint8*)createMeshV(vcount);
	current_ = vertices_;
	vcount_ = vcount;
}

void* Mesh::createMeshV(int vcount)
{
	int vertexSize = getVertexElementSize(vertex_type_);
	return new uint8[vcount * vertexSize];
}

void Mesh::clear()
{
	uint8* vd = (uint8*)vertices_;
	delete[] vd;
	box_.init();
	indices_.clear();
	vcount_ = 0;
	current_ = nullptr;

	t_indices_ = -1; //texture indices
}


unsigned int Mesh::vSize()const
{
	return vcount_;
}


unsigned int Mesh::iSize()const
{
	return indices_.size();
}

int Mesh::getVertexElementSize(VERTEX_TYPE vertex_type)
{
	switch (vertex_type)
	{
	case VERTEX_POINTS:
		return sizeof(Vertex_P);
	case VERTEX_POINTS_TEXTURE:
		return sizeof(Vertex_PT);
	case VERTEX_POINTS_COLOR:
		return sizeof(Vertex_PC);
	case VERTEX_POINTS_NORMAL:
		return sizeof(Vertex_PN);
	case VERTEX_POINTS_TBN:
		return sizeof(Vertex_PTBN);
	case VERTEX_POINTS_NORMAL_TEXTURE:
		return sizeof(Vertex_PNT);
	case VERTEX_POINTS_NORMAL_TEXTURE_TBN:
		return sizeof(Vertex_PNTTBN);
	default:
		PRINT_ERROR("error getVertexElementSize for the vertex type !\n ");
		return 0;

	}
}


uint8* Mesh::getVertex(int i) const
{
	int vertexSize = getVertexElementSize(vertex_type_);
	uint8* p8 = (uint8*)vertices_;
	p8 += i * vertexSize;
	return p8;
}

void Mesh::setTextureIndices(int i)
{
	t_indices_ = i;
}

int Mesh::textureIndic() const
{
	return t_indices_;
}

void Mesh::preDraw(Shader*shader)
{

}
