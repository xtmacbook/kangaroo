
#include <png.h>
#include <strstream>
#include <algorithm>
#include "type.h"
#include "filter.h"
#include "BaseObject.h"
#include "smartPointer.h"

using namespace std;
using namespace math;
using namespace Base;


template <typename T>
//inline const T & clamp(const T & x, const T & a, const T & b)
inline T clamp(const T & x, const T & a, const T & b)
{
	return min(max(x, a), b);
}

inline int wrapClamp(int x, int w)
{
	return clamp(x, 0, w - 1);
}

template <class T>
struct HeighImg :public BaseObject
{
	int width_;
	int height_;

	int componentCount_;

	T * data_;

	png_uint_16 colormap[256 * 4];


	HeighImg<T>* resize(const Filter & filter, uint w, uint h) ;

	void applyKernelX(const PolyphaseKernel & k, int y,  uint c, T * output);

	void applyKernelY(const PolyphaseKernel & k, int x, uint c, T * output,int output_stride);

	void allocate(int c, int  w, int h);

	uint index(int x, int y);
};

template <class T>
uint HeighImg<T>::index(int x, int y)
{
	x = wrapClamp(x, width_);
	y = wrapClamp(y, height_);

	return y * width_ + x;
}

template <class T>
void HeighImg<T>::allocate(int c, int w, int h)
{
	width_ = w;
	height_ = h;
	componentCount_ = c;

	data_ = (T*)malloc(width_ * height_ * 2 * componentCount_);
}

template <class T>
void HeighImg<T>::applyKernelY(const PolyphaseKernel & k, int x, uint c, T * output,int output_stride) 
{
	const uint length = k.length();
	const float scale = float(length) / float(height_);
	const float iscale = 1.0f / scale;

	const float width = k.width();
	const int windowSize = k.windowSize();

	for (uint i = 0; i < length; i++)
	{
		const float center = (0.5f + i) * iscale;

		const int left = (int)floorf(center - width);
		const int right = (int)ceilf(center + width);
		assert(right - left <= windowSize);

		float sum = 0;
		for (int j = 0; j < windowSize; ++j)
		{
			const int idx = this->index(x, j + left);

			sum += k.valueAt(i, j) * data_[idx * componentCount_ + c];
		}

		output[i * output_stride * componentCount_ + c] = sum;
	}
}

template <class T>
void HeighImg<T>::applyKernelX(const PolyphaseKernel & k, int y, uint c, T * output)
{
	const uint length = k.length();
	const float scale = float(length) / float(width_);
	const float iscale = 1.0f / scale;

	const float width = k.width();
	const int windowSize = k.windowSize();

	for (uint i = 0; i < length; i++)
	{
		const float center = (0.5f + i) * iscale;

		const int left = (int)floorf(center - width);
		const int right = (int)ceilf(center + width);

		assert(right - left <= windowSize);

		float sum = 0;

		for (int j = 0; j < windowSize; ++j)
		{
			const int idx = this->index(left + j, y);

			sum += k.valueAt(i, j) * data_[idx * componentCount_ + c];
		}

		output[i * componentCount_ + c] = sum;
	}
}

template <class T>
HeighImg<T>* HeighImg<T>::resize(const Filter & filter, uint w, uint h) 
{
	PolyphaseKernel xkernel(filter, width_, w, 32);
	PolyphaseKernel ykernel(filter, height_, h, 32);

	SmartPointer<HeighImg<T> > tmp_image(new HeighImg<T>());
	SmartPointer<HeighImg<T> > dst_image(new HeighImg<T>());

	for (int i = 0; i < 256 * 4; i++)
	{
		dst_image->colormap[i] = this->colormap[i];
	}

	tmp_image->allocate(componentCount_, w, height_);
	dst_image->allocate(componentCount_, w, h);

	for (uint c = 0; c < componentCount_; c++)
	{
		for (uint y = 0; y < height_; y++)
		{
			this->applyKernelX(xkernel, y, c, tmp_image->data_ + y * w * componentCount_);
		}

		for (uint x = 0; x < w; x++) 
		{
			tmp_image->applyKernelY(ykernel, x, c, dst_image->data_ + x * componentCount_ , w);

		}
	}
	
	return dst_image.release();
}

template <class T>
void downSample(HeighImg<T>* img)
{
	const uint32 w = std::max(1, img->width_ / 2);
	const uint32 h = std::max(1, img->height_ / 2);

	KaiserFilter filter(3);
	filter.setParameters(4.0, 1.0);

	SmartPointer<HeighImg<T> >dimage =  img->resize(filter, w, h);

	char * filename = "d:/temp/down.png";

	png_image image_pg;
	
	memset(&image_pg, 0, sizeof(image_pg));

	image_pg.version = PNG_IMAGE_VERSION;
	image_pg.width = dimage->width_;
	image_pg.height = dimage->height_;
	image_pg.format = PNG_FORMAT_LINEAR_Y;
	image_pg.flags = 0;
	image_pg.colormap_entries = 0;

	if (!png_image_write_to_file(&image_pg, filename, 0,
		(png_bytep)dimage->data_, (png_int_32) (dimage->width_ * 2), NULL))
	{
		printf("%s", image_pg.message);
		int a = 3;
	}
}

int main()
{

	png_image image_png;
	memset(&image_png, 0, sizeof(image_png));
	image_png.version = PNG_IMAGE_VERSION;

	//D:/temp/heightmap/ps_height_4k.png  D:/temp/height.png
	if (png_image_begin_read_from_file(&image_png, "D:/temp/heightmap/ps_height_4k.png"))
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
			int mCX, mCZ;
			mCX = image_png.width;
			mCZ = image_png.height;

			void * mData = malloc(image_png.height * image_png.width * pixel_size);
			memcpy(mData, buffer_png, image_png.height * image_png.width * pixel_size);

			png_image_free(&image_png);
			free(buffer_png);

			uint16 * data = new uint16[mCX * mCZ];

			if (image_png.format == PNG_FORMAT_LINEAR_Y)
			{
				for (unsigned int z = 0; z < mCZ; z++)
				{
					uint16 * src = (uint16*)mData + z * mCX;
					for (unsigned int x = 0; x < mCX; x++)
					{
						data[(mCZ - z - 1) * mCX + x] = (float)(src[x])/* * (0.1f / 40.0f)*/;
					}
				}
			}

			free(mData);

			HeighImg<uint16> height;
			height.width_ = mCX;
			height.height_ = mCZ;
			height.data_ = data;
			height.componentCount_ = 1;

			downSample(&height);

			return true;
		}
	}


	return 0;
}
