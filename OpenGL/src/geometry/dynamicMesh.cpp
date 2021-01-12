#include "dynamicMesh.h"
#include "log.h"

#include <algorithm>

DynamicMeshGeoemtry::DynamicMeshGeoemtry(int flag, unsigned verticesSize, unsigned indiceSize,
	IRenderMeshObj_SP robj) :
	MeshGeometry(robj, VERTEX_POINTS_NORMAL_TEXTURE),
	buffer_mode_(flag),
	vertices_buffer_pool_size_(verticesSize),
	indices_buffer_pool_size_(indiceSize)
{
}

DynamicMeshGeoemtry::~DynamicMeshGeoemtry()
{
	if (vertexBuffer_)
		SAFTE_DELETE(vertexBuffer_);
	if (indicesBuffer_)
		SAFTE_DELETE(indicesBuffer_);
	if (vao_)
		glDeleteVertexArrays(1, &vao_);
}

void DynamicMeshGeoemtry::postRender()
{
	if (vertexBuffer_) vertexBuffer_->lockBuffer();
	if (indicesBuffer_) indicesBuffer_->lockBuffer();
}

void DynamicMeshGeoemtry::initGeometry()
{
	mesh_obj_->bind();

	vertexBuffer_ = new FlashBuffer<Vertex>(vertices_buffer_pool_size_);
	vertexBuffer_->setTarget(GL_ARRAY_BUFFER);
	vertexBuffer_->create_buffers(buffer_mode_);

	if (have_indices_)
	{
		indicesBuffer_ = new FlashBuffer<unsigned>(indices_buffer_pool_size_);
		indicesBuffer_->setTarget(GL_ELEMENT_ARRAY_BUFFER);
		indicesBuffer_->create_buffers(buffer_mode_);
	}

	vertexBuffer_->bind();
	setupVertexAttribute();
	vertexBuffer_->unbind();

	mesh_obj_->unBind();
}

void DynamicMeshGeoemtry::updateGeometry(const CameraBase* camera)
{
	current_v_index_ = 0;
	current_i_index_ = 0;

	/*for (int i = 0; i < meshs_.size(); i++)
	{
		IRenderMeshObj_SP irmsp;
		if (first_)
		{
			irmsp = createRenderMeshObj();
			irmsp->call_ = meshs_[i]->call_;
			mesh_obj_.push_back(irmsp);
		}
		else
			irmsp = mesh_obj_[i];

		irmsp->clear();

		setupMesh(meshs_[i], irmsp);
	}*/
	computeBoundingBox(NULL);
	if (first_) first_ = !first_;
}

void DynamicMeshGeoemtry::clearMesh()
{
	MeshGeometry::clearMesh();

	current_v_index_ = 0;
	current_i_index_ = 0;
}

void DynamicMeshGeoemtry::cpu2GpuTime(uint64* vt, uint64* it)
{
	if (vertexBuffer_) *vt = vertexBuffer_->timestamp();
	else *vt = 0;
	if (indicesBuffer_) *it = indicesBuffer_->timestamp();
	else *it = 0;
}

void DynamicMeshGeoemtry::swapQueryTime()
{
	if (vertexBuffer_) vertexBuffer_->swapQueryTime();
	if (indicesBuffer_) indicesBuffer_->swapQueryTime();
}

void DynamicMeshGeoemtry::setupMesh()
{
	mesh_obj_->bind();

	uint16 call = mesh_obj_->call();

	Vertex* vertexData = NULL;

	if (DRAW_ARRAYS == call)
	{
		MultRenderMeshObj* multRenderObj = (MultRenderMeshObj*)mesh_obj_.addr();
		assert(multRenderObj);
		ArraysRenderMeshObj* obj = (ArraysRenderMeshObj*)multRenderObj;
		assert(obj);

		int vertexSize = 0;
		int meshNum = meshs_.size();

		obj->first_ = new int[meshNum]; //delete
		obj->count_ = new int[meshNum]; //delete
		obj->drawcount_ = meshNum;

		for (int i = 0; i < meshNum; i++)
		{
			obj->first_[i] = vertexSize;
			vertexSize += obj->count_[i] = meshs_[i]->vSize();
		}

		if (current_v_index_ + vertexSize > vertexBuffer_->blockSize())
		{
			Log::instance()->printMessage("DynamicMeshGeoemtry buffer is not enough!\n");
			exit(1);
		}

		vertexBuffer_->mapBuffer();
		Vertex* ver_data = vertexBuffer_->getBufferPtr();
		ver_data += current_v_index_;

		//copy from cpu to gpu
		for (int i = 0; i < meshNum; i++)
		{
			memcpy(ver_data, meshs_[i]->cVertex(), meshs_[i]->vSize() * sizeof(Vertex));
			ver_data += obj->count_[i];
			obj->first_[i] = (i == 0) ? current_v_index_ : obj->first_[i - 1] + obj->count_[i - 1];
		}

		vertexBuffer_->setDataSize((vertexSize + current_v_index_) * sizeof(Vertex));

		vertexBuffer_->flush_data();

		if (have_indices_) indicesBuffer_->flush_data();

		obj->pos_vbo_ = vertexBuffer_->VBO();

		current_v_index_ += vertexSize;

	}

	else if (DRAW_ELEMENTS == call)
	{
		////vertex data
		//const std::vector<std::vector<Vertex> >& vs = mesh->vertices_;
		//int multVertexSize = vs.size();
		//assert(multVertexSize == 1);

		////indices
		//int multIds = mesh->indices_.size();
		//obj->count_ = new int[multIds];  //delete
		//obj->indices_ = (void**)new uint16 * [multIds];//uint16 type->GL_UNSIGNED_SHORT //delete
		//obj->drawcount_ = multIds;

		//for (int i = 0; i < multIds; i++)
		//{
		//	obj->count_[i] = mesh->indices_[i].size();
		//	obj->indices_[i] = new uint16[obj->count_[i]];
		//	const std::vector<uint16>& vi = mesh->indices_[i];
		//	std::copy(vi.cbegin(), vi.cend(), (uint16*)(obj->indices_[i]));
		//}

		//const std::vector<Vertex>& vertexs = vs[0];
		//glGenVertexArrays(1, &obj->vao_);
		//glBindVertexArray(obj->vao_);

		//glGenBuffers(1, &obj->pos_vbo_);
		//glBindBuffer(GL_ARRAY_BUFFER, obj->pos_vbo_);
		//glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(Vertex), &vertexs[0], GL_STATIC_DRAW);

		//setupVertexAttribute(mesh->CFVF());

		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);

	}

	CHECK_GL_ERROR;
}
