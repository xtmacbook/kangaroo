//
//  comm.cpp
//  OpenGL
//
//  Created by xt on 15/7/24.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#include "baseMesh.h"
#include "gls.h"
#include "helpF.h"
#include "shader.h"
#include "log.h"

#define  SPHERE_MERIDIAN_SLICES_NUM  128
#define  SPHERE_PARALLEL_SLICES_NUM  128

static GLfloat quadVertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f, 0.0, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0, 0.0f, 0.0f,
	1.0f, -1.0f, 0.0, 1.0f, 0.0f,

	-1.0f,  1.0f, 0.0, 0.0f, 1.0f,
	1.0f, -1.0f, 0.0, 1.0f, 0.0f,
	1.0f,  1.0f, 0.0, 1.0f, 1.0f
};

static GLfloat cubVertices[] = {
	// Back face
	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Bottom-left
	0.5f, 0.5f, -0.5f,  -1.0f, 1.0f, 1.0f,  0.0f, 0.0f,  // top-right
	0.5f, -0.5f, -0.5f, -1.0f, 1.0f, 0.0f,  0.0f, 0.0f, // bottom-right
	0.5f, 0.5f, -0.5f,  -1.0f, 1.0f, 1.0f,  0.0f, 0.0f,   // top-right
	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
	-0.5f, 0.5f, -0.5f,  -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // top-left
													  // Front face
	-0.5f, -0.5f, 0.5f,1.0f, 0.0f, 0.0f,0.0f, 0.0f,  // bottom-left
	0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f,0.0f, 0.0f,    // bottom-right
	0.5f, 0.5f, 0.5f,  1.0f, 1.0f, 1.0f,0.0f, 0.0f,     // top-right
	0.5f, 0.5f, 0.5f,  1.0f, 1.0f, 1.0f,0.0f, 0.0f,    // top-right
	-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,0.0f, 0.0f,    // top-left
	-0.5f, -0.5f, 0.5f,1.0f, 0.0f, 0.0f,0.0f, 0.0f,   // bottom-left
																										 // Left face
	 -0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.0f,-1.0f, 0.0f,    // top-right
	 -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 1.0f,-1.0f, 0.0f,   // top-left
	 -0.5f, -0.5f, -0.5f,0.0f, 0.0f, 1.0f, -1.0f, 0.0f,   // bottom-left
	 -0.5f, -0.5f, -0.5f,0.0f, 0.0f, 1.0f, -1.0f, 0.0f,  // bottom-left
	 -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 0.0f,    // bottom-right
	 -0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.0f,-1.0f, 0.0f,    // top-right
																																						   // Right face
	   0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.0f,1.0f, 0.0f,    // top-left
	   0.5f, -0.5f, -0.5f,0.0f, 0.0f, 1.0f,1.0f, 0.0f,  // bottom-right
	   0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 1.0f,1.0f, 0.0f,   // top-right
	   0.5f, -0.5f, -0.5f,0.0f, 0.0f, 1.0f,1.0f, 0.0f,   // bottom-right
	   0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.0f,1.0f, 0.0f,     // top-left
	   0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f,1.0f, 0.0f,   // bottom-left
		// Bottom face
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,0.0f, -1.0f, // top-right
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,0.0f, -1.0f,  // top-left
		0.5f, -0.5f, 0.5f,   0.0f, 1.0f, 0.0f,0.0f, -1.0f,  // bottom-left
		0.5f, -0.5f, 0.5f,   0.0f, 1.0f, 0.0f,0.0f, -1.0f,   // bottom-left
		-0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.0f,0.0f, -1.0f,  // bottom-right
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,0.0f, -1.0f, // top-right
		// Top face
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,// top-left
		0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
		0.5f, 0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 1.0f,  // top-right
		0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,// top-left
																																							-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left
};
Mesh_SP createArrow(const V3f& color)
{
	Vertex_PN origin;
	origin.position_ = V3f((0.0, 0.0, 0.0));
	origin.normal_ = color;

	unsigned int sample = 36;
	float detadegree = 360.0 / sample;

	Mesh_SP ms = new Mesh(VERTEX_POINTS_NORMAL);
	ms->createMesh(sample + 2);
	ms->addVertex(&origin);

	for (int i = 0; i <= sample; i++)
	{
		float radian = math::degreeToRadin(i * detadegree);

		Vertex_PN vp;
		vp.position_.z = math::sinR(radian);
		vp.position_.y = math::cosR(radian);
		vp.position_.x = -2.0f;
		vp.normal_ = color;
		ms->addVertex(&vp);
	}

	ms->computeBox();

	return ms;
}

IRenderNode_SP getLineRenderNode(const V3f& s, const V3f& e, const V3f& color, bool update)
{
	RenderNode_SP rn = new RenderNode;
	IRenderMeshObj_SP meshObj = new ArraysRenderMeshObj;
	base::SmartPointer<DLineMeshGeoemtry> mg = new DLineMeshGeoemtry(s, e, color, meshObj);
	mg->initGeometry();
	if (rn) rn->setGeometry(mg);
	return rn;
}


LIBENIGHT_EXPORT IRenderNode_SP getPointsRenderNode(const V3f& s, const V3f& color, bool update)
{
	RenderNode_SP rn = new RenderNode;
	IRenderMeshObj_SP meshObj = new ArraysRenderMeshObj;
	meshObj->model(GL_TRIANGLE_FAN);
	SmartPointer<DPointsMeshGeoemtry> mg = new DPointsMeshGeoemtry(s, color, meshObj);
	mg->initGeometry();
	if (rn) rn->setGeometry(mg);
	return rn;
}


LIBENIGHT_EXPORT IRenderNode_SP getRayRenderNode(const V3f& s, const V3f& e, const V3f& color, bool update)
{
	Mesh_SP mesh = createArrow(V3f(1.0, 0.0, 0.0));
	Matrixf tran = math::translateR(Matrixf(1.0), e);
	V3f ray = e - s;
	math::normalizeVec3(ray);
	Matrixf rot = math::mfq(math::qftv(V3f(1.0, 0.0, 0.0), ray));

	IRenderMeshObj_SP meshObj = new ArraysRenderMeshObj;
	meshObj->model(GL_TRIANGLE_FAN);
	SmartPointer<DLineMeshGeoemtry> mg = new DLineMeshGeoemtry(s, e, color, meshObj);
	mg->updateModelMatix(tran * rot * math::scaleR(Matrixf(1.0f), V3f(0.05, 0.05, 0.05)));
	mg->addMesh(mesh);
	mg->initGeometry();

	RenderNode_SP rn = new RenderNode;

	if (rn) rn->setGeometry(mg);
	return rn;
}

IRenderNode_SP getHudRenderNode(float xoffset, float yoffset, float width, float height, float texCoordZ, bool update)
{
	RenderNode_SP rn = new RenderNode;
	IRenderMeshObj_SP meshObj = new ArraysRenderMeshObj;
	SmartPointer<HUDGeoemtry> mg = new HUDGeoemtry(xoffset, yoffset, width, height, texCoordZ, meshObj);
	mg->initGeometry();
	if (rn) rn->setGeometry(mg);
	return rn;
}


LIBENIGHT_EXPORT IRenderNode_SP getQuadRenderNode(const V3f& center, float scale, bool update)
{
	RenderNode_SP rn = new RenderNode;
	IRenderMeshObj_SP meshObj = new ArraysRenderMeshObj;
	meshObj->model(GL_TRIANGLES);
	SmartPointer<QuadGeometry> mg = new QuadGeometry(center, scale, meshObj);
	mg->initGeometry();
	if (rn) rn->setGeometry(mg);
	return rn;
}


LIBENIGHT_EXPORT IRenderNode_SP getSphereRenderNode(const V3f& center, float scale, bool)
{
	RenderNode_SP rn = new RenderNode;
	SmartPointer<ElementsRenderMeshObj> meshObj = new ElementsRenderMeshObj;
	meshObj->call(DRAW_ELEMENTS);
	meshObj->model(GL_TRIANGLES);
	meshObj->type_ = GL_UNSIGNED_INT;
	SmartPointer<SphereGeoemtry> mg = new SphereGeoemtry(center, scale, meshObj);
	mg->initGeometry();
	if (rn) rn->setGeometry(mg);
	return rn;
}


LIBENIGHT_EXPORT IRenderNode_SP getCubRenderNode(const V3f& center, float scale, bool)
{
	RenderNode_SP rn = new RenderNode;
	IRenderMeshObj_SP meshObj = new ArraysRenderMeshObj;
	meshObj->model(GL_TRIANGLES);
	SmartPointer<CubGeometry> mg = new CubGeometry(center, scale, meshObj);
	mg->initGeometry();
	if (rn) rn->setGeometry(mg);
	return rn;
}

DLineMeshGeoemtry::DLineMeshGeoemtry(const V3f& s, const V3f& e, const V3f& color, IRenderMeshObj_SP rsp) :MeshGeometry(rsp, VERTEX_POINTS_NORMAL)
{
	Vertex_PN start, end;
	start.position_ = s;
	start.normal_ = color;
	end.position_ = e;
	end.normal_ = color;

	Mesh_SP ms = new Mesh(VERTEX_POINTS_NORMAL);
	ms->createMesh(2);
	ms->addVertex(&start);
	ms->addVertex(&end);
	addMesh(ms);
}


DPointsMeshGeoemtry::DPointsMeshGeoemtry(const V3f& s, const V3f& color, IRenderMeshObj_SP rsp) : MeshGeometry(rsp, VERTEX_POINTS_COLOR)
{
	Vertex_PC start;
	start.position_ = s;
	start.color_ = color;

	Mesh_SP ms = new Mesh(VERTEX_POINTS_COLOR);
	ms->createMesh(1);
	ms->addVertex(&start);
	addMesh(ms);
}

void HUDGeoemtry::drawGeoemtry(const DrawInfo&info)
{
	Shader* hudShader = info.draw_shader_;

	hudShader->setFloat(hudShader->getVariable("sizeY"), height_);
	hudShader->setFloat(hudShader->getVariable("offsetX"), xoffset_);
	hudShader->setFloat(hudShader->getVariable("offsetY"), yoffset_);
	hudShader->setFloat(hudShader->getVariable("sizeX"), width_);
	if (hudShader->getVariable("stackZ"))
		hudShader->setFloat(hudShader->getVariable("stackZ"), texCoordz_);

	MeshGeometry::drawGeoemtry(info);
}

HUDGeoemtry::HUDGeoemtry(float xoffset, float yoffset, float width, float height, float texCoordZ, IRenderMeshObj_SP rsp) :
	MeshGeometry(rsp, VERTEX_POINTS_TEXTURE), xoffset_(xoffset),
	yoffset_(yoffset), width_(width), height_(height), texCoordz_(texCoordZ)
{
	Mesh_SP  ms = new Mesh(VERTEX_POINTS_TEXTURE);
	Vertex_PT point;
	ms->createMesh(1);
	ms->addVertex(&point);
	addMesh(ms);
}

void SphereGeoemtry::computeBoundingBox(void *)
{
	V3f min(-1, -1, -1);
	V3f max(1, 1, 1);

	box_ = BoundingBox(min * scale_ + pos_, max * scale_ + pos_);
}

SphereGeoemtry::SphereGeoemtry(V3f pos, float scale, IRenderMeshObj_SP rsp) :MeshGeometry(rsp, VERTEX_POINTS),pos_(pos),scale_(scale)
{
	int IndexNumber = SPHERE_MERIDIAN_SLICES_NUM * SPHERE_PARALLEL_SLICES_NUM * 6;

	unsigned *pIndices = new unsigned[IndexNumber];
	unsigned indexCount = 0;

	for (int i = 0; i < SPHERE_PARALLEL_SLICES_NUM; i++)
	{
		for (int j = 0; j < SPHERE_MERIDIAN_SLICES_NUM; j++)
		{
			pIndices[indexCount] = i * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j;
			indexCount++;

			pIndices[indexCount] = (i + 1) * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j + 1;
			indexCount++;

			pIndices[indexCount] = (i + 1) * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j;
			indexCount++;

			pIndices[indexCount] = i * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j;
			indexCount++;

			pIndices[indexCount] = i * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j + 1;
			indexCount++;

			pIndices[indexCount] = (i + 1) * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j + 1;
			indexCount++;

		}
	}

	Mesh_SP  ms = new Mesh(VERTEX_POINTS);
	for (int i = 0; i < IndexNumber; i++)
		ms->addIndices(pIndices[i]);
	addMesh(ms);

	SAFTE_DELETE_ARRAY(pIndices);
}

QuadGeometry::QuadGeometry(const V3f& centerPos, float scale, IRenderMeshObj_SP rsp)
	:MeshGeometry(rsp, VERTEX_POINTS_TEXTURE)
{
	SmartPointer<ArraysRenderMeshObj> armeshObj = new ArraysRenderMeshObj;
	armeshObj->model(GL_TRIANGLES);
	armeshObj->call(DRAW_ARRAYS);

	Mesh_SP mesh = new Mesh(VERTEX_POINTS_TEXTURE);
	mesh->createMesh(6);

	for (int i = 0; i < 6; i++)
	{
		Vertex_PT vertex;
		vertex.position_ = V3f(quadVertices[i * 5 + 0], quadVertices[i * 5 + 1], quadVertices[i * 5 + 2]) * scale + centerPos;
		vertex.texCoords_ = V2f(quadVertices[i * 5 + 3], quadVertices[i * 5 + 4]);
		mesh->addVertex(&vertex);
	}
	addMesh(mesh);
}

CubGeometry::CubGeometry(const V3f& centerPos, float scale, IRenderMeshObj_SP rsp) :MeshGeometry(rsp, VERTEX_POINTS_NORMAL_TEXTURE)
{
	SmartPointer<ArraysRenderMeshObj> armeshObj = new ArraysRenderMeshObj;
	armeshObj->model(GL_TRIANGLES);
	armeshObj->call(DRAW_ARRAYS);

	Mesh_SP mesh = new Mesh(VERTEX_POINTS_NORMAL_TEXTURE);
	mesh->createMesh(36);

	for (int i = 0; i < 36; i++)
	{
		Vertex_PNT vertex;
		vertex.position_ = V3f(cubVertices[i * 8 + 0], cubVertices[i * 8 + 1], cubVertices[i * 8 + 2]) * scale + centerPos;
		vertex.normal_ = V3f(cubVertices[i * 8 + 3], cubVertices[i * 8 + 4], cubVertices[i * 8 + 5]) ;
		vertex.texCoords_ = V2f(cubVertices[i * 8 + 6], cubVertices[i * 8 + 7]);
		mesh->addVertex(&vertex);
	}
	addMesh(mesh);
}
