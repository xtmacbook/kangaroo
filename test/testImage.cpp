
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
#include "jpgd.h"
#include "framebuffers.h"
#include "jpeg_gpu.h"

#define  MAX_CHANELS 3




class TestImgScene : public Scene
{
protected:
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual void					render(PassInfo&);
	virtual bool					initTexture(const SceneInitInfo&);
	virtual bool					initShader(const SceneInitInfo&);

private:
	int					imageWidth;
	int					imageHeight;
	int					componentsNum;

	char				quantTables[MAX_CHANELS][64];

	int					MCU_per_row;
	int					MCU_blocks_num;

	Jpeg_Data			jpegData_;
};



bool TestImgScene::initSceneModels(const SceneInitInfo&)
{
	base::SmartPointer<Quad> quad = new Quad;
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

	getRenderNode(0)->render(shader, ps);
	shader->turnOff();
}

bool TestImgScene::initTexture(const SceneInitInfo&)
{
	std::string jpegFile = "D:/download/Clipmaps/Direct3D/Media/Clipmaps/Mars1k.jpg";
	jpegData_.loadFile(jpegFile.c_str());


	return true;
}

bool TestImgScene::initShader(const SceneInitInfo&)
{
	Shader * shader = new Shader;
	std::string code = Shader::loadMultShaderInOneFile("test/jpg_gpu.glsl");

	shader->getShaderFromMultCode(Shader::VERTEX, "Quad", code);
	shader->getShaderFromMultCode(Shader::FRAGMENT, "Quad", code);
	shader->linkProgram();
	shader->checkProgram();
	shaders_.push_back(shader);

	shader = new Shader;
	shader->getShaderFromMultCode(Shader::VERTEX, "Quad", code);
	shader->getShaderFromMultCode(Shader::FRAGMENT, "PS_IDCT_Rows", code);
	shader->linkProgram();
	shader->checkProgram();

	shader = new Shader;
	shader->getShaderFromMultCode(Shader::VERTEX, "Quad", code);
	shader->getShaderFromMultCode(Shader::FRAGMENT, "PS_IDCT_Unpack_Rows", code);
	shader->linkProgram();
	shader->checkProgram();

	shaders_.push_back(shader);

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
