#include "imgLoad.h"
#include "stream.h"
#include "log.h"
#include "image.h"
#include "color.h"
#include "rgbe.h"

#include <vector>

using namespace math;

namespace IO
{
	bool ImageFile::loadHDR(const char*file, Base::SmartPointer<Base::Image>&img)
	{
		FILE *fp = fopen(file, "rb");
		if (!fp) {
			std::string err = "Error opening file:";
			err += file;
			PRINT_ERROR(err.c_str());
			return false;
		}

		rgbe_header_info header;
		int w, h;
		if (RGBE_ReadHeader(fp, &w, &h, &header))
			return false;

		img->setwidth(w);
		img->setheight(h);
		img->setdepth(4);
		img->allocate(w * h * 4);

		//  if (RGBE_ReadPixels_RLE(fp, img.data, img.width, img.height))
		if (RGBE_ReadPixels_Raw_RLE(fp, (unsigned char*)img->pixels(), w, h))
			return false;

		fclose(fp);

		return true;
	}
	
}
