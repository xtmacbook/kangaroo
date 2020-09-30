#ifndef _Render_h_
#define _Render_h_

#include "decl.h"
#include "smartPointer.h"
#include "BaseObject.h"
#include "thread.h"

class  IRender :public Base::BaseObject
{

};

class  Render :public IRender,public Base::Thread
{
public:
	Render();

	~Render();

	virtual void	render();

	virtual void    run();


};

typedef Base::SmartPointer<Render>  Render_SP;
#endif
