#include "brute_force.h"
#include <dynamicMesh.h>
#include "geomipmapping.h"

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
#include <Inputmanager.h>
#include <callback.h>
#include <text.h>
#include <gls.h>
//from Foucs 3d terrain book
class TerrainScene :public Scene
{
public:
protected:
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initKeyCallback();
	virtual bool					update();

public:

	virtual void					render(PassInfo&);

	Cbrute_force_terrain			bruteForce_terrain_;
	Cgeomipmapping					geoMapping_terrain_;

	MeshGeometry_Sp					meshGeometry_ = nullptr;
	bool							brute_ = false;
};


bool keyEvent(int code)
{
	return true;
}
bool TerrainScene::initSceneModels(const SceneInitInfo&)
{
	if (brute_)
	{
		bruteForce_terrain_.initHightMap();
		IRenderMeshObj_SP  renderMesh = new ArraysRenderMeshObj;
		meshGeometry_ = bruteForce_terrain_.initGeometry(renderMesh);
		if (renderNodeNum() == 0)
		{
			RenderNode_SP rn = new RenderNode;
			if (rn) rn->setGeometry(meshGeometry_);
			addRenderNode(rn);
		}
		else
		{
			((RenderNode*)getRenderNode(0).addr())->setGeometry(meshGeometry_);
		}

		bruteForce_terrain_.initTexture();
	}
	else
	{
		IRenderMeshObj_SP  renderMesh = new ArraysRenderMeshObj;
		geoMapping_terrain_.initHightMap();
		geoMapping_terrain_.initTexture();
		meshGeometry_ = geoMapping_terrain_.initGeometry(renderMesh);
		RenderNode_SP rn = new RenderNode;
		if (rn) rn->setGeometry(meshGeometry_);
		addRenderNode(rn);
		bool result = geoMapping_terrain_.init(17);
		return result;
	}
	return true;
}

bool TerrainScene::initShader(const SceneInitInfo&)
{
	Shader * shader = new Shader;
	//因为obj模型原因此处使用normal作为color
	char vertShder[] = "#version 330 core \n"
		"layout(location = 0) in vec3 position;"
		"layout(location = 1) in vec3 vcolor;"
		"layout(location = 2) in vec2 tex;"

		"out vec3 ocolor;"
		"out vec2 texCoord;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"

		"void main()"
		"{"
		"	gl_Position = projection * view * model * vec4(position, 1.0f);"
		"	ocolor = vcolor;"
		"	texCoord = tex;"
		"}";

	char fragShader[] = "#version 330 core \n"
		"in vec3 ocolor;"
		"in vec2 texCoord;"
		"out vec4 color;"
		"uniform sampler2D hightmap;"
		"void main()"
		"{"
		"color = texture(hightmap, texCoord);"
		"}";

	char vertShder_g[] = "#version 330 core \n"
		"layout(location = 0) in vec3 position;"
		"layout(location = 1) in vec2 tex;"

		"out vec2 texCoord;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"

		"void main()"
		"{"
		"	gl_Position = projection * view * model * vec4(position, 1.0f);"
		"	texCoord = tex;"
		"}";

	char fragShader_g[] = "#version 330 core \n"
		"in vec2 texCoord;"
		"out vec4 color;"
		"uniform sampler2D hightmap;"
		"void main()"
		"{"
		"color = texture(hightmap, texCoord);"
		"}";

	if(brute_)shader->loadShaderSouce(vertShder, fragShader, NULL);
	else shader->loadShaderSouce(vertShder_g, fragShader_g, NULL);
	shaders_.push_back(shader);
	shader->turnOn();

	GLint loc = shader->getVariable("hightmap");
	if (loc != -1)
		shader->setInt(loc, 0);
	shader->turnOff();

	return true;
}

bool TerrainScene::initKeyCallback()
{
	Scene::initKeyCallback();

	KEY_CALLBACK(258, base::NewPermanentCallback(keyEvent)); //GLFW_KEY_TAB
	KEY_CALLBACK(79, base::NewPermanentCallback(keyEvent));
	KEY_CALLBACK(80, base::NewPermanentCallback(keyEvent));

	KEY_CALLBACK(333, base::NewPermanentCallback(keyEvent)); //add
	KEY_CALLBACK(334, base::NewPermanentCallback(keyEvent)); //sub

	return true;
}

bool TerrainScene::update()
{
	if (!brute_)
	{
		geoMapping_terrain_.updateGeometry(getCamera(), meshGeometry_);
	}

	Scene::update();

	return true;
}

void TerrainScene::render(PassInfo&ps)
{
	Shader * shader = shaders_[0];
	shader->turnOn();
	initUniformVal(shader);
	if (!brute_) geoMapping_terrain_.render();
	CHECK_GL_ERROR;
	getRenderNode(0)->render(shader, ps);
	shader->turnOff();

	CHECK_GL_ERROR;
}

int main()
{
	TerrainScene * scene = new TerrainScene;
	Camera *pCamera = new Camera();
	scene->setMasterCamera(pCamera);
	WindowManager *pWindowManager = new WindowManager();
	pWindowManager->initialize();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	const char * title = "3d terrain Test";
	application.initialize(1024, 960, title);
	application.initScene();
	pCamera->setClipPlane(0.1f, 5000.0f);
	application.start();

	return 0;
}
