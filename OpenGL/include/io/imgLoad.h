#pragma once
#ifndef __OpenGL__ImgLoad__
#define __OpenGL__ImgLoad__

#include "decl.h"
#include "stream.h"
#include "smartPointer.h"

namespace Base
{
	class Image;
	class OpenExrImage;
}

namespace IO
{
    class LIBENIGHT_EXPORT ImageFile
    {
    public:

		static bool					loadDDS(StdInputStream * stream, Base::SmartPointer<Base::Image>&);
		static bool					loadOpenEXRImage(const char *fileName, Base::SmartPointer<Base::Image>&);
		static bool					loadBMP(StdInputStream * stream, Base::SmartPointer<Base::Image>&);
		static bool					loadJPG(StdInputStream * stream, Base::SmartPointer<Base::Image>&);
		static bool					loadPNG(StdInputStream * stream, Base::SmartPointer<Base::Image>&);
		static bool					loadKtx(const char*fileName, Base::SmartPointer<Base::Image>&);
	    	static bool					loadTga(StdInputStream * stream, Base::SmartPointer<Base::Image>&);
		static bool					loadHDR(const char*fileName , Base::SmartPointer<Base::Image>&);
    };
}
#endif
