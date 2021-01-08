//
//  baseGeometry.h
//  openGLTest
//
//  Created by xt on 18/6/10.
//  Copyright © 2018年 xt. All rights reserved.
//

#ifndef baseGeometry_h
#define baseGeometry_h

#include "BaseObject.h"
#include "SmartPointer.h"
#include "boundingBox.h"
#include "mesh.h"
#include "common.h"
#include "renderMesh.h"
#include "gls.h"
#include <string>
#include <vector>


class Shader;
class CameraBase;

struct DrawInfo
{
	Shader* draw_shader_ = nullptr;
	bool needBind_ = true;
	Matrixf	matrix_ = Matrixf(1.0f);
};

class LIBENIGHT_EXPORT CommonGeometry :public base::BaseObject
{
public:
	CommonGeometry(bool update = false);
	virtual ~CommonGeometry();

	virtual void updateGeometry(const CameraBase* camera);
	virtual void initGeometry() = 0;
	virtual void drawGeoemtry(const DrawInfo&) = 0;
	virtual void computeBoundingBox(void *);
	virtual Matrixf				getModelMatrix()const;
	virtual void				updateModelMatix(const Matrixf&);

	const base::BoundingBox& boundingBox()const;
protected:
	base::BoundingBox  box_;
	Matrixf modelMatrix_;
	bool   beupdte_;
};


class LIBENIGHT_EXPORT  MeshGeometry :public CommonGeometry
{
public:
	MeshGeometry(IRenderMeshObj_SP, VERTEX_TYPE);
	~MeshGeometry();

	virtual void initGeometry();
	virtual void drawGeoemtry(const DrawInfo&);
	virtual void computeBoundingBox(void *);

	void		 addMesh(Mesh_SP);

protected:
	virtual void setupMesh();
	virtual void drawMesh(int, Matrixf, Shader *);
	virtual int  setupVertexAttribute();
	
	std::vector<Mesh_SP>		meshs_;
	VERTEX_TYPE					vertex_type_;
	IRenderMeshObj_SP			mesh_obj_;


};
 
typedef base::SmartPointer<CommonGeometry> CommonGeometry_Sp;
typedef base::SmartPointer<MeshGeometry>	MeshGeometry_Sp;

#endif /* baseGeometry_h */
