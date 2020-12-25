//
//  baseGeometry.h
//  openGLTest
//
//  Created by xt on 18/6/10.
//  Copyright © 2018年 xt. All rights reserved.
//

#ifndef baseGeometry_h
#define baseGeometry_h

#include "BaseObject.h"
#include "SmartPointer.h"
#include "boundingBox.h"
#include "mesh.h"
#include "common.h"
#include "renderMesh.h"
#include "gls.h"
#include <string>
#include <vector>


class Shader;
class CameraBase;

struct DrawInfo
{
	Shader* draw_shader_ = nullptr;
	bool needBind_ = true;
	Matrixf	matrix_ = Matrixf(1.0f);
};

class LIBENIGHT_EXPORT CommonGeometry :public base::BaseObject
{
public:
	CommonGeometry(bool update = false);

	virtual ~CommonGeometry();

	void         bindVAO();
	void         unBindVAO();
	virtual void updateGeometry(const CameraBase* camera);
	virtual void initGeometry() = 0;
	virtual void drawGeoemtry(const DrawInfo&) = 0;
	virtual void computeBoundingBox(void *);
	virtual Matrixf				getModelMatrix()const;
	virtual void				updateModelMatix(const Matrixf&);

	const base::BoundingBox& boundingBox()const;
protected:
	unsigned int  vao_;
	base::BoundingBox  box_;
	Matrixf modelMatrix_;
	bool   beupdte_;
};

class LIBENIGHT_EXPORT Cub :public CommonGeometry
{
public:
	Cub(bool update = false);
	virtual ~Cub();
	virtual void updateGeometry();
	virtual void initGeometry();
	virtual void drawGeoemtry(const DrawInfo&);
	virtual void computeBoundingBox(void *);

private:
	unsigned int vbo_;
};

class LIBENIGHT_EXPORT Quad :public CommonGeometry
{
public:
	Quad(bool update = false);
	~Quad();

	virtual void updateGeometry();
	virtual void initGeometry();
	virtual void drawGeoemtry(const DrawInfo&);
	virtual void computeBoundingBox(void *);

	unsigned int vbo_;

};

class LIBENIGHT_EXPORT ObjGeometry :public CommonGeometry
{
public:
	ObjGeometry(const char*file);
	~ObjGeometry();
	virtual void updateGeometry();
	virtual void initGeometry();
	virtual void drawGeoemtry(const DrawInfo&);
	virtual void computeBoundingBox(void *);

private:
	unsigned int vbo_[3];//vertex texture normal
	std::string file_;
	int vertices_size_ = 0;
};

class LIBENIGHT_EXPORT MeshGeometry :public CommonGeometry
{
public:
	MeshGeometry(bool update = false);
	~MeshGeometry();

	virtual void preRender();
	virtual void postRender();
	virtual void updateGeometry(const CameraBase* camera);
	virtual void initGeometry();
	virtual void drawGeoemtry(const DrawInfo&);
	virtual void computeBoundingBox(void *);

	void		 addMesh(Mesh_SP);
	
	void         clear();

	std::vector<Mesh_SP > meshs_;
	std::vector<IRenderMeshObj_SP> mesh_obj_;

protected:
	virtual IRenderMeshObj_SP createRenderMeshObj();
	virtual void setupMesh(const Mesh_SP mesh, IRenderMeshObj_SP& obj);
	virtual void drawMesh(int , Matrixf,Shader *, const Mesh_SP mesh, IRenderMeshObj_SP& obj);
	virtual int  setupVertexAttribute(const int);
	virtual void setupOtherBufferAndVA(int idx);
	
private:
	int vertices_size_ = 0;
};


template <class T>
class  MeshGeometryX :public CommonGeometry
{
public:
	MeshGeometryX(bool update = false);
	~MeshGeometryX();

	virtual void preRender();
	virtual void postRender();
	virtual void updateGeometry(const CameraBase* camera);
	virtual void initGeometry();
	virtual void drawGeoemtry(const DrawInfo&);
	virtual void computeBoundingBox(void *);

	void		 addMesh(MeshX<T> *);

	std::vector<MeshX<T> *> meshs_;
	std::vector<IRenderMeshObj_SP> mesh_obj_;

protected:
	virtual IRenderMeshObj_SP createRenderMeshObj();
	virtual void setupMesh(const MeshX<T>* mesh, IRenderMeshObj_SP& obj);
	virtual void drawMesh(int, Matrixf, Shader *, const MeshX<T>* mesh, IRenderMeshObj_SP& obj);
	virtual int  setupVertexAttribute(const int);
	virtual void setupOtherBufferAndVA(int idx);

private:
	int vertices_size_ = 0;
};

template <class T>
void MeshGeometryX<T>::setupOtherBufferAndVA(int idx)
{

}

template <class T>
int MeshGeometryX<T>::setupVertexAttribute(const int fvf)
{
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(T), (void*)0);
	GLuint indx = 1;
	if (fvf & FVF_NORMAL)
	{
		// vertex normals
		glEnableVertexAttribArray(indx);
		glVertexAttribPointer(indx, 3, GL_FLOAT, GL_FALSE, sizeof(T), (void*)offsetof(Vertex, Normal));
		indx += 1;
	}
	if (fvf & FVF_TEXT0)
	{
		// vertex texture coords
		glEnableVertexAttribArray(indx);
		glVertexAttribPointer(indx, 2, GL_FLOAT, GL_FALSE, sizeof(T), (void*)offsetof(Vertex, TexCoords));
		indx += 1;
	}
	if (fvf & FVF_TANGENT)
	{
		// vertex tangent
		glEnableVertexAttribArray(indx);
		glVertexAttribPointer(indx, 3, GL_FLOAT, GL_FALSE, sizeof(T), (void*)offsetof(Vertex, Tangent));
		indx += 1;
	}
	if (fvf & FVF_BITANGENT)
	{
		// vertex bitangent
		glEnableVertexAttribArray(indx);
		glVertexAttribPointer(indx, 3, GL_FLOAT, GL_FALSE, sizeof(T), (void*)offsetof(Vertex, Bitangent));
		indx += 1;
	}

	return indx;
}

template <class T>
void MeshGeometryX<T>::drawMesh(int loc, Matrixf transform, Shader *shader, const MeshX<T>* mesh, IRenderMeshObj_SP& obj)
{
	if (loc != -1)
	{
		transform *= mesh->Cmatrix();
		shader->setMatrix4(loc, 1, GL_FALSE, &transform[0][0]);
	}
	obj->preRender(shader);
	glBindVertexArray(obj->vao_);
	obj->draw();
	glBindVertexArray(0);
	obj->postRender(shader);
}

template <class T>
void MeshGeometryX<T>::setupMesh(const MeshX<T>* mesh, IRenderMeshObj_SP& obj)
{
	obj->call_ = mesh->call_;
	obj->type_ = mesh->type_;
	obj->mode_ = mesh->mode_;
	obj->aIndirectCom_ = mesh->aIndirectCom_;

	if (obj->type_ == 0) obj->type_ = GL_UNSIGNED_SHORT;

	glGenVertexArrays(1, &obj->vao_);
	glBindVertexArray(obj->vao_);

	T * vertexData = NULL;
	if (DRAW_ARRAYS == mesh->call_)
	{
		int vertexSize = 0;
		const std::vector<std::vector<T> >& vs = mesh->vertices_;
		int multVertexSize = vs.size();

		obj->first_ = new int[multVertexSize]; //delete
		obj->count_ = new int[multVertexSize]; //delete
		obj->drawcount_ = multVertexSize;

		for (int i = 0; i < multVertexSize; i++)
		{
			obj->first_[i] = vertexSize;
			vertexSize += obj->count_[i] = vs[i].size();
		}

		vertexData = new T[vertexSize];  //delete
		T* temp = vertexData;
		for (int i = 0; i < multVertexSize; i++)
		{
			std::copy(vs[i].cbegin(), vs[i].cend(), temp);
			temp += vs[i].size();
		}

		glGenBuffers(1, &obj->pos_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, obj->pos_vbo_);
		glBufferData(GL_ARRAY_BUFFER, vertexSize * sizeof(T), vertexData, GL_STATIC_DRAW);

		setupVertexAttribute(mesh->CFVF());

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
	else if (DRAW_ARRAYS_INDIRECT == mesh->call_)
	{

	}
	else if (DRAW_ARRAYS_INSTANC == mesh->call_)
	{

	}
	else if (DRAW_ELEMENTS == mesh->call_)
	{
		const std::vector<std::vector<T> >& vs = mesh->vertices_;
		int multVertexSize = vs.size();
		assert(multVertexSize == 1);

		//indices
		int multIds = mesh->indices_.size();
		obj->count_ = new int[multIds];  //delete
		obj->indices_ = (void**)new uint16 *[multIds];//uint16 type->GL_UNSIGNED_SHORT //delete
		obj->drawcount_ = multIds;

		for (int i = 0; i < multIds; i++)
		{
			obj->count_[i] = mesh->indices_[i].size();
			obj->indices_[i] = new uint16[obj->count_[i]];
			const std::vector<uint16>& vi = mesh->indices_[i];
			std::copy(vi.cbegin(), vi.cend(), (uint16*)(obj->indices_[i]));
		}

		const std::vector<T>& vertexs = vs[0];

		glGenBuffers(1, &obj->pos_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, obj->pos_vbo_);
		glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(T), &vertexs[0], GL_STATIC_DRAW);

		setupVertexAttribute(mesh->CFVF());

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else if (DRAW_ELEMENTS_BASE_VERTEX == mesh->call_) {

	}
	else if (DRAW_ELEMENT_INSTANCE_BASE_VERTEX == mesh->call_) {

	}
	else if (DRAW_ELEMENT_INSTANCE_BASE_INSTANCE == mesh->call_) {

	}
	else if (DRAW_ELEMENT_INDIRECT == mesh->call_) {

		//vertex data
		const std::vector<std::vector<T> >& vs = mesh->vertices_;
		int multVertexSize = vs.size();
		assert(multVertexSize == 1);

		int multIds = mesh->indices_.size();
		assert(multIds == 1);

		const std::vector<T>& vertexs = vs[0];
		glGenBuffers(1, &obj->pos_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, obj->pos_vbo_);
		glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(T), &vertexs[0], GL_STATIC_DRAW);

		int idx = setupVertexAttribute(mesh->CFVF());

		const std::vector<uint16>& inds = mesh->indices_[0];
		glGenBuffers(1, &obj->indices_vbo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->indices_vbo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16) * inds.size(), &inds[0], GL_STATIC_DRAW);

		glGenBuffers(1, &obj->indirect_vbo_);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, obj->indirect_vbo_);
		obj->drawcount_ = mesh->numIndirect_;
		glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand) * obj->drawcount_, mesh->eIndirectCom_, GL_STATIC_DRAW);

		setupOtherBufferAndVA(idx);
	}

	glBindVertexArray(0);
}

template <class T>
IRenderMeshObj_SP MeshGeometryX<T>::createRenderMeshObj()
{
	return new RenderMeshObj;
}

template <class T>
void MeshGeometryX<T>::addMesh(MeshX<T> *mesh)
{
	meshs_.push_back(mesh);
}

template <class T>
void MeshGeometryX<T>::computeBoundingBox(void *)
{
	std::vector<MeshX<T> * >::const_iterator iter = meshs_.cbegin();
	for (; iter != meshs_.cend(); iter++)
	{
		box_.expandBy((*iter)->getBOX());
	}
}

template <class T>
void MeshGeometryX<T>::drawGeoemtry(const DrawInfo&di)
{
	preRender();

	Shader * shader = di.draw_shader_;
	GLint loc = shader->getVariable("model");
	Matrixf transform = di.matrix_ * getModelMatrix();

	for (int i = 0; i < mesh_obj_.size(); i++)
		drawMesh(loc, transform, shader, meshs_[i], mesh_obj_[i]);

	postRender();
}

template <class T>
void MeshGeometryX<T>::initGeometry()
{
	std::vector<MeshX<T> *>::const_iterator iter = meshs_.cbegin();

	for (; iter != meshs_.cend(); iter++)
	{
		IRenderMeshObj_SP irmsp = createRenderMeshObj();
		setupMesh(*iter, irmsp);
		mesh_obj_.push_back(irmsp);
	}
	computeBoundingBox(NULL);
}

template <class T>
void MeshGeometryX<T>::updateGeometry(const CameraBase* camera)
{
	if (!beupdte_) return;
}

template <class T>
void MeshGeometryX<T>::postRender()
{

}

template <class T>
void MeshGeometryX<T>::preRender()
{

}

template <class T>
MeshGeometryX<T>::~MeshGeometryX()
{
	for (int i = 0; i < meshs_.size(); i++)
	{
		delete meshs_[i];
	}
}

template <class T>
MeshGeometryX<T>::MeshGeometryX(bool update /*= false*/)
{

}



typedef base::SmartPointer<CommonGeometry> CommonGeometry_Sp;
typedef base::SmartPointer<MeshGeometry>	MeshGeometry_Sp;

#endif /* baseGeometry_h */
