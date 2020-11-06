
#include <scene.h>
#include <RenderNode.h>
#include <shader.h>
#include <gLApplication.h>
#include <fspCamera.h>
#include <windowManager.h>
#include <log.h>
#include <Inputmanager.h>
#include <glinter.h>
#include <resource.h>
#include <camera.h>

#include "type.h"
#include "boundingBox.h"
#include <vector>


#define FILE_BLOCK_SIZE 512

#define  SPHERE_MERIDIAN_SLICES_NUM  128
#define  SPHERE_PARALLEL_SLICES_NUM  128

#define CLIPMAP_STACK_SIZE_MAX 4096
#define CLIPMAP_STACK_SIZE_MIN 1024
#define MIPMAP_LEVELS_MAX 7

using namespace math;
 

class ClipMappingScene :public Scene
{
public:


	struct SphereGeoemtry
	{
		void setUpGeoemtry()
		{

#if 0
			std::vector<V3f> vertexs;
			for (int i = 0; i < SPHERE_MERIDIAN_SLICES_NUM * SPHERE_PARALLEL_SLICES_NUM; i++)
			{
				float meridianPart = (i % (SPHERE_MERIDIAN_SLICES_NUM + 1)) / float(SPHERE_MERIDIAN_SLICES_NUM);
				float parallelPart = (i / (SPHERE_MERIDIAN_SLICES_NUM + 1)) / float(SPHERE_PARALLEL_SLICES_NUM);

				float angle1 = meridianPart * 3.14159265 * 2.0;
				float angle2 = (0.5 - parallelPart) * 3.14159265;

				float cos_angle1 = cos(angle1);
				float sin_angle1 = sin(angle1);
				float cos_angle2 = cos(angle2);
				float sin_angle2 = sin(angle2);

				V3f VertexPosition;
				VertexPosition.x = cos_angle1 * cos_angle2;
				VertexPosition.y = sin_angle2;
				VertexPosition.z = sin_angle1 * cos_angle2;

				vertexs.push_back(VertexPosition);
			}
			
#endif
			int IndexNumber = SPHERE_MERIDIAN_SLICES_NUM * SPHERE_PARALLEL_SLICES_NUM * 6;

			unsigned *pIndices = new unsigned[IndexNumber];
			unsigned indexCount = 0;

			for (int i = 0; i < SPHERE_PARALLEL_SLICES_NUM; i++)
			{
				for (int j = 0; j < SPHERE_MERIDIAN_SLICES_NUM; j++)
				{
					pIndices[indexCount] = i * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j;
					indexCount++;

					pIndices[indexCount] = (i + 1) * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j;
					indexCount++;

					pIndices[indexCount] = (i + 1) * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j + 1;
					indexCount++;

					
					pIndices[indexCount] = i * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j;
					indexCount++;

					pIndices[indexCount] = (i + 1) * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j + 1;
					indexCount++;

					pIndices[indexCount] = i * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j + 1;
					indexCount++;

					
				}
			}

			glGenVertexArrays(1, &vao_);

			glBindVertexArray(vao_);
			glGenBuffers(1, &vbo_);

			glGenBuffers(1, &vvbo_);
			glBindBuffer(GL_ARRAY_BUFFER, vvbo_);
			glBufferData(GL_ARRAY_BUFFER, 0,0, GL_STATIC_DRAW);
			glBufferData(GL_ARRAY_BUFFER, sizeof(V3f) * vertexs.size(), &vertexs[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned), pIndices, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		 
		void draw()
		{
			glBindVertexArray(vao_);
			glDrawElements(GL_TRIANGLES, SPHERE_MERIDIAN_SLICES_NUM * SPHERE_MERIDIAN_SLICES_NUM + 30,GL_UNSIGNED_INT,0);
			glBindVertexArray(0);
		}

		GLuint  vao_;
		GLuint  vbo_;
		GLuint  vvbo_;
	};

	ClipMappingScene();
	virtual ~ClipMappingScene();
protected:
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initShader(const SceneInitInfo&);

	virtual bool					update();
	virtual void					processKeyboard(int key, int st, int action, int mods, float deltaTime);

 
protected:
	void							calculateClipmapParameters();
	void							initStackTexture();
	void							updateMipPosition(int &position, int offset);
	void							updatestackTexture(const V3f&eyePos);

public:
	virtual void					render(PassInfo&);

private:
	int        g_SourceImageWidth;
	int        g_SourceImageHeight;
	int        g_SourceImageMipsNum;
	int**      g_ppSourceImageMipsSize = nullptr;
	int*       g_pMipDataOffset = nullptr;     // Offset in bytes for each mip level data stored in a temporary file

	int			g_UpdateRegionSize = 64;
	int			g_ClipmapStackSize;
	std::vector<int>    g_StackSizeList;
	std::vector<int>    g_UpdateRegionSizeList;

	math::V2f g_StackPosition;
	int        g_StackDepth;
	int **g_ppUpdatePositions;                           // Defines positions for each clipmap layer where new data should be placed


	SphereGeoemtry		sphere_;
};

ClipMappingScene::ClipMappingScene()
{
}

ClipMappingScene::~ClipMappingScene()
{
	
}

void ClipMappingScene::updateMipPosition(int &position, int offset)
{
	position += offset;

	if (offset > 0)
	{
		if (position > g_ClipmapStackSize - offset)
			position = 0;
	}
	else
	{
		if (position < 0)
			position = g_ClipmapStackSize + offset;
	}
}

void ClipMappingScene::updatestackTexture(const V3f&eyePos)
{
	float length = sqrtf(eyePos.x * eyePos.x + eyePos.z * eyePos.z);
	float posHorizontal;
	float posVertical;

	int tileBlockSize = 0;
	int mipCornerLU[2];
	int mipCornerRD[2];
	int srcBlock[2];
	int dstBlock[2];

	base::AABB subResourceBox;
	subResourceBox.min_.z = 0;
	subResourceBox.max_.z = 1;
	unsigned correction = 0;

	if (1)
	{
		for (int i = 0; i < g_StackDepth; ++i)
		{
			tileBlockSize = g_UpdateRegionSize / (unsigned)pow(2.0, i);

			// Calculate regions of source image that should be updated in the current stack layer
			mipCornerLU[0] = int(g_StackPosition.x * g_ppSourceImageMipsSize[i][0]) + g_ClipmapStackSize / 2;
			if (mipCornerLU[0] > int(g_ppSourceImageMipsSize[i][0] - tileBlockSize))
				mipCornerLU[0] -= g_ppSourceImageMipsSize[i][0];
			mipCornerRD[0] = mipCornerLU[0] + tileBlockSize;

			mipCornerLU[1] = int(g_StackPosition.y * g_ppSourceImageMipsSize[i][1]) - g_ClipmapStackSize / 2;
			mipCornerRD[1] = int(g_StackPosition.y * g_ppSourceImageMipsSize[i][1]) + g_ClipmapStackSize / 2;

			if (mipCornerLU[1] < 0)
			{
				correction = -mipCornerLU[1];
				mipCornerLU[1] = 0;
			}
			else if (mipCornerLU[1] >= g_ppSourceImageMipsSize[i][1])
			{
				updateMipPosition(g_ppUpdatePositions[i][0], tileBlockSize);
				continue;
			}

			if (mipCornerRD[1] <= 0)
			{
				updateMipPosition(g_ppUpdatePositions[i][0], tileBlockSize);
				continue;
			}
			else if (mipCornerRD[1] > g_ppSourceImageMipsSize[i][1])
				mipCornerRD[1] = g_ppSourceImageMipsSize[i][1];

			subResourceBox.min_.x = g_ppUpdatePositions[i][0];
			subResourceBox.max_.x = subResourceBox.min_.x + tileBlockSize;
			subResourceBox.min_.y = g_ppUpdatePositions[i][1] + correction;

			for (int j = mipCornerLU[1]; j < mipCornerRD[1]; j += tileBlockSize)
			{
				subResourceBox.max_.y = subResourceBox.min_.y + tileBlockSize;

				srcBlock[0] = mipCornerLU[0];
				srcBlock[1] = j;

				dstBlock[0] = subResourceBox.min_.x;
				dstBlock[1] = subResourceBox.min_.y;

				//g_JPEG_Manager.AddBlock(i, srcBlock, dstBlock);

				subResourceBox.min_.y += tileBlockSize;

				if (subResourceBox.min_.y > UINT(g_ClipmapStackSize - tileBlockSize))
					subResourceBox.min_.y = 0;
			}

			//g_JPEG_Manager.Update(pd3dDevice, i);
			updateMipPosition(g_ppUpdatePositions[i][0], tileBlockSize);
		}

		g_StackPosition.x += (float)g_UpdateRegionSize / g_SourceImageWidth;
	}
}


bool ClipMappingScene::initSceneModels(const SceneInitInfo&)
{
	g_SourceImageWidth = 16384;
	g_SourceImageHeight = 8192;

	int baseDimension = CLIPMAP_STACK_SIZE_MAX;

	while (baseDimension >= CLIPMAP_STACK_SIZE_MIN)
	{
		g_StackSizeList.push_back(baseDimension);
		baseDimension >>= 1;
	}

	g_ClipmapStackSize = g_StackSizeList[1];

	calculateClipmapParameters();

	g_UpdateRegionSize = (int)(16.0 * pow(2.0f, g_StackDepth - 1));

	initStackTexture();

	sphere_.setUpGeoemtry();

	return true;
}

bool ClipMappingScene::initShader(const SceneInitInfo&)
{
	Shader * shader = new Shader;
	std::string code = Shader::loadMultShaderInOneFile("test/clipmap.glsl");

	shader->getShaderFromMultCode(Shader::VERTEX, "Compiled", code);
	shader->getShaderFromMultCode(Shader::FRAGMENT, "base", code);
	shader->linkProgram();
	shader->checkProgram();

	shaders_.push_back(shader);
	shader->turnOn();
	shader->setInt(shader->getVariable("g_SphereMeridianSlices"), SPHERE_MERIDIAN_SLICES_NUM);
	shader->setInt(shader->getVariable("g_SphereParallelSlices"), SPHERE_PARALLEL_SLICES_NUM);
	shader->turnOff();

	CHECK_GL_ERROR;
	/*texture0_ = shader_->getVariable("texture0");
	viewportOrthographicMatrix_ = shader_->getVariable("viewportOrthographicMatrix");
	initUniform();*/
	return true;
}

bool ClipMappingScene::update()
{
	updatestackTexture(getCamera()->getPosition());
	return true;
}

void ClipMappingScene::processKeyboard(int key, int st, int action, int mods, float deltaTime)
{
	Scene::processKeyboard(key, st, action, mods, deltaTime);
}
 

void ClipMappingScene::calculateClipmapParameters()
{
	g_StackDepth = 0;
	int dimensionMax = g_SourceImageWidth;

	if (g_SourceImageHeight > g_SourceImageWidth)
		dimensionMax = g_SourceImageHeight;

	g_SourceImageMipsNum = int(log((double)dimensionMax + 1.0) / log(2.0)) + 1;

	for (int i = 0; i < dimensionMax; ++i)
	{
		if (dimensionMax / pow(2.0f, g_StackDepth) > g_ClipmapStackSize)
		{
			g_StackDepth++;
		}
		else
		{
			break;
		}
	}

	g_ppSourceImageMipsSize = new int*[g_SourceImageMipsNum];

	for (int i = 0; i < g_SourceImageMipsNum; ++i)
	{
		g_ppSourceImageMipsSize[i] = new int[2];

		g_ppSourceImageMipsSize[i][0] = g_SourceImageWidth / (unsigned)pow(2.0f, i);
		g_ppSourceImageMipsSize[i][1] = g_SourceImageHeight / (unsigned)pow(2.0f, i);
	}

	g_StackPosition.x = 0.0f;
	g_StackPosition.y = 0.5f;

	assert(g_StackDepth);

	g_pMipDataOffset = new int[g_StackDepth];
	g_ppUpdatePositions = new int*[g_StackDepth];

	for (int i = 0; i < g_StackDepth; ++i)
	{
		g_ppUpdatePositions[i] = new int[2];

		g_ppUpdatePositions[i][0] = 0;
		g_ppUpdatePositions[i][1] = 0;
	}

}

void ClipMappingScene::initStackTexture()
{
	int mipCornerLU[2];
	int mipCornerRD[2];
	int tileBlockSize = 0;
	int blockIndex[2];
	int subBlockIndex[2];
	int blockCorner[2];

	base::AABB subResourceBox;
	subResourceBox.min_.z = 0;
	subResourceBox.max_.z = 1;

	int srcBlock[2];
	int dstBlock[2];

	for (int i = 0; i < g_StackDepth; ++i)
	{
		mipCornerLU[0] = int(g_StackPosition.x * g_ppSourceImageMipsSize[i][0] - g_ClipmapStackSize* 0.5f);
		mipCornerLU[1] = int(g_StackPosition.y * g_ppSourceImageMipsSize[i][1] - g_ClipmapStackSize* 0.5f);

		mipCornerRD[0] = int(g_StackPosition.x * g_ppSourceImageMipsSize[i][0] + g_ClipmapStackSize* 0.5f);
		mipCornerRD[1] = int(g_StackPosition.y * g_ppSourceImageMipsSize[i][1] + g_ClipmapStackSize* 0.5f);

		tileBlockSize = int(g_UpdateRegionSize / pow(2.0, i));

		subResourceBox.min_.y = 0;
		subResourceBox.max_.y = subResourceBox.min_.y + tileBlockSize;

		if (g_ppSourceImageMipsSize[i][1] == g_ClipmapStackSize)
		{
			g_ppUpdatePositions[i][1] = g_ClipmapStackSize / 2;

			for (int j = mipCornerLU[1] + g_ClipmapStackSize / 2; j < mipCornerRD[1]; j += tileBlockSize)
			{
				subResourceBox.min_.x = 0;
				subResourceBox.max_.x = subResourceBox.min_.x + tileBlockSize;

				blockIndex[1] = j / FILE_BLOCK_SIZE;
				blockCorner[1] = blockIndex[1] * FILE_BLOCK_SIZE;
				subBlockIndex[1] = (j - blockCorner[1]) / tileBlockSize;

				for (int k = mipCornerLU[0]; k < mipCornerRD[0]; k += tileBlockSize)
				{
					int tempBoundary = k;

					if (tempBoundary < 0)
						tempBoundary += g_ppSourceImageMipsSize[i][0];

					blockIndex[0] = tempBoundary / FILE_BLOCK_SIZE;

					blockCorner[0] = blockIndex[0] * FILE_BLOCK_SIZE;
					subBlockIndex[0] = (tempBoundary - blockCorner[0]) / tileBlockSize;

					srcBlock[0] = tempBoundary;
					srcBlock[1] = j;

					dstBlock[0] = subResourceBox.min_.x;
					dstBlock[1] = subResourceBox.min_.y;

					//g_JPEG_Manager.AddBlock(i, srcBlock, dstBlock);

					subResourceBox.min_.x += tileBlockSize;
					subResourceBox.max_.x += tileBlockSize;
				}

				//g_JPEG_Manager.Update(pd3dDevice, i);

				subResourceBox.min_.y += tileBlockSize;
				subResourceBox.max_.y += tileBlockSize;
			}

			subResourceBox.min_.y = g_ClipmapStackSize / 2;
			subResourceBox.max_.y = subResourceBox.min_.y + tileBlockSize;

			for (int j = mipCornerLU[1]; j < mipCornerRD[1] - g_ClipmapStackSize* 0.5; j += tileBlockSize)
			{
				subResourceBox.min_.x = 0;
				subResourceBox.max_.x = subResourceBox.min_.x + tileBlockSize;

				blockIndex[1] = j / FILE_BLOCK_SIZE;
				blockCorner[1] = blockIndex[1] * FILE_BLOCK_SIZE;
				subBlockIndex[1] = (j - blockCorner[1]) / tileBlockSize;

				for (int k = mipCornerLU[0]; k < mipCornerRD[0]; k += tileBlockSize)
				{
					int tempBoundary = k;

					if (tempBoundary < 0)
						tempBoundary += g_ppSourceImageMipsSize[i][0];

					blockIndex[0] = tempBoundary / FILE_BLOCK_SIZE;

					blockCorner[0] = blockIndex[0] * FILE_BLOCK_SIZE;
					subBlockIndex[0] = (tempBoundary - blockCorner[0]) / tileBlockSize;

					srcBlock[0] = tempBoundary;
					srcBlock[1] = j;

					dstBlock[0] = subResourceBox.min_.x;
					dstBlock[1] = subResourceBox.min_.y;

					//	g_JPEG_Manager.AddBlock(i, srcBlock, dstBlock);

					subResourceBox.min_.x += tileBlockSize;
					subResourceBox.max_.y += tileBlockSize;
				}

				//g_JPEG_Manager.Update(pd3dDevice, i);

				subResourceBox.min_.y += tileBlockSize;
				subResourceBox.max_.y += tileBlockSize;
			}
		}
		else
		{
			for (int j = mipCornerLU[1]; j < mipCornerRD[1]; j += tileBlockSize)
			{
				subResourceBox.min_.x = 0;
				subResourceBox.max_.x = subResourceBox.min_.x + tileBlockSize;

				blockIndex[1] = j / FILE_BLOCK_SIZE;
				blockCorner[1] = blockIndex[1] * FILE_BLOCK_SIZE;
				subBlockIndex[1] = (j - blockCorner[1]) / tileBlockSize;

				for (int k = mipCornerLU[0]; k < mipCornerRD[0]; k += tileBlockSize)
				{
					int tempBoundary = k;

					if (tempBoundary < 0)
						tempBoundary += g_ppSourceImageMipsSize[i][0];

					blockIndex[0] = tempBoundary / FILE_BLOCK_SIZE;

					blockCorner[0] = blockIndex[0] * FILE_BLOCK_SIZE;
					subBlockIndex[0] = (tempBoundary - blockCorner[0]) / tileBlockSize;

					srcBlock[0] = tempBoundary;
					srcBlock[1] = j;

					dstBlock[0] = subResourceBox.min_.x;
					dstBlock[1] = subResourceBox.min_.y;

					//	g_JPEG_Manager.AddBlock(i, srcBlock, dstBlock);

					subResourceBox.min_.x += tileBlockSize;
					subResourceBox.max_.x += tileBlockSize;
				}

				//g_JPEG_Manager.Update(pd3dDevice, i);

				subResourceBox.min_.y += tileBlockSize;
			}	subResourceBox.max_.y += tileBlockSize;
		}
	}
}

void ClipMappingScene::render(PassInfo&info)
{
	shaders_[0]->turnOn();

	initUniformVal(shaders_[0]);

	sphere_.draw();

	shaders_[0]->turnOff();
	
	CHECK_GL_ERROR;
}

int main()
{
	ClipMappingScene * scene = new ClipMappingScene;
	Camera *pCamera = new Camera();
	pCamera->positionCamera(0.0, 0.0, -2.5, 0.0, 0.0, 0.0,0.0, 1.0, 0.0);

	pCamera->setClipPlane(1.0f, 5000.0f);
	scene->setMasterCamera(pCamera);
	WindowManager *pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.shouldCreateSharedContext_ = true;
	wc.title_ = "texture ClipMappings terrain";
	wc.width_ = 1024;
	wc.height_ = 906;
	wc.pos_x_ = 50;
	wc.pos_y_ = 50;

	application.initialize(&wc, &dc);

	application.initScene();

	application.start();
}

