

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
#include "gls.h"
#include "dynamicMesh.h"
#include "glinter.h"

typedef struct {
	int elements_count;
	float *array;
} fan_s;

GLuint g_vao;
GLuint g_vbo;
#define VBO_SIZE 4096
#define MAX_FANS 1024
GLint fans_starts[MAX_FANS];
GLsizei fans_sizes[MAX_FANS];

GLApplication* g_app;

float fan0_data[] = {
	0.0, 0.0, 0.0,1.0,0.0,0.0,
	0.0, 10.0, 0.0,1.0,0.0,0.0,
	10.0, 10.0, 0.0,1.0,0.0,0.0,
	10.0, 0.0, 0.0,1.0,0.0,0.0
};
float fan1_data[] = {
	20.0, 0.0, 0.0,1.0,0.0,0.0,
	20.0, 10.0, 0.0,1.0,0.0,0.0,
	15.0, 10.0, 0.0,1.0,0.0,0.0,
	15.0, 0.0, 0.0,1.0,0.0,0.0
};

class TestScene:public Scene
{
public:
protected:
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initThisDemo(const SceneInitInfo&);
	virtual bool					update();
	virtual void					render(PassInfo&);

public:
	bool							brute_ = false;
};

bool TestScene::initSceneModels(const SceneInitInfo&)
{
	Base::SmartPointer<DynamicMeshGeoemtry> mg = new DynamicMeshGeoemtry(1, 1248 * 1248, 1248 * 1248);
	mg->RFVF() |= FVF_XYZ;
	mg->RFVF() |= FVF_NORMAL;
	mg->initGeometry();

	Mesh_SP mesh = new Mesh;
	mesh->rmode() = GL_TRIANGLE_FAN;
	mesh->RFVF() |= FVF_XYZ;
	mesh->RFVF() |= FVF_NORMAL;
	mesh->call_ = DRAW_ARRAYS;
	mg->addMesh(mesh);

	RenderNode_SP rn = new RenderNode;
	if (rn) rn->setGeometry(mg);
	addRenderNode(rn);

	return true;
}

bool TestScene::initThisDemo(const SceneInitInfo&)
{
	return true;
}

bool TestScene::update()
{

	CommonGeometry_Sp geo = ((RenderNode*)getRenderNode(0).addr())->getGeometry();
	DynamicMeshGeoemtry* dGeo = (DynamicMeshGeoemtry*)(geo.addr());

	Mesh_SP mesh = dGeo->meshs_[0];

	mesh->clear();

	for (int i = 0; i < 1000; i++)
	{
		Vertex v;
		v.Position = V3f(200.0, 0.0, 0.0);
		v.Normal = V3f(1.0, 0.0, 0.0);
		mesh->addVertex(i,v);

		v.Position = V3f(200.0, 100.0, 0.0);
		v.Normal = V3f(1.0, 0.0, 0.0);
		mesh->addVertex(i, v);

		v.Position = V3f(300.0, 100.0, 0.0);
		v.Normal = V3f(1.0, 0.0, 0.0);
		mesh->addVertex(i, v);

		v.Position = V3f(300.0, 0.0, 0.0);
		v.Normal = V3f(1.0, 0.0, 0.0);
		mesh->addVertex(i,v);
	}

	Scene::update();
	{
		CommonGeometry_Sp geo = ((RenderNode*)getRenderNode(0).addr())->getGeometry();
		DynamicMeshGeoemtry* dGeo = (DynamicMeshGeoemtry*)(geo.addr());
		uint64 vt, it;
		dGeo->cpu2GpuTime(&vt, &it);
		if (vt != 0) printScreen(std::string("CPU->GPU: " + std::to_string(vt)), 20, g_app->heigh() - 70, g_app->width(), g_app->heigh());
		if (it != 0) printScreen(std::string("CPU->GPU: " + std::to_string(it)), 20, g_app->heigh() - 90, g_app->width(), g_app->heigh());
		dGeo->swapQueryTime();
	}

	return true;
}

void TestScene::render(PassInfo&info)
{
	Scene::render(info);
}

int main()
{
	TestScene * scene = new TestScene;
	Camera *pCamera = new Camera();
	scene->setMasterCamera(pCamera);
	WindowManager *pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);
	g_app = &application;

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "Test Falsh buffer";
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
