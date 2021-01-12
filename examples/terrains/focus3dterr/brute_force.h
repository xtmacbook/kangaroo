 
#ifndef __BRUTE_FORCE_H__
#define __BRUTE_FORCE_H__

#include "terrain.h"
#include "geometry.h"

class Cbrute_force_terrain : public Cterrain
{
	private:
	public:
	void render();
	virtual MeshGeometry_Sp initGeometry(IRenderMeshObj_SP);

	Cbrute_force_terrain( void )
	{	}
	~Cbrute_force_terrain( void )
	{	}
};

#endif	//__BRUTE_FORCE_H__
