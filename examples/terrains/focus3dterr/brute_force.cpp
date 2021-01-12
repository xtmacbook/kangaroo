 
#include <stdio.h>
#include "brute_force.h"
#include "common.h"
#include <mesh.h>
#include <resource.h>
#include <shader.h>
#include <gls.h>



void Cbrute_force_terrain::render()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture->getTexture());
}

MeshGeometry_Sp Cbrute_force_terrain::initGeometry(IRenderMeshObj_SP rsp)
{
	float fTexLeft, fTexBottom, fTexTop;

	base::SmartPointer<MeshGeometry> mg = new MeshGeometry(rsp,VERTEX_POINTS_NORMAL_TEXTURE);
	for (int iZ = 0; iZ < m_iSize - 1; iZ++)
	{
		//loop through the X-axis of the terrain
		//this is where the triangle strip is constructed
		Mesh_SP mesh = new Mesh(VERTEX_POINTS_NORMAL_TEXTURE);
		mesh->createMesh((m_iSize - 1) * 2);
		rsp->model(GL_TRIANGLE_STRIP);

		for (int iX = 0; iX < m_iSize - 1; iX++)
		{
			//calculate the texture coordinates
			fTexLeft = (float)iX / m_iSize;
			fTexBottom = (float)iZ / m_iSize;
			fTexTop = (float)(iZ + 1) / m_iSize;

			Vertex_PNT vertex0;
			//use height-based coloring (high-points are
			//light, low points are dark)
			unsigned char ucColor = getTrueHeightAtPoint(iX, iZ);

			vertex0.normal_ = V3f(ucColor,ucColor,ucColor);
			vertex0.position_ = V3f((float)iX, getScaledHeightAtPoint(iX, iZ), (float)iZ);
			vertex0.texCoords_ = V2f(fTexLeft, fTexBottom);

			Vertex_PNT vertex1;
			ucColor = getTrueHeightAtPoint(iX, iZ + 1);
			vertex1.normal_ = V3f(ucColor, ucColor, ucColor);
			vertex1.position_ = V3f((float)iX, getScaledHeightAtPoint(iX, iZ + 1), (float)iZ + 1);
			vertex1.texCoords_ = V2f(fTexLeft, fTexTop);

			mesh->addVertex(&vertex0);
			mesh->addVertex(&vertex1);

			m_iVertsPerFrame += 2;

			//there are no triangles being rendered on the first X-loop, they just start the
			//triangle strip off
			if (iX != 0)
				m_iTrisPerFrame += 2;
		}
		mesh->computeBox();

		mg->addMesh(mesh);
	}
	mg->initGeometry();
	return mg;
}

