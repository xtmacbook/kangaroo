#include "scene.h"
#include "RenderNode.h"
#include "shader.h"
#include "engineLoad.h"
#include "resource.h"
#include "gLApplication.h"
#include "camera.h"
#include "windowManager.h"
#include "log.h"
#include "helpF.h"
#include <baseMesh.h>
#include <Inputmanager.h>
#include <callback.h>
#include <text.h>
#include "gls.h"
#include "dynamicMesh.h"
#include "geometry.h"
#include <glinter.h>
#include <glQuery.h>

/**
 * test opengl Draw Api:
	glDrawElements								 (GLenum mode, GLsizei count, GLenum type, const void*indices)
	glDrawElementsInstance						 (GLenum mode, GLsizei count, GLenum type, const void*indices, GLsizei instancecount)
	glDrawElementsBaseVertex					 (GLenum mode, GLsizei count, GLenum type, const void*indices,							GLint basevert)
	glDrawElementsIndirect						 (GLenum mode,				  GLenum type, void* indirect)
	glDrawElementsInstancedBaseInstance			 (GLenum mode, GLsizei count, GLenum type, const void*indices,  GLsizei instancecount,					GLuint baseinstance)
	glDrawElementsInstancedBaseVertex			 (GLenum mode, GLsizei count, GLenum type, void *indices,       GLsizei instancecount, GLint basevertex)
	glDrawElementsInstancedBaseVertexBaseInstance(GLenum mode, GLsizei count, GLenum type​, void *indices​,       GLsizei instancecount​, GLint basevertex​, GLuint baseinstance)

	glMultiDrawElements	

	glMultiDrawElements:
	void glMultiDrawElements(mode,const GLsizei *count,GLenum type,const Glvoid*const*indices,Glsizei pricount)
	{
		GLsizei i;
		for(i = 0;i < primcount;i++)
		{
			glDrawElements(mode,count[i],type,indices[i]);
		}
	}

	glMultDrawElementsBaseVertex:
	void glMultDrawElementsBaseVertex(GLenum mode,const GLsizei*count,GLenum type,const GLvoid*const*indices,GLsizei primcount,const GLint * baseVertex)
	{
		GLsize i;
		for(i = 0;i <primcout;i++)
		{
			glDrawElementsBaseVertex(mode,count[i],type,indices[i],baseVertex[i]);
		}
	}

 */

#define INSTANCE_COUNT 200

class DrawScene;

DrawScene* g_scene;

static GLfloat quadVertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f, 0.0, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0, 0.0f, 0.0f,
	1.0f, -1.0f, 0.0, 1.0f, 0.0f,

	-1.0f,  1.0f, 0.0, 0.0f, 1.0f,
	1.0f, -1.0f, 0.0, 1.0f, 0.0f,
	1.0f,  1.0f, 0.0, 1.0f, 1.0f
};


static const GLfloat vertex_positions[] =
{
	-1.0f, -1.0f,  0.0f,
	1.0f, -1.0f,  0.0f, 
	-1.0f,  1.0f,  0.0f,
	-1.0f, -1.0f,  0.0f,
};

// Color for each vertex
static const GLfloat vertex_colors[] =
{
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f
};

static const GLushort vertex_indices[] =
{
	0, 1, 2
};

static const GLfloat cube_positions[] =
{
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	1.0f, -1.0f, -1.0f, 
	1.0f, -1.0f,  1.0f, 
	1.0f,  1.0f, -1.0f, 
	1.0f,  1.0f,  1.0f
};

// Color for each vertex
static const GLfloat cube_colors[] =
{
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.5f
};

static const GLushort cube_indices[] =
{
	0, 1, 2, 3, 6, 7, 4, 5,         // First strip
	0xFFFF,                         // <<-- This is the restart index
	2, 6, 0, 4, 1, 5, 3, 7          // Second strip
};


const std::vector<GLfloat> vertices
{
	-0.63f,  0.00f, 0.0f, 0.0f, 0.5f, 0.5f,
	-0.63f,  0.50f, 0.0f, 0.0f, 0.5f, 0.5f,
	0.63f,  0.50f, 0.0f,  0.0f, 0.5f, 0.5f,
	0.63f,  0.00f, 0.0f,  0.0f, 0.5f, 0.5f,
						  
	-0.63f, -0.50f, 0.0f, 0.0f, 0.5f, 0.5f,
	-0.63f,  0.00f, 0.0f, 0.0f, 0.5f, 0.5f,
	0.63f,  0.00f, 0.0f,  0.0f, 0.5f, 0.5f,
	0.63f, -0.50f, 0.0f	, 0.0f, 0.5f, 0.5f
};

const std::vector<GLuint> indices
{
	0, 1, 2,
	2, 3, 0
};


class DrawScene :public Scene
{
protected:
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual void					render(PassInfo&);
	virtual bool					initShader(const SceneInitInfo&);
public:
	Shader * shader ;

};

bool DrawScene::initSceneModels(const SceneInitInfo&)
{
	RenderNode_SP quadNode(new RenderNode);
	
	SmartPointer<ArraysRenderMeshObj> armeshObj = new ArraysRenderMeshObj;
	armeshObj->model(GL_TRIANGLES);
	armeshObj->call(DRAW_ARRAYS);

	MeshGeometry_Sp mg = new MeshGeometry(armeshObj,VERTEX_POINTS_TEXTURE);
	Mesh_SP mesh   = new Mesh(VERTEX_POINTS_TEXTURE);
	mesh->createMesh(6);

	for (int i = 0; i < 6; i++)
	{
		Vertex_PT vertex( V3f(quadVertices[i * 5 + 0],quadVertices[i * 5 + 1] ,quadVertices[i * 5 + 2]),
			V2f(quadVertices[i * 5 + 3] ,quadVertices[i * 5 + 4]));
		mesh->addVertex(&vertex);
	}
	mg->addMesh(mesh);

	mg->initGeometry();
	quadNode->setGeometry(mg);
	addRenderNode(quadNode);

	IRenderNode_SP	sphere_ = getSphereRenderNode(V3f(0.0, 0.0, 0.0), 1.0, false);
	addRenderNode(sphere_);

	return true;
}

void DrawScene::render(PassInfo&info)
{
	shader->turnOn();

	initUniformVal(shader);

	getRenderNode(0)->render(shader, info);
	//getRenderNode(1)->render(shader, info);

	shader->turnOff();

	CHECK_GL_ERROR;
}

bool DrawScene::initShader(const SceneInitInfo&)
{
	 shader = new Shader;

	//因为obj模型原因此处使用normal作为color
	char vertShder[] = "#version 330 core \n"
		"layout(location = 0) in vec3 position;"
		"layout(location = 1) in vec2 texCoord;"

		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"

		"void main()"
		"{"
		"	gl_Position = projection * view * model * vec4(position, 1.0f);"
		"}";

	char fragShader[] = "#version 330 core \n"
		"out vec4 color;"
		"void main()"
		"{"
			"color = vec4(0.5,1.0,0.5, 1.0f);"
		"}";

	shader->loadShaderSouce(vertShder, fragShader, NULL);
	
	return true;
}

 

int main()
{
	DrawScene * scene = new DrawScene;
	g_scene = scene;

	Camera *pCamera = new Camera();
	scene->setMasterCamera(pCamera);
	WindowManager *pWindowManager = new WindowManager();

	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "Draw Api Test";
	wc.width_ = 1024;
	wc.height_ = 960;
	wc.pos_x_ = 50;
	wc.pos_y_ = 50;

	application.initialize(&wc, &dc);
	application.initScene();
	pCamera->setClipPlane(0.1f, 500.0f);
	application.start();

	return 0;
}
