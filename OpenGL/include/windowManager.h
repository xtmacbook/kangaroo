//
//  WindowManager.h
//  OpenGL
//
//  Created by xt on 15/8/10.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#ifndef __OpenGL__GLFWManager__
#define __OpenGL__GLFWManager__

#include "BaseObject.h"
#include "common.h"
#include <stdio.h>
#include <string>
#include "glu.h"

class  GLApplication;
class  InputManager;
class  Camera;

class LIBENIGHT_EXPORT WindowManager : public base::BaseObject
{
public:

	WindowManager();

	~WindowManager();

	bool					 initialize();
	bool					 createWindow(int width, int height, const char *title);
	bool					 initGui();
	bool					 destoryGui();
	void					 guiFrameBegin();
	void					 guiFrameRender();
	void					 loop();
	void					 swapTheBuffers();
	bool					 processInput(bool continueGame);
	virtual void			 destroy();
	GLUwindow*				 getWindow();
	InputManager*			 getInputManager();
	bool					 shouldeClose()const;
private:
	InputManager* inputManager_;
protected:

	GLUwindow* window_;
	Camera* camera_;
};

#endif /* defined(__OpenGL__GLFWManager__) */
