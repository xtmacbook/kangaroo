#pragma once
#ifndef __OpenGL__EngineLoad__
#define __OpenGL__EngineLoad__

#include "decl.h"
#include "IRenderNode.h"
#include "texture.h"
#include <vector>
#include "mesh.h"


namespace IO
{
	struct LModelInfo
	{
		std::vector<Texture_Sp> textures_;
		std::map<unsigned int, std::vector<Mesh_SP> > mapMeshs;
	};

	struct ImageOption
	{
		bool clamped_ = false;
		bool flip_ = false;
	};

	class LIBENIGHT_EXPORT EngineLoad
	{
	public:
		enum ModelLoadType
		{
			ML_ASSIMP,
			ML_NONEPLUG
		};
		static bool loadNode(const char*file, LModelInfo&, ModelLoadType = ML_ASSIMP);
		static base::SmartPointer<base::Image> loadImage(const char*file, const ImageOption&option = {true,false});

	};

};

#endif
