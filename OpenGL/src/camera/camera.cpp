//
//  camer.cpp
//  OpenGL
//
//  Created by xt on 15/8/9.

//  Copyright (c) 2015ƒÍ xt. All rights reserved.


#include "helpF.h"
#include "camera.h"
#include "windowManager.h"
#include "scene.h"
#include "gls.h"
#include "glu.h"
using namespace math;




Camera::Camera():
	_mode(_NIL),
	_behavior(OBJECT | ALL | PAN_RELATIVE),
	_track_quat(Quatf(0.0,0.0,0.0,1.0)),
	_curquat(Quatf(0.0,0.0,0.0,1.0)),
	_eye_y(V3f(0.0,1.0,0.0)), // _eye_y(vec3_y), // hack by woid
    _mouse_start(V2f(0.0,0.0)),
    _eye_pos(V3f(0.0,0.0,0.0)),
    _prev_eye_pos(V3f(0.0,0.0,0.0)),
    _focus_pos(V3f(0.0,0.0,0.0)),
    viewMatrix_(Matrixf(1.0)),
    old_viewMatrix_(Matrixf(1.0)),
    _camera(Matrixf(1.0)),
    _p(Quatf(0.0,0.0,0.0,1.0))
{

}

Camera::~Camera()
{

}

Matrixf Camera::getViewMatrix() const
{
	return viewMatrix_;
}

Matrixf Camera::getProjectionMatrix() const
{
	return perspective(_fov, _screen_ratio, _near_z, _far_z);
}


void Camera::update()
{

}

void Camera::setBehavior(int flag)
{
    _behavior = flag;
}


void Camera::positionCamera(float positionX, float positionY, float positionZ, float centerX, 
	float centerY, float centerZ, float upX, float upY, float upZ)
{
	_eye_pos = V3f(positionX, positionY, positionZ);
	_focus_pos = V3f(centerX, centerY, centerZ);
	viewMatrix_ = lookAt(_eye_pos,_focus_pos,V3f(upX,upY,upZ));

	if (_behavior & (CAMERA | CAMERA_THIRD_PERSON))
	{
		_track_quat = Quatf();
		_curquat = qfm(viewMatrix_);
		_curquat = normalize(_curquat);
	}
	initPos_ = true;
}

void Camera::setEyePos(const V3f&eye_pos)
{
	_eye_pos = eye_pos;
	if (_behavior & (CAMERA | CAMERA_THIRD_PERSON))
	{
		viewMatrix_ = lookAt(_eye_pos, _focus_pos, V3f(0.0,1.0,0.0));
		_track_quat = Quatf();
		_curquat = qfm(viewMatrix_);
		_curquat = normalize(_curquat);
	}
}

void Camera::setFousePos(const V3f&focus_pos)
{
	_focus_pos = focus_pos;
	if (_behavior & (CAMERA | CAMERA_THIRD_PERSON))
	{
		viewMatrix_ = lookAt( _eye_pos, _focus_pos, V3f(0.0, 0.0, 1.0));
		_curquat = qfm(viewMatrix_);
		_curquat = normalize(_curquat);
	}
	_track_quat = Quatf();
}

V3f Camera::getPosition()const
{
	return _eye_pos;
}

math::V3f Camera::getViewDir() const
{
	V3f tv = _focus_pos - _eye_pos;
	return math::normalize(tv);
}

void Camera::processKeyboard(int key, int st, int action, int mods, float deltaTime)
{
	if (action == GLU_PRESS)
	{
		if      (key == GLU_KEY_O)
		{
			_behavior &= ~CAMERA_THIRD_PERSON;
			_behavior &= ~CAMERA;
			_behavior |= OBJECT;
		}
		else if (key == GLU_KEY_P)
		{
			_behavior &= ~OBJECT;
			_behavior &= ~CAMERA;
			_behavior |= CAMERA_THIRD_PERSON;
		}
		else if (key == GLU_KEY_C)
		{
			_behavior &= ~OBJECT;
			_behavior &= ~CAMERA_THIRD_PERSON;
			_behavior |= CAMERA;
		}

		else if (key == GLU_KEY_Z)
		{
			(_behavior & ZOOM) ? _behavior &= ~ZOOM : _behavior |= ZOOM;
		}
		else if (key == GLU_KEY_R)
		{
			(_behavior & ROTATE) ? _behavior &= ~ROTATE : _behavior |= ROTATE;
		}
		else if (key == GLU_KEY_N)
		{
			(_behavior & PAN) ? _behavior &= ~PAN : _behavior |= PAN;
		}
		else if (key == GLU_KEY_F)
		{
			(_behavior & DONT_TRANSLATE_FOCUS) ? _behavior &= ~DONT_TRANSLATE_FOCUS : _behavior |= DONT_TRANSLATE_FOCUS;
		}
		else if (key == GLU_KEY_L)
		{
			(_behavior & PAN_RELATIVE) ? _behavior &= ~PAN_RELATIVE : _behavior |= PAN_RELATIVE;
		}

		else if (key == GLU_KEY_W)
		{ 
			V3f dir(viewMatrix_[2][0], viewMatrix_[2][1], viewMatrix_[2][2]);
			_eye_pos -= dir;
			viewMatrix_ = lookAt(_eye_pos, _focus_pos, _eye_y);
		}

		else if (key == GLU_KEY_S)
		{
			V3f dir(viewMatrix_[2][0], viewMatrix_[2][1], viewMatrix_[2][2]);
			_eye_pos += dir;
			viewMatrix_ = lookAt(_eye_pos, _focus_pos, _eye_y);
		}
		else if (key == GLU_KEY_A)
		{
			V3f dir(viewMatrix_[0][0], viewMatrix_[0][1], viewMatrix_[0][2]);
			_eye_pos -= dir;
			_focus_pos -= dir;
			viewMatrix_ = lookAt(_eye_pos, _focus_pos, _eye_y);
		}
		else if (key == GLU_KEY_D)
		{
			V3f dir(viewMatrix_[0][0], viewMatrix_[0][1], viewMatrix_[0][2]);
			_eye_pos += dir;
			_focus_pos += dir;
			viewMatrix_ = lookAt(_eye_pos, _focus_pos, _eye_y);
		}
	}
	else if (action == GLU_RELEASE)
	{
	}
}

void Camera::mouse_move(const V2f&  pt, const float & z_scale)
{
	switch (_mode)
	{
	case _TUMBLE:
		mouse_move_tumble(pt);
		break;
	case _PAN:
		mouse_move_plan(pt);
		break;
	case _DOLLY:
		mouse_move_dolly(pt);
		break;
	default:
		break;
	}
	
}

void Camera::mouse_down(int button,const V2f& pt, int state)
{
    old_viewMatrix_ = viewMatrix_;

    _mouse_start.x = pt.x;
    _mouse_start.y = pt.y;

    _track_quat = _curquat;

    _prev_eye_pos = _eye_pos;
    _prev_focus_pos = _focus_pos;

    if(button == GLU_MOUSE_BUTTON_LEFT)
        _mode = _TUMBLE;
	else if (button == GLU_MOUSE_BUTTON_MIDDLE)
        _mode = _PAN;
    else if(button == GLU_MOUSE_BUTTON_RIGHT)
        _mode = _DOLLY;
}

void Camera::mouse_up(int button,const V2f& pt, int state)
{
	old_viewMatrix_ = viewMatrix_;

	_mouse_start.x = pt.x;
	_mouse_start.y = pt.y;

	_track_quat = _curquat;

	_mode = _NIL;

}

void Camera::mouse_scroll(double xoffset, double yoffset)
{
}

bool Camera::isAABBVisible_E(const AABB&)const
{
    return  true;
}


void Camera::mouse_move_tumble(const V2f&pt)
{
	V2f pt1((2 * _mouse_start.x - _screen_width) / _screen_width,
				(_screen_height - 2 * _mouse_start.y) / _screen_height);

	V2f pt2((2 * pt.x - _screen_width) / _screen_width,
				(_screen_height - 2 * pt.y) / _screen_height);

	if (_behavior & CAMERA_THIRD_PERSON)
	{
		pt2.x -= pt1.x;
		pt2.y -= pt1.y;
		pt1.x = pt1.y = 0;
	}
	
	Quatf tmpquat;

	math::trackball(tmpquat, pt1, pt2, (0.8));

	if (_behavior & ROTATE)
	{
		Quatf cam_rot, inv_cam_rot;
		
		cam_rot = qfm(_camera);
		inv_cam_rot = conj(cam_rot);

		if (_behavior & CAMERA_THIRD_PERSON) {
			tmpquat = conj(tmpquat);
			_curquat = _track_quat * inv_cam_rot * tmpquat * cam_rot;
		}
		else if (_behavior & CAMERA)
			// Setting the camera in first person is equivalent to having _camera equal to _m in the third person camera formula
			_curquat = tmpquat * _track_quat;
		else if (_behavior & OBJECT)
			// In object mode _curquat is the inverse of _curquat in camera mode
			_curquat = inv_cam_rot * tmpquat * cam_rot * _track_quat;

		if (_behavior & (CAMERA | CAMERA_THIRD_PERSON))
		{
			V3f tmp =  _prev_eye_pos -  _focus_pos;
			Matrixf M = mfq(_curquat);
			float mag = std::sqrt(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);

			_eye_y = V3f(M[0][1], M[1][1], M[2][1]);
			V3f z(M[0][2], M[1][2], M[2][2]);

			z *= mag;
			_eye_pos = z + _focus_pos;
			viewMatrix_ = lookAt( _eye_pos, _focus_pos, _eye_y);
		}
		else if (_behavior & OBJECT)
		{
			Matrixf tmp = mfq(_curquat);

			viewMatrix_[0][0] = tmp[0][0];
			viewMatrix_[0][1] = tmp[0][1];
			viewMatrix_[0][2] = tmp[0][2];

			viewMatrix_[1][0] = tmp[1][0];
			viewMatrix_[1][1] = tmp[1][1];
			viewMatrix_[1][2] = tmp[1][2];

			viewMatrix_[2][0] = tmp[2][0];
			viewMatrix_[2][1] = tmp[2][1];
			viewMatrix_[2][2] = tmp[2][2];
		}
	}

}

void Camera::mouse_move_plan(const V2f&pt)
{
	V2f pt_delta;
	Matrixf inv_m;
	if (_behavior & PAN_RELATIVE)
	{
		V2f pt1((2.0 * _mouse_start.x - _screen_width) / _screen_width,
			(_screen_height - 2.0 * _mouse_start.y) / _screen_height);
		V2f pt2((2.0 * pt.x - _screen_width) / _screen_width,
			(_screen_height - 2.0 * pt.y) / _screen_height);
	 
		pt_delta.x = (pt1 - pt2).x * 5;
		pt_delta.y = (pt1 - pt2).y * 5;
	}
	else
	{
		pt_delta.x = pt.x * 2.0 / _screen_width - 1.0;
		pt_delta.y = (_screen_height - pt.y) * 2.0 / _screen_height - 1.0;
	}
	if (_behavior & (CAMERA | CAMERA_THIRD_PERSON))
	{
		if (_behavior & CAMERA_THIRD_PERSON)
		{
			pt_delta.x = -pt_delta.x;
			pt_delta.y = -pt_delta.y;
			inv_m = inverse(_camera);
		}
		else
			inv_m = inverse(viewMatrix_);

		float fov2 = (float)-tan(0.5 * Const<float>::pi_180() * _fov) * old_viewMatrix_[3][2];

		V4f in((float)(pt_delta.x * fov2 * _screen_ratio),
			(float)(pt_delta.y * fov2),
			(float)(_behavior & PAN_RELATIVE ? 0 : old_viewMatrix_[3][2]), 0.0
		);

		V4f offset_ = inv_m * in;
		V3f offset(offset_.x, offset_.y, offset_.z);
		if (!(_behavior & PAN_RELATIVE))
		{
			offset = offset + _prev_eye_pos;
			offset *= -1.0;
		}

		_eye_pos = _prev_eye_pos + offset;
		_focus_pos = _prev_focus_pos + offset;
		viewMatrix_ = lookAt(_eye_pos, _focus_pos, _eye_y);
	}
	else if (_behavior & OBJECT)
	{
		V3f obj_pos = V3f(old_viewMatrix_[3][0], old_viewMatrix_[3][1], old_viewMatrix_[3][2]);
		V4f eye_obj_pos_ = _camera * V4f(obj_pos.x, obj_pos.y, obj_pos.z, 1.0);
		V3f eye_obj_pos = V3f(eye_obj_pos_.x, eye_obj_pos_.y, eye_obj_pos_.z);

		float fov2 = (float)(-tan(0.5 * Const<float>::pi_180() * _fov) * eye_obj_pos.z);

		V4f shift(
			(float)(pt_delta.x * fov2 * _screen_ratio),
			(float)(pt_delta.y * fov2),
			(float)(_behavior & PAN_RELATIVE ? 0 : obj_pos.z),
			0.0
		);

		inv_m  = inverse(_camera);
		V4f o_ = inv_m * shift;
		V3f offset = V3f(o_.x, o_.y, o_.z);

		if (_behavior & PAN_RELATIVE)
		{
			viewMatrix_[3][0] = old_viewMatrix_[3][0] + offset.x;
			viewMatrix_[3][1] = old_viewMatrix_[3][1] + offset.y;
			viewMatrix_[3][2] = old_viewMatrix_[3][2] + offset.z;
		}
		else
		{
			viewMatrix_[3][0] = offset.x;
			viewMatrix_[3][1] = offset.y;
			viewMatrix_[3][2] = offset.z;
		}
	}
}

void Camera::mouse_move_dolly(const V2f&pt)
{
	float z_delta = 1.0 * ((pt.y - _mouse_start.y) / _screen_height * (_far_z - _near_z));
    z_delta *= 1.0;

	if (_behavior & ZOOM)
	{
		if (_behavior & (CAMERA | CAMERA_THIRD_PERSON))
		{
			V3f z = _prev_eye_pos - _prev_focus_pos;
			V3f norm_z = z;
			norm_z = normalize(norm_z);

			V3f z_offset(norm_z);
			z_offset *= z_delta;

			_eye_pos = _prev_eye_pos + z_offset;

			if (!(_behavior & DONT_TRANSLATE_FOCUS))
				_focus_pos = _prev_focus_pos + z_offset;
			viewMatrix_ = lookAt( _eye_pos, _focus_pos, _eye_y);
			if (_behavior & DONT_TRANSLATE_FOCUS)
				_curquat = qfm(viewMatrix_);
		}
		else if (_behavior & OBJECT)
		{
			V4f obj_pos_ = V4f(old_viewMatrix_[3][0], old_viewMatrix_[3][1], old_viewMatrix_[3][2],1.0);
			V4f eye_obj_pos_;
			Matrixf invcam;

			invcam = inverse(_camera);
			eye_obj_pos_ = _camera * obj_pos_;
			eye_obj_pos_.z += z_delta * 0.5;

			obj_pos_ =  invcam * eye_obj_pos_;

			viewMatrix_[3][0] = obj_pos_.x;
			viewMatrix_[3][1] = obj_pos_.y;
			viewMatrix_[3][2] = obj_pos_.z;
		}
	}
}
