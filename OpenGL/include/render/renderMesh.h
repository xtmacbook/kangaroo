#ifndef Opengl_rendermesh_h
#define Opengl_rendermesh_h

#include "smartPointer.h"
#include "BaseObject.h"

#include <vector>
class Shader;

using namespace base;
 
struct LIBENIGHT_EXPORT IRenderMeshObj :public base::BaseObject
{

	IRenderMeshObj();

	virtual ~IRenderMeshObj();
		
	void bind();
	void unBind();

	unsigned int pos_vbo_ = 0;
	unsigned int indices_vbo_ = 0;
	unsigned int indirect_vbo_ = 0;

	virtual void preRender(Shader*);
	virtual void postRender(Shader*);

	virtual void draw() = 0;

	void			call(short);
	inline short	call()const {return call_;}
	void			model(unsigned int);

protected:
	short						call_ = DRAW_ARRAYS;
	unsigned int				mode_ = 0;

private:
	unsigned int vao_ = 0;
};

//for only one not for mult
//glDrawArraysInstancedBaseInstance
//glDrawElementsInstancedBaseVertex
//glDrawElementsInstancedBaseInstance
//glDrawElementsInstancedBaseVertexBaseInstance
struct LIBENIGHT_EXPORT InstanceRenderMeshObj :public IRenderMeshObj
{
	int 						basevertex_ = 0; //base vertex
	unsigned int				baseInstance_ = 0;
	unsigned int				instanceCount_ = 1;
};

struct LIBENIGHT_EXPORT IndirectRenderMeshObj :public IRenderMeshObj
{
	int							stride_ = 0; //for indirect
	unsigned int				drawcount_ = 1;
};

struct LIBENIGHT_EXPORT MultRenderMeshObj :public IRenderMeshObj
{
	virtual ~MultRenderMeshObj();

	unsigned int				drawcount_ = 1; //mult draw 
	int*						count_ = nullptr;
};

/*
gldrawarrays
glmultidrawarrays
*/
struct LIBENIGHT_EXPORT ArraysRenderMeshObj :public MultRenderMeshObj
{
	typedef MultRenderMeshObj Base;

	virtual ~ArraysRenderMeshObj();
	int *						first_ = nullptr; //
	virtual void draw();
};

/*
gldrawelement
glmultidrawelement

glMultiDrawElementsBaseVertex
glDrawElementsBaseVertex
*/
struct LIBENIGHT_EXPORT ElementsRenderMeshObj :public MultRenderMeshObj
{

	typedef MultRenderMeshObj Base;

	virtual ~ElementsRenderMeshObj();

	void **						indices_ = nullptr;
	unsigned int				type_;
	int *						basevertex_ = nullptr;
	virtual void draw();
};


struct LIBENIGHT_EXPORT ArrayInstanceRendermeshObj :public InstanceRenderMeshObj
{
	int 						first_ = 0;
	int							count_ = 0;
	virtual void				draw();
};


struct LIBENIGHT_EXPORT ElementInstanceRendermeshObj :public InstanceRenderMeshObj
{
	void*						indices_;
	int							count_ = 0;
	unsigned int				type_;
	virtual void				draw();
};

//glMultiDrawArraysIndirect 
struct LIBENIGHT_EXPORT ArrayIndirectRenderMeshObj :public IndirectRenderMeshObj
{
	virtual void				draw();
};
//glMultiDrawElementsIndirect 
struct LIBENIGHT_EXPORT ElementIndirectRenderMeshObj :public IndirectRenderMeshObj
{
	virtual void				draw();
	unsigned int				type_;
};

struct LIBENIGHT_EXPORT RenderMeshObj :public IRenderMeshObj
{
	std::vector<unsigned int> t_indices_; //texture indices in renderNode textures
};

typedef base::SmartPointer<IRenderMeshObj> IRenderMeshObj_SP;

#endif
