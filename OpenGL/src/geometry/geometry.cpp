
#include "geometry.h"
#include "resource.h"
#include "meshLoad.h"
#include "log.h"
#include "shader.h"
#include "helpF.h"
#include "gls.h"
#include "mesh.h"
CommonGeometry::CommonGeometry(bool update /*= false*/):beupdte_(update)
{
	box_.init();
	modelMatrix_ = Matrixf(1.0);
}

void CommonGeometry::updateGeometry(const CameraBase* camera)
{

}

CommonGeometry::~CommonGeometry()
{
}

const base::BoundingBox& CommonGeometry::boundingBox() const
{
	return box_;
}

void CommonGeometry::computeBoundingBox(void *)
{

}

math::Matrixf CommonGeometry::getModelMatrix()const
{
	return modelMatrix_;
}

void CommonGeometry::updateModelMatix(const Matrixf&m)
{
	modelMatrix_ = m;
}

MeshGeometry::MeshGeometry(IRenderMeshObj_SP obj, VERTEX_TYPE vt) 
	:CommonGeometry(false),mesh_obj_(obj), vertex_type_(vt)
{

}

MeshGeometry::~MeshGeometry()
{
}

void MeshGeometry::initGeometry()
{
	setupMesh();
	computeBoundingBox(NULL);
}

void MeshGeometry::drawGeoemtry(const DrawInfo&di)
{
	/**
	*  because the meshGeometry have many mesh object,and the getModelMatrix() is the matrix for meshGeometry
	but each of mesh object have a matrix
	*/
	Shader * shader = di.draw_shader_;
	GLint loc = shader->getVariable("model", false);
	Matrixf transform = di.matrix_ * getModelMatrix();
	drawMesh(loc, transform, shader);
}


void MeshGeometry::computeBoundingBox(void *)
{
	std::vector<Mesh_SP>::iterator iter = meshs_.begin();
	for (; iter != meshs_.end(); iter++)
	{
		(*iter)->computeBox();
		box_.expandBy((*iter)->getBOX());
	}
}

void MeshGeometry::addMesh(Mesh_SP mesh)
{
	meshs_.push_back(mesh);
}

void MeshGeometry::setupMesh()
{
	mesh_obj_->bind();

	uint16 call = mesh_obj_->call();

	if (DRAW_ARRAYS == call)
	{
		MultRenderMeshObj* multRenderObj = (MultRenderMeshObj*)mesh_obj_.addr();
		assert(multRenderObj);

		ArraysRenderMeshObj * obj = (ArraysRenderMeshObj*)multRenderObj;
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

		int vesize = Mesh::getVertexElementSize(vertex_type_);
		uint8 * vertexData =  new uint8 [vesize* vertexSize];  //delete
		uint8* temp = vertexData;
		for (int i = 0; i < meshNum; i++)
		{
			Mesh_SP mesh = meshs_[i];
			memcpy(temp, mesh->cVertex(), mesh->vSize() * vesize);
			temp += mesh->vSize() * vesize;
		}

		glGenBuffers(1, &obj->pos_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, obj->pos_vbo_);
		glBufferData(GL_ARRAY_BUFFER, vertexSize * vesize, (GLvoid*)vertexData, GL_STATIC_DRAW);
		 
		setupVertexAttribute();

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		SAFTE_DELETE_ARRAY(vertexData);
	}
	else if (DRAW_ELEMENTS == call)
	{
		MultRenderMeshObj* multRenderObj = (MultRenderMeshObj*)mesh_obj_.addr();
		assert(multRenderObj);
		ElementsRenderMeshObj * obj = dynamic_cast<ElementsRenderMeshObj*>(multRenderObj);
		assert(obj);

		int vertexNum = 0;
		int indicesNum = 0;
		int meshNum = meshs_.size();
		
		obj->basevertex_ = new int[meshNum]; //delete
		obj->count_ = new int[meshNum]; //delete
		obj->drawcount_ = meshNum;
		obj->indices_ = (void**)new uint16 *[meshNum];//uint16 type->GL_UNSIGNED_SHORT //delete

		Mesh_SP mesh;
		for (int i = 0; i < meshNum; i++)
		{
			mesh = meshs_[i];
			indicesNum += mesh->iSize();
			vertexNum += meshs_[i]->vSize();
			obj->count_[i] = mesh->iSize();
			obj->indices_[i] = BUFFER_OFFSET(indicesNum * sizeof(uint16));
		}

		int vesize = Mesh::getVertexElementSize(vertex_type_);
		uint8 * vertexData = new uint8[vesize* vertexNum]; 
		uint16 * indicesData = new uint16[indicesNum];
		
		uint8* tempVertexData = vertexData;
		UINT16 * tempIndiceData = indicesData;

		for (int i = 0; i < meshNum; i++)
		{
			Mesh_SP mesh = meshs_[i];

			if(mesh->vSize() > 0) memcpy(tempVertexData, mesh->cVertex(), mesh->vSize() * vesize);
			memcpy(tempIndiceData, mesh->cIndice(), mesh->iSize() * sizeof(uint16));

			tempVertexData += mesh->vSize() * vesize;
			tempIndiceData += mesh->iSize();

			obj->basevertex_[i] = (i == 0) ?  0 : (obj->basevertex_[i - 1] + mesh->vSize());
		}
 

		glGenBuffers(1, &obj->indices_vbo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->indices_vbo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16) * indicesNum, indicesData, GL_STATIC_DRAW);

		glGenBuffers(1, &obj->pos_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, obj->pos_vbo_);
		glBufferData(GL_ARRAY_BUFFER, vertexNum * vesize, vertexData, GL_STATIC_DRAW);

		setupVertexAttribute();

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		SAFTE_DELETE_ARRAY(vertexData);
		SAFTE_DELETE_ARRAY(indicesData);

	}
	else if (DRAW_ARRAY_INSTANC == call)
	{

	}
	else if (DRAW_ELEMENT_INSTANC == call)
	{

	}
	else if (DRAW_ARRAYS_INDIRECT == call)
	{

	}
	else if (DRAW_ELEMENTS_INDIRECT == call)
	{
		//vertex data

		/*const std::vector<Vertex>& vertexs = vs[0];
		glGenBuffers(1, &obj->pos_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, obj->pos_vbo_);
		glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(Vertex), &vertexs[0], GL_STATIC_DRAW);

		int idx = setupVertexAttribute(mesh->CFVF());

		const std::vector<uint16>& inds = mesh->indices_[0];
		glGenBuffers(1, &obj->indices_vbo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->indices_vbo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16) * inds.size(), &inds[0], GL_STATIC_DRAW);

		glGenBuffers(1, &obj->indirect_vbo_);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, obj->indirect_vbo_);
		obj->drawcount_ = mesh->numIndirect_;
		glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand) * obj->drawcount_, mesh->eIndirectCom_, GL_STATIC_DRAW);

		setupOtherBufferAndVA(idx);*/
	}
	else
	{
		PRINT_ERROR("setupMesh error for call type! \n");
	}
	
	mesh_obj_->unBind();



	/*Mesh * ms = mesh;
	TMesh * tms = dynamic_cast<TMesh*>(ms);
	if (tms)
	{
		RenderMeshObj * rmo = dynamic_cast<RenderMeshObj*>(renderObj.addr());
		rmo->t_indices_ = tms->t_indices_;
	}
*/

	/*if (mesh->indirectCom_.size())
	{
	glGenBuffers(1, &obj->indirect_vbo_);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, obj->indirect_vbo_);
	glBufferData(GL_DRAW_INDIRECT_BUFFER,( mesh->indirectCom_.size() * sizeof(unsigned int)), &(mesh->indirectCom_[0]), GL_STATIC_DRAW);
	}
	*/
	CHECK_GL_ERROR;

}


void MeshGeometry::drawMesh(int loc, Matrixf transform, Shader *shader)
{
	if (loc != -1)
		shader->setMatrix4(loc, 1, GL_FALSE, math::value_ptr(transform));
	mesh_obj_->preRender(shader);
	mesh_obj_->bind();
	mesh_obj_->draw();
	mesh_obj_->unBind();
	mesh_obj_->postRender(shader);
}

int MeshGeometry::setupVertexAttribute()
{
	int vesize = Mesh::getVertexElementSize(vertex_type_);

	int idx = 0;
	switch (vertex_type_)
	{
	case VERTEX_POINTS:
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)0);
		idx = 1;
		break;
	case VERTEX_POINTS_TEXTURE:
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)sizeof(Vertex_P));
		idx = 2;
		break;
	case VERTEX_POINTS_COLOR:
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)sizeof(Vertex_P));
		idx = 2;
		break;
	case VERTEX_POINTS_NORMAL:
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)sizeof(Vertex_P));
		idx = 2;
		break;
	case VERTEX_POINTS_TBN:
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)sizeof(Vertex_P));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)(sizeof(Vertex_P) + 12));
		idx = 3;
		break;
	case VERTEX_POINTS_NORMAL_TEXTURE:
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)sizeof(Vertex_P));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)(sizeof(Vertex_PN)));
		idx = 3;
		break;
	case VERTEX_POINTS_NORMAL_TEXTURE_TBN:
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)sizeof(Vertex_P));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)(sizeof(Vertex_PN)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)(sizeof(Vertex_PNT)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, vesize, (GLvoid*)(sizeof(Vertex_PNT) + 12));
		idx = 5;
		break;
	default:
		break;
	
	}
	 
	return idx;
}

