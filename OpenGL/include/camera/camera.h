//
//  camera.h
//  OpenGL
//
//  Created by xt on 15/8/9.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#ifndef __OpenGL__camer__
#define __OpenGL__camer__

#include "boundingBox.h"
#include "cameraBase.h"

#include <stdio.h>

using namespace math;
using namespace base;

class Scene;

class LIBENIGHT_EXPORT Camera :public CameraBase
{
public:
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	enum Camera_Mouse_Button
	{
		LEFT_BUTTON,
		RIGHT_BUTTON,
		MIDDLE_BUTTON
	};

	enum Camera_Mouse_Action
	{
		PRESS,
		RELEASE,
		DRAG,
		SCROLLUP,
		SCROLLDOWN
	};

	enum Camera_Key_Action
	{
		KEY_PRESS,
		KEY_RELEASE,
		KEY_REPEAT
	};

	typedef enum {
		CAMERA				= 0x00000001,
		OBJECT				= 0x00000002,
		CAMERA_THIRD_PERSON = 0x00000004,

		ZOOM			    = 0x00000010,
		PAN					= 0x00000020,
		ROTATE				= 0x00000040,

		ALL					= ZOOM | PAN | ROTATE,
		DONT_TRANSLATE_FOCUS= 0x00000100,
		PAN_RELATIVE		= 0x00000200,

	}Behavior;

	typedef enum {
		_NIL,
		_TUMBLE,
		_DOLLY,
		_PAN 
	} Mode;

	Camera();

	virtual ~Camera();

	virtual Matrixf				getViewMatrix()const;
	virtual Matrixf				getProjectionMatrix()const;
	virtual void				update()override;
    void						setBehavior(int flag);
	
	virtual void                positionCamera(float positionX, float positionY, float positionZ,
		float centerX, float centerY, float centerZ,
		float upX, float upY, float upZ);

	virtual void				setEyePos(const V3f&);
	virtual V3f					getPosition()const;
	virtual V3f					getViewDir()const override;

	void						setFousePos(const V3f&);

	virtual void		processKeyboard(int key, int st, int action, int mods, float deltaTime);
	virtual void		mouse_move(const V2f & pt, const float & z_scale);
	virtual void        mouse_down(int button,const V2f& pt, int state);
	virtual void		mouse_up(int button,const V2f& pt, int state);
	virtual void        mouse_scroll(double xoffset,double yoffset);

    bool                isAABBVisible_E(const AABB&)const;
protected:
	virtual void       mouse_move_tumble(const V2f&pt);
	virtual void       mouse_move_plan(const V2f&pt);
	virtual void       mouse_move_dolly(const V2f&pt);
private:

protected:
	Mode				_mode;
	long				_behavior;

	glm::quat            _track_quat;
	glm::quat            _curquat;

	V2f		             _mouse_start;

	V3f		             _eye_pos;
	V3f					 _prev_eye_pos;

	V3f					_focus_pos;
	V3f					_prev_focus_pos;

	V3f		            _eye_y;

	Matrixf	            viewMatrix_;
	Matrixf             old_viewMatrix_;
	
	Matrixf	            _p;
	Matrixf             _camera;

};

#endif
