
#include "imgLoad.h"
#include "image.h"
#include "log.h"
#include "gls.h"

#include <vector>

#ifdef ENGINE_JPEG
#include <jpeglib.h>
#endif

namespace IO {

#if defined(ENGINE_JPEG)

	//jpeg
	struct my_error_mgr 
	{
		struct jpeg_error_mgr pub;	/* "public" fields */
		jmp_buf setjmp_buffer;	/* for return to caller */
	};

	typedef struct my_error_mgr * my_error_ptr;

	METHODDEF(void) my_error_exit(j_common_ptr cinfo)
	{
		/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
		my_error_ptr myerr = (my_error_ptr)cinfo->err;

		/* Always display the message. */
		/* We could postpone this until after returning, if we chose. */
		(*cinfo->err->output_message) (cinfo);

		/* Return control to the setjmp point */
		longjmp(myerr->setjmp_buffer, 1);
	}
#endif // defined(ENGINE_JPEG)


	base::Image* ImageFile::loadJPG(StdInputStream * stream)
	{
#ifdef ENGINE_JPEG

		char * signature = "\xFF\xD8\xFF";

		/* This struct contains the JPEG decompression parameters and pointers to
		* working space (which is allocated as needed by the JPEG library).
		*/
		jpeg_decompress_struct cinfo;
		my_error_mgr jerr;

		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = my_error_exit;

		/* Establish the setjmp return context for my_error_exit to use. */
		if (setjmp(jerr.setjmp_buffer))
		{
			/* If we get here, the JPEG code has signaled an error.
			* We need to clean up the JPEG object, close the input file, and return.
			*/
			jpeg_destroy_decompress(&cinfo);
			return nullptr;
		}

		/* Now we can initialize the JPEG decompression object. */
		jpeg_create_decompress(&cinfo);

		std::vector<uint8> byte_array;
		byte_array.resize(stream->size());
		stream->serializeData(byte_array.data(), stream->size());

		/* Step 2: specify data source (eg, a file) */
		jpeg_mem_src(&cinfo, &byte_array[0], stream->size());
		/* Step 3: read file parameters with jpeg_read_header() */
		jpeg_read_header(&cinfo, TRUE);


		if (cinfo.ac_huff_tbl_ptrs[0] == NULL &&
			cinfo.ac_huff_tbl_ptrs[1] == NULL &&
			cinfo.dc_huff_tbl_ptrs[0] == NULL &&
			cinfo.dc_huff_tbl_ptrs[1] == NULL)
		{
			LOGE("load jpeg : fail to parse this color because the load dht (opencv) !\n");
			jpeg_destroy_decompress(&cinfo);
			return nullptr;
		}

		/* Step 4: set parameters for decompression */

		/* In this example, we don't need to change any of the defaults set by
		* jpeg_read_header(), so we do nothing here.
		*/

		/* Step 5: Start decompressor */
		jpeg_start_decompress(&cinfo);
		

		J_COLOR_SPACE cs = cinfo.out_color_space;
		int width =  cinfo.output_width;
		int height = cinfo.output_height;
		int channels = cinfo.output_components;

		base::Image* img = new base::Image();
		img->allocate(width * height * channels);

		img->levelDataPtr_ = new uint8*[1];
		img->levelDataPtr_[0] = (uint8*)img->pixels();

		img->setwidth(width);
		img->setheight(height);
		img->settarget(GL_TEXTURE_2D);
		img->settype(GL_UNSIGNED_BYTE);
		img->setComponents(channels);
		img->setnumOfMiplevels(1);
		switch (cs)
		{
		case JCS_GRAYSCALE:
			img->setformat(GL_RED);
			img->setinternalformat(GL_R8);
			break;
		case JCS_RGB:
			img->setformat(GL_RGB);
			img->setinternalformat(GL_RGB8);
			break;
		default:
			LOGE("load jpeg : fail to parse this color space type !\n");
			return img;
			break;
		}

		img->settypesize(0);
		img->setfaces(1);
		img->setElementSize(channels);
		img->setdepth(1);

		/* copy data for openGL */
		int row_stride = cinfo.output_width * cinfo.output_components;

		uint8 * data = (uint8*) img->pixels();

		JSAMPROW rowptr[1];

		while (cinfo.output_scanline < cinfo.output_height)
		{
			/* jpeg_read_scanlines expects an array of pointers to scanlines.
			* Here the array is only one element long, but you could ask for
			* more than one scanline at a time if that's more convenient.
			*/
			rowptr[0] = data + (cinfo.output_height - cinfo.output_scanline - 1) * row_stride;//for opengl

			jpeg_read_scanlines(&cinfo, rowptr, 1);
		}

		jpeg_finish_decompress(&cinfo);

		jpeg_destroy_decompress(&cinfo);


		return img;
#else
		Log::instance()->printMessage("have no the jpeg lib ....\n");
		return false;
#endif // ENGINE_JPEG

	}
}
