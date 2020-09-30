
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <png.h>
#include "texture.h"
#include "gls.h"
#include "log.h"
#include "terrainUpdater.h"

 
TerrainUpdater::TerrainUpdater( int w, int h):
	RasterUpdater(w,h){

}

TerrainUpdater::~TerrainUpdater()
{
	
}

bool TerrainUpdater::loadTerrainFile(const char*heightFile)
{
	unsigned int				mCX;
	unsigned int				mCZ;
	void *						mData;

	png_image image_png;
	memset(&image_png, 0, sizeof(image_png));
	image_png.version = PNG_IMAGE_VERSION;

	if (png_image_begin_read_from_file(&image_png, heightFile))
	{
		size_t pixel_size = 0;

		// check file format
		switch (image_png.format)
		{
		case PNG_FORMAT_GRAY:
			pixel_size = 1;
			break;
		case PNG_FORMAT_LINEAR_Y:
			pixel_size = 2;
			break;
		case PNG_FORMAT_RGB:
			pixel_size = 3;
			break;
		case PNG_FORMAT_RGBA:
			pixel_size = 4;
			break;
		default:
			return false;
		}

		size_t size = PNG_IMAGE_SIZE(image_png); // get image size
		png_bytep buffer_png = (png_bytep)malloc(size);

		if (png_image_finish_read(&image_png, NULL/*background*/, buffer_png, 0/*row_stride*/, nullptr/*colormap*/))
		{

			mCX = image_png.width;
			mCZ = image_png.height;

			mData = malloc(image_png.height * image_png.width * pixel_size);
			memcpy(mData, buffer_png, image_png.height * image_png.width * pixel_size);

			png_image_free(&image_png);
			free(buffer_png);

			heightMap_.data_ = new float[mCX * mCZ];
			heightMap_.width_ = mCX;
			heightMap_.height_ = mCZ;

			if (image_png.format == PNG_FORMAT_LINEAR_Y)
			{
				for (unsigned int z = 0; z < mCZ; z++)
				{
					unsigned short * src = (unsigned short*)mData + z * mCX;
					for (unsigned int x = 0; x < mCX; x++)
					{
						((float*)heightMap_.data_)[(mCZ - z - 1) * mCX + x] = (float)(src[x]) * (0.1f / 40.0f);
					}
				}
			}

			free(mData);
		}

	}
	return false;
}

void TerrainUpdater::initTextureObj()
{
	heightMap_.heightTextureObj_ = new Texture();
	heightMap_.heightTextureObj_->height_ = heightMap_.height_;
	heightMap_.heightTextureObj_->width_ = heightMap_.width_;

	heightMap_.heightTextureObj_->internalformat_ = GL_R32F;
	heightMap_.heightTextureObj_->format_ = GL_RED;

	heightMap_.heightTextureObj_->target_ = GL_TEXTURE_2D;
	heightMap_.heightTextureObj_->type_ = GL_FLOAT;
	{
		heightMap_.heightTextureObj_->createObj();
		heightMap_.heightTextureObj_->bind();
		heightMap_.heightTextureObj_->mirrorRepeat();
		heightMap_.heightTextureObj_->filterLinear();
		V4f borderColor(0.0);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (GLfloat *)&borderColor);
		if (!heightMap_.heightTextureObj_->context(heightMap_.data_))
		{
		}
		CHECK_GL_ERROR;
	}

}

void TerrainUpdater::bindHeightMap(bool flag)
{
	(flag) ? heightMap_.heightTextureObj_->bind() : heightMap_.heightTextureObj_->unBind();
}

bool TerrainUpdater::initDiffMap(const char* file)
{
	diffuseTexture_ = new Texture(file);
	diffuseTexture_->target_ = GL_TEXTURE_2D;
	if (diffuseTexture_->loadData())
	{
		diffuseTexture_->createObj();
		diffuseTexture_->bind();
		diffuseTexture_->mirrorRepeat();
		diffuseTexture_->filterLinear();
		if (!diffuseTexture_->context(NULL))
		{
			return false;
		}
		CHECK_GL_ERROR;
	}
}

