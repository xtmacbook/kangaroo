
#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "decl.h"
#include "type.h"
#include "util.h"
#include "BaseObject.h"
#include "smartPointer.h"
using namespace math;
using namespace base;

//struct LIBENIGHT_EXPORT Vertex
//{
//	V3f Position;
//	// normal
//	V3f Normal;
//	// texCoords
//	V2f TexCoords;
//	// tangent
//	V3f Tangent;
//	// bitangent
//	V3f Bitangent;
//};

enum VERTEX_TYPE
{
	VERTEX_POINTS = FVF_XYZ,
	VERTEX_POINTS_TEXTURE = FVF_XYZ | FVF_TEXT0,
	VERTEX_POINTS_COLOR  = FVF_XYZ | FVF_COLOR,
	VERTEX_POINTS_NORMAL = FVF_XYZ | FVF_NORMAL,
	VERTEX_POINTS_TBN = FVF_XYZ | FVF_TANGENT | FVF_BITANGENT,
	VERTEX_POINTS_NORMAL_TEXTURE = FVF_XYZ | FVF_NORMAL | FVF_TEXT0,
	VERTEX_POINTS_NORMAL_TEXTURE_TBN = FVF_XYZ | FVF_NORMAL | FVF_TEXT0 | FVF_TANGENT | FVF_BITANGENT
};

struct LIBENIGHT_EXPORT Vertex_P
{
	Vertex_P();
	Vertex_P(const V3f&p);
	V3f position_;
};

struct LIBENIGHT_EXPORT Vertex_PT :public Vertex_P
{
	Vertex_PT();
	Vertex_PT(const V3f&p, const V2f&t);
	V2f texCoords_;
};

struct LIBENIGHT_EXPORT Vertex_PC :public Vertex_P
{
	Vertex_PC();
	Vertex_PC(const V3f&p, const V3f&c);
	V3f color_;
};

struct LIBENIGHT_EXPORT Vertex_PN :public Vertex_P
{
	Vertex_PN();
	Vertex_PN(const V3f&p, const V3f&n);
	V3f normal_;
};

struct LIBENIGHT_EXPORT Vertex_PTBN :public Vertex_P
{
	Vertex_PTBN();
	Vertex_PTBN(const V3f&p, const V3f&t, const V3f&b);
	V3f tangent_;
	V3f bitangent_;
};

struct LIBENIGHT_EXPORT Vertex_PNT :public Vertex_PN
{
	Vertex_PNT();
	Vertex_PNT(const V3f&p, const V3f&n,const V2f&t);
	V2f texCoords_;
};

struct LIBENIGHT_EXPORT Vertex_PNTTBN :public Vertex_PNT
{
	Vertex_PNTTBN();
	Vertex_PNTTBN(const V3f&p, const V3f&n, const V2f&t,const V3f& tangent,const V3f&bitangent);
	V3f tangent_;
	V3f bitangent_;
};

typedef base::SmartPointer<Vertex_P>	Vertex_SP;

#endif
