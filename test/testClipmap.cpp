
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
#include "clipmap_prepprocessor.h"
#include <util.h>
#include <engineLoad.h>
#include <vector>


#define FILE_BLOCK_SIZE 512

#define  SPHERE_MERIDIAN_SLICES_NUM  128
#define  SPHERE_PARALLEL_SLICES_NUM  128

#define CLIPMAP_STACK_SIZE_MAX 4096
#define CLIPMAP_STACK_SIZE_MIN 1024
#define MIPMAP_LEVELS_MAX 7

#define SOURCE_FILES_NUM 5

#define  QUAD_TEST_STACK_TEXTURE

const V3f LIGHTPOS{ 10.0,0.0,10.0 };

char g_SrcMediaPath[SOURCE_FILES_NUM][MAX_PATH] = { "Clipmaps//Mars16k.jpg",
													"Clipmaps/Mars8k.jpg",
													"Clipmaps/Mars4k.jpg",
													"Clipmaps/Mars2k.jpg",
													"Clipmaps/Mars1k.jpg" };

char g_SrcMediaPathHM[SOURCE_FILES_NUM][MAX_PATH] = { "Clipmaps//MarsHm16k.jpg",
													"Clipmaps/MarsHm8k.jpg",
													"Clipmaps/MarsHm4k.jpg",
													"Clipmaps/MarsHm2k.jpg",
													"Clipmaps/MarsHm1k.jpg" };

float g_MipmapColors[MIPMAP_LEVELS_MAX][3] =
{
	{ 0.5f, 1.0f, 0.0f },
	{ 0.0f, 1.0f, 1.0f },
	{ 1.0f, 1.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f },
	{ 1.0f, 0.0f, 1.0f },
	{ 1.0f, 0.0f, 0.0f },
};

static GLfloat quadVertices[] = {
	// positions   // texCoords
	-512.0f,  256.0f,  0.0f, 1.0f,
	-512.0f, -256.0f,  0.0f, 0.0f,
	512.0f, -256.0f,  1.0f, 0.0f,

	-512.0f,  256.0f,  0.0f, 1.0f,
	512.0f, -256.0f,  1.0f, 0.0f,
	512.0f,  256.0f,  1.0f, 1.0f
};

using namespace math;
 

class ClipMappingScene :public Scene
{
public:


	struct SphereGeoemtry
	{
		void setUpGeoemtry()
		{


#ifdef QUAD_TEST_STACK_TEXTURE
			glGenVertexArrays(1, &vao_);
			glBindVertexArray(vao_);
			glGenBuffers(1, &vbo_);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
			glBindVertexArray(0);
#else
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
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned), pIndices, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
#endif
		
		}

		 
		void draw()
		{
#ifdef QUAD_TEST_STACK_TEXTURE
			glBindVertexArray(vao_);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
#else
			glBindVertexArray(vao_);
			glDrawElements(GL_TRIANGLES, SPHERE_MERIDIAN_SLICES_NUM * SPHERE_MERIDIAN_SLICES_NUM * 6,GL_UNSIGNED_INT,0);
			glBindVertexArray(0);
#endif
			
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
	void							createClipmapTextures();
	void							calculateClipmapParameters();
	void							initStackTexture();
	void							updateMipPosition(int &position, int offset);
	void							updatestackTexture(const V3f&eyePos);

public:
	virtual void					render(PassInfo&);

	base::SmartPointer<Texture>			g_pPyramidTexture;
	base::SmartPointer<Texture>			g_pPyramidTextureHM;
	base::SmartPointer<Texture>			g_pStackTexture;
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

	Clipmap_Manager*		clipmapManager_;
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

	if (eyePos.z >= 0)
	{
		if (eyePos.x <= 0)
			posHorizontal = atanf(-(eyePos.x / eyePos.z)) / (PI * 2);
		else
			posHorizontal = 1.0f - atanf(eyePos.x / eyePos.z) / (PI * 2);
	}
	else
	{
		posHorizontal = 0.5f - atanf((eyePos.x / eyePos.z)) / (PI * 2);
	}

	posVertical = 0.5f - atanf(eyePos.y / length) / PI;

	V2f updateBorder;
	// Calculate border sizes to be updated
	updateBorder.x = posHorizontal - g_StackPosition.x;

	if (updateBorder.x > 0)
	{
		if (updateBorder.x > 0.5f)
			updateBorder.x = updateBorder.x - 1.0f;
	}
	else
	{
		if (updateBorder.x < -0.5f)
			updateBorder.x += 1.0f;
	}

	updateBorder.y = posVertical - g_StackPosition.y;


	int updateBorderSize[2];
	updateBorderSize[0] = int(updateBorder.x * g_SourceImageWidth);
	updateBorderSize[1] = int(updateBorder.y * g_SourceImageHeight);

	int srcBlock[2];
	int dstBlock[2];

	int tileBlockSize = 0;
	int mipCornerLU[2];
	int mipCornerRD[2];

	base::AABB subResourceBox;
	subResourceBox.min_.z = 0;
	subResourceBox.max_.z = 1;
	unsigned correction = 0;

	if (updateBorderSize[0] > g_UpdateRegionSize)
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

				clipmapManager_->addBlock(i, srcBlock, dstBlock);

				subResourceBox.min_.y += tileBlockSize;

				if (subResourceBox.min_.y > UINT(g_ClipmapStackSize - tileBlockSize))
					subResourceBox.min_.y = 0;
			}

			clipmapManager_->update( i,g_pStackTexture);
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

	g_ClipmapStackSize = g_StackSizeList[1]; //now select 1 index

	calculateClipmapParameters();

	g_UpdateRegionSize = (int)(16.0 * pow(2.0f, g_StackDepth - 1));

	int blocksPerLayer = g_SourceImageWidth / g_UpdateRegionSize;

	clipmapManager_ = new Clipmap_Manager;
	clipmapManager_->intitialize(g_StackDepth, g_SrcMediaPath,nullptr/*g_SrcMediaPathHM*/);
	clipmapManager_->allocateBlocks(blocksPerLayer);
	clipmapManager_->allocateTextures(g_ClipmapStackSize,g_UpdateRegionSize);

	createClipmapTextures();

	initStackTexture();

	sphere_.setUpGeoemtry();

	CHECK_GL_ERROR;
	return true;
}

bool ClipMappingScene::initShader(const SceneInitInfo&)
{
#ifdef QUAD_TEST_STACK_TEXTURE
	Shader * shader = new Shader;
	//因为obj模型原因此处使用normal作为color
	char vertShder[] = "#version 330 core \n"
		"layout(location = 0) in vec2 position;"
		"layout(location = 1) in vec2 texCoord;"

		"uniform mat4 view;"
		"uniform mat4 projection;"
		"out vec2 texCoords;"
		"void main()"
		"{"
		"gl_Position = projection * view *  vec4(position,1.0, 1.0f);"
		"texCoords = texCoord;"
		"}";

	char fragShader[] = "#version 330 core \n"
		"out vec4 color;"
		"in vec2 texCoords;"
		"uniform sampler2DArray diffuseTex;"
		"void main()"
		"{"
		"color = texture(diffuseTex,vec3(texCoords,0));"
		"}";

	shader->loadShaderSouce(vertShder, fragShader, NULL);
	shaders_.push_back(shader);

	shader->turnOn();
	unsigned loc = shader->getVariable("diffuseTex");
	if (loc)
	{
		shader->setInt(loc, 0);
	}
#else
	Shader * shader = new Shader;
	std::string code = Shader::loadMultShaderInOneFile("test/clipmap.glsl");

	shader->getShaderFromMultCode(Shader::VERTEX, "Compiled", code);
	shader->getShaderFromMultCode(Shader::FRAGMENT, "trilinear", code);
	shader->linkProgram();
	shader->checkProgram();

	shaders_.push_back(shader);
	shader->turnOn();

	shader->setInt(shader->getVariable("g_SphereMeridianSlices"), SPHERE_MERIDIAN_SLICES_NUM);
	shader->setInt(shader->getVariable("g_SphereParallelSlices"), SPHERE_PARALLEL_SLICES_NUM);

	V2i textureSize{ g_SourceImageWidth,g_SourceImageHeight };
	shader->setVec2Int(shader->getVariable("g_TextureSize"), 1, &textureSize[0]);

	shader->setInt(shader->getVariable("StackTexture"), 0);
	shader->setInt(shader->getVariable("PyramidTexture"), 1);
	shader->setInt(shader->getVariable("PyramidTextureHM"), 2);

	shader->turnOff();
#endif

	CHECK_GL_ERROR;
	/*texture0_ = shader_->getVariable("texture0");
	viewportOrthographicMatrix_ = shader_->getVariable("viewportOrthographicMatrix");
	initUniform();*/
	return true;
}

bool ClipMappingScene::update()
{
	//updatestackTexture(getCamera()->getPosition());
	return true;
}

void ClipMappingScene::processKeyboard(int key, int st, int action, int mods, float deltaTime)
{
	Scene::processKeyboard(key, st, action, mods, deltaTime);
}
 

void ClipMappingScene::createClipmapTextures()
{
	g_pPyramidTexture = new Texture(g_SrcMediaPath[g_StackDepth]);
	g_pPyramidTexture->target_ = GL_TEXTURE_2D;
	g_pPyramidTexture->numOfMiplevels_ = g_SourceImageMipsNum - g_StackDepth;
	if (g_pPyramidTexture->loadData())
	{
		g_pPyramidTexture->createObj();
		g_pPyramidTexture->bind();
		g_pPyramidTexture->mirrorRepeat();
		g_pPyramidTexture->filterLinear();
		if (!g_pPyramidTexture->context(NULL))
		{
		}
		CHECK_GL_ERROR;
	}

	g_pPyramidTextureHM = new Texture(g_SrcMediaPathHM[g_StackDepth]);
	g_pPyramidTextureHM->target_ = GL_TEXTURE_2D;
	g_pPyramidTextureHM->numOfMiplevels_ = g_SourceImageMipsNum - g_StackDepth;
	if (g_pPyramidTextureHM->loadData())
	{
		g_pPyramidTextureHM->createObj();
		g_pPyramidTextureHM->bind();
		g_pPyramidTextureHM->mirrorRepeat();
		g_pPyramidTextureHM->filterLinear();
		if (!g_pPyramidTextureHM->context(NULL))
		{
		}
		CHECK_GL_ERROR;
	}

	g_pStackTexture = new Texture;
	g_pStackTexture->target_ = GL_TEXTURE_2D_ARRAY;
	g_pStackTexture->internalformat_ = GL_RGBA8;
	g_pStackTexture->format_ = GL_RGBA;
	g_pStackTexture->type_ = GL_HALF_FLOAT;;
	g_pStackTexture->width_ = g_ClipmapStackSize;
	g_pStackTexture->height_ = g_ClipmapStackSize;
	g_pStackTexture->depth_ = g_StackDepth; //array size;
	g_pStackTexture->createObj();
	g_pStackTexture->bind();
	g_pStackTexture->mirrorRepeat();
	g_pStackTexture->filterLinear();
	g_pStackTexture->contextNULL();
	g_pStackTexture->unBind();

	CHECK_GL_ERROR;
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

	shaders_[0]->turnOn();
	shaders_[0]->setUInt(shaders_[0]->getVariable("g_StackDepth"), g_StackDepth);
	V2f scaleFactor;
	scaleFactor.x = (float)g_SourceImageWidth / g_ClipmapStackSize;
	scaleFactor.y = (float)g_SourceImageHeight / g_ClipmapStackSize;
	shaders_[0]->setVec2f(shaders_[0]->getVariable("g_ScaleFactor"), 1, &scaleFactor[0]);
	shaders_[0]->setVec3f(shaders_[0]->getVariable("g_MipColors"), MIPMAP_LEVELS_MAX, g_MipmapColors[0]);
	shaders_[0]->turnOff();
}

void ClipMappingScene::initStackTexture()
{
	int mipCornerLU[2];
	int mipCornerRD[2];
	int tileBlockSize = 0;

	/*int blockIndex[2];
	int subBlockIndex[2];
	int blockCorner[2];*/

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

				/*blockIndex[1] = j / FILE_BLOCK_SIZE;
				blockCorner[1] = blockIndex[1] * FILE_BLOCK_SIZE;
				subBlockIndex[1] = (j - blockCorner[1]) / tileBlockSize;*/

				for (int k = mipCornerLU[0]; k < mipCornerRD[0]; k += tileBlockSize)
				{
					int tempBoundary = k;

					if (tempBoundary < 0)
						tempBoundary += g_ppSourceImageMipsSize[i][0];

					/*blockIndex[0] = tempBoundary / FILE_BLOCK_SIZE;
					blockCorner[0] = blockIndex[0] * FILE_BLOCK_SIZE;
					subBlockIndex[0] = (tempBoundary - blockCorner[0]) / tileBlockSize;*/

					srcBlock[0] = tempBoundary;
					srcBlock[1] = j;

					dstBlock[0] = subResourceBox.min_.x;
					dstBlock[1] = subResourceBox.min_.y;

					clipmapManager_->addBlock(i, srcBlock, dstBlock);

					subResourceBox.min_.x += tileBlockSize;
					subResourceBox.max_.x += tileBlockSize;
				}

				clipmapManager_->update(i, g_pStackTexture);

				subResourceBox.min_.y += tileBlockSize;
				subResourceBox.max_.y += tileBlockSize;
			}

			subResourceBox.min_.y = g_ClipmapStackSize / 2;
			subResourceBox.max_.y = subResourceBox.min_.y + tileBlockSize;

			for (int j = mipCornerLU[1]; j < mipCornerRD[1] - g_ClipmapStackSize* 0.5; j += tileBlockSize)
			{
				subResourceBox.min_.x = 0;
				subResourceBox.max_.x = subResourceBox.min_.x + tileBlockSize;

				/*blockIndex[1] = j / FILE_BLOCK_SIZE;
				blockCorner[1] = blockIndex[1] * FILE_BLOCK_SIZE;
				subBlockIndex[1] = (j - blockCorner[1]) / tileBlockSize;*/

				for (int k = mipCornerLU[0]; k < mipCornerRD[0]; k += tileBlockSize)
				{
					int tempBoundary = k;

					if (tempBoundary < 0)
						tempBoundary += g_ppSourceImageMipsSize[i][0];

					/*blockIndex[0] = tempBoundary / FILE_BLOCK_SIZE;
					blockCorner[0] = blockIndex[0] * FILE_BLOCK_SIZE;
					subBlockIndex[0] = (tempBoundary - blockCorner[0]) / tileBlockSize;*/

					srcBlock[0] = tempBoundary;
					srcBlock[1] = j;

					dstBlock[0] = subResourceBox.min_.x;
					dstBlock[1] = g_ClipmapStackSize - subResourceBox.max_.y;

					clipmapManager_->addBlock(i, srcBlock, dstBlock);

					subResourceBox.min_.x += tileBlockSize;
					subResourceBox.max_.y += tileBlockSize;
				}

				clipmapManager_->update(i, g_pStackTexture);

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

				/*blockIndex[1] = j / FILE_BLOCK_SIZE;
				blockCorner[1] = blockIndex[1] * FILE_BLOCK_SIZE;
				subBlockIndex[1] = (j - blockCorner[1]) / tileBlockSize;*/

				for (int k = mipCornerLU[0]; k < mipCornerRD[0]; k += tileBlockSize)
				{
					int tempBoundary = k;

					if (tempBoundary < 0)
						tempBoundary += g_ppSourceImageMipsSize[i][0];

					/*blockIndex[0] = tempBoundary / FILE_BLOCK_SIZE;
					blockCorner[0] = blockIndex[0] * FILE_BLOCK_SIZE;
					subBlockIndex[0] = (tempBoundary - blockCorner[0]) / tileBlockSize;*/

					srcBlock[0] = tempBoundary;
					srcBlock[1] = j;

					dstBlock[0] = subResourceBox.min_.x;
					dstBlock[1] = g_ClipmapStackSize - subResourceBox.max_.y;

					clipmapManager_->addBlock(i, srcBlock, dstBlock);

					subResourceBox.min_.x += tileBlockSize;
					subResourceBox.max_.x += tileBlockSize;
				}

				clipmapManager_->update(i, g_pStackTexture);

				subResourceBox.min_.y += tileBlockSize;
			}	subResourceBox.max_.y += tileBlockSize;
		}
	}
}


void ClipMappingScene::render(PassInfo&info)
{

	Shader* curShader = shaders_[0];


	const CameraBase * camera = getCamera();
	Matrixf viewmatrix = camera->getViewMatrix();

	V3f worldRight{viewmatrix[0][0],viewmatrix[1][0], viewmatrix[2][0]};
	V3f worldUp{ viewmatrix[0][1],viewmatrix[1][1], viewmatrix[2][1] };

	curShader->turnOn();
	initUniformVal(curShader);
#ifdef QUAD_TEST_STACK_TEXTURE
#else
	curShader->setFloat3V(curShader->getVariable("g_EyePosition"), 1, &camera->getPosition()[0]);
	curShader->setFloat3V(curShader->getVariable("g_WorldRight"), 1, &worldRight[0]);
	curShader->setFloat3V(curShader->getVariable("g_WorldUp"), 1, &worldUp[0]);
	curShader->setFloat3V(curShader->getVariable("g_LightPosition"), 1, &LIGHTPOS[0]);
#endif
	

	glActiveTexture(GL_TEXTURE0);
	g_pStackTexture->bind();

	sphere_.draw();
	curShader->turnOff();
	
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

