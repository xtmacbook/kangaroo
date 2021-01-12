//
//  comm.h
//  OpenGL
//
//  Created by xt on 15/7/24.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#ifndef __OpenGL__comm__
#define __OpenGL__comm__
#include "common.h"
#include "shader.h"
#include "renderNode.h"
#include "geometry.h"

#include <map>

class LIBENIGHT_EXPORT DLineMeshGeoemtry :public MeshGeometry
{
public:
	DLineMeshGeoemtry(const V3f& s, const V3f& e, const V3f& color, IRenderMeshObj_SP);
};

class LIBENIGHT_EXPORT DPointsMeshGeoemtry :public MeshGeometry
{
public:
	DPointsMeshGeoemtry(const V3f& s, const V3f& color, IRenderMeshObj_SP);
};


class  HUDGeoemtry :public MeshGeometry
{
public:
	virtual void drawGeoemtry(const DrawInfo&);

	HUDGeoemtry(float xoffset, float yoffset, float width, float height, float texCoordZ, IRenderMeshObj_SP);

private:
	float xoffset_, yoffset_;
	float width_, height_, texCoordz_;
};

class CubGeometry :public MeshGeometry
{
public:
	CubGeometry(const V3f& centerPos, float scale, IRenderMeshObj_SP rsp);
};

class SphereGeoemtry : public MeshGeometry
{
public:
	typedef Vertex_P Vertex;

	virtual void computeBoundingBox(void*);
	SphereGeoemtry(V3f pos, float scale, IRenderMeshObj_SP);

	static Shader_SP	getShader();
private:
	static Shader_SP  sphereShader_;
	V3f pos_;
	float scale_;
};

class QuadGeometry :public MeshGeometry
{
public:
	QuadGeometry(const V3f& centerPos, float scale, IRenderMeshObj_SP);
};

LIBENIGHT_EXPORT IRenderNode_SP getLineRenderNode(const V3f& s, const V3f& e, const V3f& color, bool);
LIBENIGHT_EXPORT IRenderNode_SP getPointsRenderNode(const V3f& s, const V3f& color, bool);
LIBENIGHT_EXPORT IRenderNode_SP getRayRenderNode(const V3f& s, const V3f& end, const V3f& color, bool);
LIBENIGHT_EXPORT IRenderNode_SP getHudRenderNode(float xoffset, float yoffset, float width, float height, float texCoordZ, bool);
LIBENIGHT_EXPORT IRenderNode_SP getQuadRenderNode(const V3f& center = V3f(0.0, 0.0, 0.0), float scale = 1.0, bool u = false);
LIBENIGHT_EXPORT IRenderNode_SP getSphereRenderNode(const V3f& center, float scale, bool);



#endif  
