#ifndef Opengl_RenderNode_h
#define Opengl_RenderNode_h

#include "IRenderNode.h"
#include "BaseObject.h"
#include "geometry.h"
#include "texture.h"

using namespace base;

class LIBENIGHT_EXPORT RenderNode :public IRenderNode
{
public:
	virtual void				render(Shader*, PassInfo&);
	virtual void				update(const CameraBase*);

	virtual AABB				getBox()const;
	virtual Matrixf				getModelMatix()const;

	void						setGeometry(CommonGeometry_Sp);
	CommonGeometry_Sp			getGeometry();

	void					    loadTextures();
	void						textures(std::vector<Texture_Sp>&);
	Texture_Sp					textures(int i)const;
	//static	IRenderNode_SP		loadFromFile(const char*file);

protected:
	CommonGeometry_Sp			geometry_;
	std::vector<Texture_Sp>		textures_;
};
typedef SmartPointer<RenderNode> RenderNode_SP;

#endif
