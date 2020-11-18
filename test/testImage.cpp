
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
	virtual bool					update();

private:

	Jpeg_Data			jpegData_;
	Jpeg_Data			JpegDataHM_;

	UpdateInfo			 block_;

	base::SmartPointer<Texture>		targetTexture;
	base::SmartPointer<FrameBufferObject> frameBufferObj_;
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
	Jpeg_Data::initTechnique();

	block_.initialize(1);
	int srcBlock[2] = { /*jpegData_.width(),jpegData_.height()*/0,0 };
	int dstBlock[2];
	block_.addBlock(srcBlock, dstBlock);

	jpegData_.allocateTextures(jpegData_.width(), jpegData_.height());
	jpegData_.updateTextureData(1, jpegData_.height(), block_.getSrcBlocksPointer());

	targetTexture = new Texture();
	targetTexture->width_ = jpegData_.width();
	targetTexture->height_ = jpegData_.height();
	targetTexture->baseInternalformat_ = GL_RGBA8;
	targetTexture->createObj();
	targetTexture->bind();
	targetTexture->contextNULL();
	targetTexture->unBind();

	frameBufferObj_ = new FrameBufferObject(GL_FRAMEBUFFER);
	frameBufferObj_->bindObj(true, false);
	frameBufferObj_->colorTextureAttachments(targetTexture);
	frameBufferObj_->bindObj(false, false);
	CHECK_GL_ERROR;


	return true;
}

bool TestImgScene::initShader(const SceneInitInfo&)
{
	
	return true;
}


bool TestImgScene::update()
{
	jpegData_.uncompressTextureData();
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
