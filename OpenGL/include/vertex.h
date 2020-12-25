
#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "decl.h"
#include "type.h"

using namespace math;
using namespace base;

struct LIBENIGHT_EXPORT Vertex
{
	V3f Position;
	// normal
	V3f Normal;
	// texCoords
	V2f TexCoords;
	// tangent
	V3f Tangent;
	// bitangent
	V3f Bitangent;
};

struct LIBENIGHT_EXPORT Vertex_P
{
	V3f Position;
};

struct LIBENIGHT_EXPORT Vertex_PT :public Vertex_P
{
	V2f TexCoords;
};
#endif
