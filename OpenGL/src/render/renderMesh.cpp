#include "mesh.h"
#include "renderMesh.h"
#include "gls.h"
#include "log.h"

//gldrawArray:
/*
	1.glDrawArraysInstanced(	GLenum mode,GLint first,GLsizei count,GLsizei instancecount);

		for (int i = 0; i < instancecount ; i++) 
		{
			instanceID = i;
			glDrawArrays(mode, first, count);
		}
		instanceID = 0;

	2.glMultiDrawArrays(	GLenum mode,const GLint * first,const GLsizei * count,GLsizei drawcount);
	3.glDrawArraysInstancedBaseInstance(GLenum mode,GLint first,GLsizei count,GLsizei instancecount,GLuint baseinstance);
		for (int i = 0; i < instancecount ; i++) 
		{
			instanceID = i;
			glDrawArrays(mode, first, count);
		}
		instanceID = 0; effect  instanced vertex attribute arrays
	4.glMultiDrawArraysIndirect 
*/



IRenderMeshObj::IRenderMeshObj()
{
	glGenVertexArrays(1, &vao_);
}

IRenderMeshObj::~IRenderMeshObj()
{
	if (vao_)
		glDeleteVertexArrays(1, &vao_);
	if (pos_vbo_)
		glDeleteBuffers(1, &pos_vbo_);
	if (indices_vbo_)
		glDeleteBuffers(1, &indices_vbo_);
}


void IRenderMeshObj::bind()
{
	glBindVertexArray(vao_);
}

void IRenderMeshObj::unBind()
{
	glBindVertexArray(0);
}

void IRenderMeshObj::preRender(Shader*)
{

}

void IRenderMeshObj::postRender(Shader*)
{

}

void IRenderMeshObj::call(short c)
{
	call_ = c;
}


void IRenderMeshObj::model(unsigned int m)
{
	mode_ = m;
}


ArraysRenderMeshObj::~ArraysRenderMeshObj()
{
	MultRenderMeshObj::~MultRenderMeshObj();
	SAFTE_DELETE_ARRAY(first_);
}

void ArraysRenderMeshObj::draw()
{
	if (DRAW_ARRAYS == call_)
		glMultiDrawArrays(mode_, first_, count_, drawcount_);
	else
		PRINT_ERROR("no draw api enable");
}


ElementsRenderMeshObj::~ElementsRenderMeshObj()
{
	MultRenderMeshObj::~MultRenderMeshObj();
	SAFTE_DELETE_ARRAY(basevertex_);
}

void ElementsRenderMeshObj::draw()
{
	if (DRAW_ELEMENTS == call_)
	{
		if(nullptr == basevertex_)
			glMultiDrawElements(mode_, (GLsizei*)count_, type_, (const GLvoid**)indices_, drawcount_);
		else
			glMultiDrawElementsBaseVertex(mode_, (GLsizei*)(count_), type_, const_cast<void **>(indices_), drawcount_, basevertex_);
	}
	else
		PRINT_ERROR("no draw api enable");
}

void ArrayInstanceRendermeshObj::draw()
{
	if (DRAW_ARRAY_INSTANC == call_)
		glDrawArraysInstancedBaseInstance(mode_, first_, count_, instanceCount_, baseInstance_);
	else
		PRINT_ERROR("no draw api enable");
}

void ElementInstanceRendermeshObj::draw()
{
	if (DRAW_ELEMENT_INSTANC == call_)
		glDrawElementsInstancedBaseVertexBaseInstance(mode_, count_, type_, indices_, instanceCount_,basevertex_, baseInstance_);
	else
		PRINT_ERROR("no draw api enable");
}

void ArrayIndirectRenderMeshObj::draw()
{
	if (DRAW_ARRAYS_INDIRECT == call_)
		glMultiDrawArraysIndirect(mode_, 0, drawcount_, stride_);
	else
		PRINT_ERROR("no draw api enable");
}

void ElementIndirectRenderMeshObj::draw()
{
	if (DRAW_ELEMENTS_INDIRECT == call_)
		glMultiDrawElementsIndirect(mode_, type_, (GLvoid*)0, drawcount_, stride_);
	else
		PRINT_ERROR("no draw api enable");
}

MultRenderMeshObj::~MultRenderMeshObj()
{
	IRenderMeshObj::~IRenderMeshObj();
	SAFTE_DELETE_ARRAY(count_);
}
