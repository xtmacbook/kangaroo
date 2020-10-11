#pragma once
#ifndef __OpenGL__ImgLoad__
#define __OpenGL__ImgLoad__

#include "decl.h"
#include "stream.h"
#include "smartPointer.h"

namespace base
{
	class Image;
	class OpenExrImage;
}

namespace IO
{
    class LIBENIGHT_EXPORT ImageFile
    {
    public:

		static bool					loadDDS(StdInputStream * stream, base::SmartPointer<base::Image>&);
		static bool					loadOpenEXRImage(const char *fileName, base::SmartPointer<base::Image>&);
		static bool					loadBMP(StdInputStream * stream, base::SmartPointer<base::Image>&);
		static bool					loadJPG(StdInputStream * stream, base::SmartPointer<base::Image>&);
		static bool					loadPNG(StdInputStream * stream, base::SmartPointer<base::Image>&);
		static bool					loadKtx(const char*fileName, base::SmartPointer<base::Image>&);
	    static bool					loadTga(StdInputStream * stream, base::SmartPointer<base::Image>&);
		static bool					loadHDR(const char*fileName , base::SmartPointer<base::Image>&);
    };
}
#endif
