#include "axes.h"
#include "mesh.h"
#include "engineLoad.h"
#include "resource.h"
#include "shader.h"
#include "renderNode.h"
#include "log.h"
#include "gls.h"

AexsGeometry::AexsGeometry(IRenderMeshObj_SP rsp) :
	MeshGeometry(rsp, VERTEX_POINTS_NORMAL)
{

}

void AexsGeometry::initGeometry()
{
	for (int i = 0; i < meshs_.size(); i++)
	{
		Mesh_SP mesh = meshs_[i];

		for (int j = 0; j < mesh->iSize(); j++)
		{
			Vertex_PN * vs = (Vertex_PN*)mesh->getVertex(j);
			if (i == 0) //
			{
				vs->normal_.x = 0.45;
				vs->normal_.y = 0.0;
				vs->normal_.z = 0.0;
			}
			else if (i == 1)
			{
				vs->normal_.x = 0.0;
				vs->normal_.y = 0.45;
				vs->normal_.z = 0.0;
			}
			else if (i == 2)
			{
				vs->normal_.x = 0.0;
				vs->normal_.y = 0.0;
				vs->normal_.z = 0.45;
			}

			else if (i == 3)
			{
				vs->normal_.x = 0.9;
				vs->normal_.y = 0.0;
				vs->normal_.z = 0.0;
				vs->position_.x += 1.0;
			}
			else if (i == 4)
			{
				vs->normal_.x = 0.0;
				vs->normal_.y = 0.9;
				vs->normal_.z = 0.0;
				vs->position_.y += 1.0;
			}
			else if (i == 5)
			{
				vs->normal_.x = 0.0;
				vs->normal_.y = 0.0;
				vs->normal_.z = 0.9;
				vs->position_.z += 1.0;
			}
			else
			{
			}
		}
		//此处用normal作为color
		mesh->box().init();
		mesh->computeBox();
	}

	setupMesh();
}


IRenderNode_SP  createAxesRenderNode()
{
	IO::LModelInfo data;
	std::string modelPath = get_model_BasePath() + "axes.obj";

	IRenderNode_SP axesNode;
	SmartPointer<ElementsRenderMeshObj> rsp = new ElementsRenderMeshObj;
	rsp->call(DRAW_ELEMENTS);
	rsp->type_ = GL_UNSIGNED_SHORT;

	if (IO::EngineLoad::loadNode(modelPath.c_str(), data))
	{
		if (!data.mapMeshs.empty())
		{
			axesNode = new RenderNode;
			base::SmartPointer<AexsGeometry> mg = new AexsGeometry(rsp);
			rsp->model(data.mapMeshs.begin()->first); 
			for each (Mesh_SP mesh in data.mapMeshs.begin()->second)
			{
				mg->addMesh(mesh);
			}
			mg->initGeometry();
			if (axesNode)
				((RenderNode*)axesNode.addr())->setGeometry(mg);
		}
	}
	return axesNode;
}

Shader_SP createAxesRenderNodeShader()
{
	//因为obj模型原因此处使用normal作为color
	char vertShder[] = "#version 330 core \n"
		"layout(location = 0) in vec3 position;"
		"layout(location = 1) in vec3 color;"

		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"out vec3 vcolor;"

		"void main()"
		"{"
		"	gl_Position = projection * view * model * vec4(position, 1.0f);"
		"   vcolor = color;"
		"}";

	char fragShader[] = "#version 330 core \n"
		"out vec4 color;"
		"in vec3 vcolor;"
		"void main()"
		"{"
		"color = vec4(vcolor, 1.0f);"
		"}";

	Shader_SP axesShader = new Shader;
	axesShader->loadShaderSouce(vertShder, fragShader, NULL);
	return axesShader;
}

