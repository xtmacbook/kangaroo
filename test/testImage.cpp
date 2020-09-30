
#include <scene.h>
#include "resource.h"
#include <gLApplication.h>
#include <camera.h>
#include <windowManager.h>
#include <glinter.h>
#include <gli.h>
#include <log.h>
#include <renderNode.h>
#include <texture.h>

class TestImgScene : public Scene
{
protected:
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual void					render(PassInfo&);
	virtual bool					initTexture(const SceneInitInfo&);
	virtual bool					initShader(const SceneInitInfo&);
};

bool TestImgScene::initSceneModels(const SceneInitInfo&)
{
	Base::SmartPointer<Quad> quad = new Quad;
	quad->initGeometry();
	RenderNode_SP quadRenderNode = new RenderNode;
	quadRenderNode->setGeometry(quad);
	addRenderNode(quadRenderNode);
	return true;
}

void TestImgScene::render(PassInfo&ps)
{
	Shader * shader = shaders_[0];
	shader->turnOn();
	initUniformVal(shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D ,texturesObj_[0]->getTexture());

	getRenderNode(0)->render(shader, ps);
	shader->turnOff();
}

bool TestImgScene::initTexture(const SceneInitInfo&)
{
	std::string tx = get_texture_BasePath() + "heightmap.jpg";
	Texture* curTexObj = new Texture(tx.c_str());
	if (curTexObj->loadData())
	{
		curTexObj->createObj();
		curTexObj->bind();
		curTexObj->mirrorRepeat();
		curTexObj->filterLinear();
		if (curTexObj->context(NULL))
		{
			curTexObj->unBind();
			texturesObj_.push_back(curTexObj);
		}
		else return false;
		CHECK_GL_ERROR;
	}

	return true;
}

bool TestImgScene::initShader(const SceneInitInfo&)
{

	const char gVertexShaderSource[] =
	{
		"#version 330 core\n"
		"layout (location = 0 ) in vec2 position;\n"
		"layout (location = 1 ) in vec2 texCoord;\n"

		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"

		"out vec2 uv;\n"
		"void main(void)\n"
		"{\n"
		"  gl_Position = projection * view * model * vec4(position,0.0,1.0);\n"
		"  uv = texCoord;\n"
		"}\n"
	};

	const char gFragmentShaderSource[] =
	{
		"#version 330 core\n"

		"out vec4 color;\n"
		"in vec2 uv;\n"

		"uniform sampler2D sampleTexture;\n"

		"void main(void)\n"
		"{\n"
		"  color = texture(sampleTexture, uv );\n"
		"}\n"
	};


	shaders_.push_back(new Shader);
	Shader * currentShader = shaders_[0];
	if (currentShader->loadShaderSouce(gVertexShaderSource, gFragmentShaderSource, nullptr) == -1) return false;
	currentShader->turnOn();
	int eunit = currentShader->getVariable("sampleTexture");
	glUniform1i(eunit, 0);

	return true;
}

int main()
{
	TestImgScene * scene = new TestImgScene;
	Camera *pCamera = new Camera();
	scene->setMasterCamera(pCamera);
	WindowManager *pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "test_Image";
	wc.width_ = 1024;
	wc.height_ = 960;
	wc.pos_x_ = 50;
	wc.pos_y_ = 50;

	dc.glVersion_.glMaxJor_ = 3.0;
	dc.glVersion_.glMinJor_ = 3.0;

	application.initialize(&wc, &dc);

	application.initScene();
	pCamera->setClipPlane(0.1f, 500.0f);
	application.start();

	return 0;
}
