#include "mesh.h"
#include "engineLoad.h"
#include "meshLoad.h"
#include "stream.h"
#include "imgLoad.h"
#include "resource.h"
#include "common.h"
using namespace  IO;

bool EngineLoad::loadNode(const char* file, LModelInfo&data, ModelLoadType  t/*= ML_ASSIMP*/)
{
	bool result = false;
	switch (t)
	{
	case EngineLoad::ML_ASSIMP:
		result = NodeFile::assimpLoadModel(file, data);
		break;
	case EngineLoad::ML_NONEPLUG:
		break;
	default:
		break;
	}

	return result;
}

base::SmartPointer<base::Image> EngineLoad::loadImage(const char* file)
{
	const char * ext = extension(file);

	StdInputStream stream(file);
	if (stream.isError())
	{
		std::string filename = get_texture_BasePath() + file;
		FILE * f = fileOpen(filename.c_str(), "rb");
		if (f == NULL) return NULL;
		stream.setFile(f);
	}

	base::SmartPointer<base::Image> img;

	bool result = false;
#if defined(ENGINE_JPEG)
	if (strCaseDiff(ext, ".jpg") == 0 || strCaseDiff(ext, ".jpeg") == 0)
	{
		img = new U8Image();
		result = ImageFile::loadJPG(&stream, img);
	}
#endif
	if (strCaseDiff(ext, ".bmp") == 0)
	{
		img = new U8Image();
		result = ImageFile::loadBMP(&stream, img);
	}
#if defined(ENGINE_PNG)
	if (strCaseDiff(ext, ".png") == 0)
	{
		img = new U8Image();
		result = ImageFile::loadPNG(&stream, img);
	}
#endif
	if (strCaseDiff(ext, ".ktx") == 0)
	{
		img = new U8Image();
		result = ImageFile::loadKtx(file, img);
	}
#if defined(ENGINE_OPENEXR)
	if (strCaseDiff(ext, ".hdr") == 0)
	{
		img = new OpenExrImage();
		result = ImageFile::loadHDR(file, img);
	}
#endif
	if (strCaseDiff(ext, ".tga") == 0)
	{
		img = new U8Image();
		result = ImageFile::loadTga(&stream, img);
	}
		if (strCaseDiff(ext, ".dds") == 0)
	{
		img = new DDSImage();
		result = ImageFile::loadDDS(&stream, img);
	}

	return img;
}
