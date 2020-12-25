
#include "imgLoad.h"
#include "image.h"
#include "gls.h"
#include "log.h"
#include "engineLoad.h"

#ifdef ENGINE_PNG
#include <png.h>
#endif
#include <vector>
#include "log.h"
namespace IO
{
#if defined(ENGINE_PNG)
	static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		StdInputStream * sis;
		sis = static_cast<StdInputStream*>(png_get_io_ptr(png_ptr));
		if (sis == NULL)
			return;   // add custom error handling here
		if (!sis->isLoading()) return;
		uint rd = sis->serializeData(data, (int)length);
		if (sis->isError())
		{
			png_error(png_ptr, "Read png Error");
		}

	}
#endif

	base::Image* ImageFile::loadPNG(StdInputStream * stream, const ImageOption*option)
	{
#if 0
		png_image image_png;
		memset(&image_png, 0, sizeof(image_png));
		image_png.version = PNG_IMAGE_VERSION;
		image_png.opaque = nullptr;

		std::vector<int8> byte_array;
		byte_array.resize(stream->size());
		stream->serializeData(byte_array.data(), stream->size());

		if (!png_image_begin_read_from_memory(&image_png, (png_const_voidp)byte_array.data(), stream->size()))
			return nullptr;

		size_t size = PNG_IMAGE_SIZE(image_png); // get image size

		base::Image * image = new base::Image;
		image->allocate(size);
		image->levelDataPtr_ = new uint8*[1];
		image->levelDataPtr_[0] = (uint8*)image->pixels();

		if (png_image_finish_read(&image_png, NULL/*background*/, image->pixels(), 0/*row_stride*/, nullptr/*colormap*/))
		{
			size_t pixel_size = 0;

			image->setwidth(image_png.width);
			image->setheight(image_png.height);

			// check file format
			switch (image_png.format)
			{
			case PNG_FORMAT_GRAY:
				pixel_size = 1;
				image->setComponents(1);
				break;
			case PNG_FORMAT_LINEAR_Y:
				pixel_size = 2;
				image->setComponents(1);
				image->settarget(GL_TEXTURE_2D);
				if (!option->clamped_)
					image->setinternalformat(GL_R16F);
				else
					image->setinternalformat(GL_R16);
				image->settype(GL_SHORT);
				image->setformat(GL_RED);
				break;
			case PNG_FORMAT_RGB:
				pixel_size = 3;
				image->setComponents(3);
				image->settarget(GL_TEXTURE_2D);
				image->setinternalformat(GL_RGB8);
				image->settype(GL_UNSIGNED_BYTE);
				image->setformat(GL_RGB);
				break;
			case PNG_FORMAT_RGBA:
				pixel_size = 4;
				image->setComponents(4);
				image->settarget(GL_TEXTURE_2D);
				image->setinternalformat(GL_RGBA8);
				image->settype(GL_UNSIGNED_BYTE);
				image->setformat(GL_RGBA);
				break;
			default:
			{
				printf("undefine png format!\n");
				delete image;
				return nullptr;
			}
			}

			image->settypesize(0);
			image->setfaces(1);
			image->setElementSize(pixel_size);
			png_image_free(&image_png);
			image->setdepth(1);

			return image;

		}
		return nullptr;
#else
		png_byte sig[8];
		int bit_depth, color_type;
		double              gamma;
		png_uint_32 channels, row_bytes, w, h;
		png_structp png_ptr = 0;
		png_infop info_ptr = 0;

		stream->serializeData(sig, 8);
		if (!png_check_sig(sig, 8))
		{
			LOGE("png png_check_sig error !\n");
			return nullptr;
		}
		/* Create and initialize the png_struct
		* with the desired error handler
		* functions.  If you want to use the
		* default stderr and longjump method,
		* you can supply NULL for the last
		* three parameters.  We also supply the
		* the compiler header file version, so
		* that we know if the application
		* was compiled with a compatible version
		* of the library.  REQUIRED
		*/
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
		if (!png_ptr)
		{
			LOGE("png create read struct error !\n");
			return nullptr;
		}

		/*
			* Allocate/initialize the memory
			* for image information.  REQUIRED.
		*/
		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			png_destroy_read_struct(&png_ptr, 0, 0);
			LOGE("png png_destroy_read_struct error !\n");
			return nullptr;
		}

		/* Set error handling if you are
		  using the setjmp / longjmp method
		  (this is the normal method of
			* doing things with libpng).
		  REQUIRED unless you  set up
		  your own error handlers in
		  the png_create_read_struct()
		  earlier */

		if (setjmp(png_jmpbuf(png_ptr)))
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			LOGE("png setjmp error !\n");
			return nullptr;
		}

		/*    Set new custom read function    */
		png_set_read_fn(png_ptr, (png_voidp)stream, (png_rw_ptr)user_read_data);

		png_set_sig_bytes(png_ptr, 8);

		/*     Read and decode PNG stream   */
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

		// get again width, height and the new bit-depth and color-type
		png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, 0, 0, 0);

		row_bytes = png_get_rowbytes(png_ptr, info_ptr);
		channels = png_get_channels(png_ptr, info_ptr);

		int byte_formats = bit_depth / 8;
		
		base::Image * image = new base::Image;
		image->setwidth(w);
		image->setheight(h);
		image->settarget(GL_TEXTURE_2D);
		image->allocate(row_bytes * h);
		image->levelDataPtr_ = new uint8*[1];
		image->levelDataPtr_[0] = (uint8*)image->pixels();
		image->setComponents(channels);

		if(byte_formats == 1) image->settype(GL_UNSIGNED_BYTE);
		else if (byte_formats == 2) image->settype(GL_UNSIGNED_SHORT);
		else
		{
			LOGE("png load byte formate is error!!\n");
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return nullptr;
		}

		switch (color_type)
		{
		case PNG_COLOR_TYPE_GRAY:
			if (byte_formats == 1)
				image->setinternalformat(GL_R8);
			else if (byte_formats == 2)
				image->setinternalformat(GL_R16);
			else
			{
				LOGE("png load PNG_COLOR_TYPE_GRAY is error!!\n");
				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
				return nullptr;
			}
			image->setformat(GL_RED);

			break;
		case PNG_COLOR_TYPE_RGB:
			if (byte_formats == 1)
				image->setinternalformat(GL_RGB8);
			else if (byte_formats == 2)
				image->setinternalformat(GL_RGB16);
			else
			{
				LOGE("png load PNG_COLOR_TYPE_RGB is error!!\n");
				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
				return nullptr;
			}
			image->setformat(GL_RGB);
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			if (byte_formats == 1)
				image->setinternalformat(GL_RGBA8);
			else if(byte_formats == 2)
				image->setinternalformat(GL_RGBA16);
			else
			{
				LOGE("png load PNG_COLOR_TYPE_RGB_ALPHA is error!!\n");
				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
				return nullptr;
			}
			image->setformat(GL_RGBA);
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			LOGE("png load PNG_COLOR_TYPE_GRAY_ALPHA is error!!\n");
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return nullptr;
		default:
			LOGE("png load no color_type exist!!\n");
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return nullptr;
		}
		

		image->settypesize(0);
		image->setfaces(1);
		image->setElementSize(byte_formats * channels);
		image->setdepth(1);

		png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
		for (int i = 0; i < h; i++)
		{
			// note that png is ordered top to
			// bottom, but OpenGL expect it bottom to top
			// so the order or swapped
			if (byte_formats == 1) memcpy((uint8*)image->pixels() + (row_bytes * (h - 1 - i)), row_pointers[i], row_bytes);
			else memcpy((uint16*)image->pixels() + (row_bytes * (h - 1 - i)), row_pointers[i], row_bytes);
		}

		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
#endif
		return image;
	}

}
