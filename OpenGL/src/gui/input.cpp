#include "glinternal.h"
#include "debug.h"
#include "util.h"
#include "log.h"

#define _GLU_STICK 3

template <class T>
void SWAP_POINTERS(T& x, T& y)
{
	T t;
	t = x;
	x = y;
	y = t;
}

GLUkeyfun gluSetKeyCallback(GLUwindow* handle, GLUkeyfun callback)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);
	SWAP_POINTERS(window->callbacks_.key_ , callback);
	return callback;
}
GLUmousebuttonfun gluSetMouseButtonCallback(GLUwindow* handle, GLUmousebuttonfun callback)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);
	SWAP_POINTERS(window->callbacks_.mouseButton_, callback);
	return callback;
}

GLUframebuffersizefun gluSetFramebufferSizeCallback(GLUwindow* handle, GLUframebuffersizefun callback)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);
	SWAP_POINTERS(window->callbacks_.fbsize_, callback);
	return callback;
}
GLUcursorposfun gluSetCursorPosCallback(GLUwindow* handle, GLUcursorposfun callback)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);
	SWAP_POINTERS(window->callbacks_.cursorPos_, callback);
	return callback;
}
GLUcursorenterfun gluSetCursorEnterCallback(GLUwindow* handle, GLUcursorenterfun callback)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);
	SWAP_POINTERS(window->callbacks_.cursorEnter_, callback);
	return callback;
}
GLUscrollfun gluSetScrollCallback(GLUwindow* handle, GLUscrollfun callback)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);
	SWAP_POINTERS(window->callbacks_.scroll_, callback);
	return callback;
}

GLUcharfun gluSetCharCallback(GLUwindow* handle, GLUcharfun callback)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);
	SWAP_POINTERS(window->callbacks_.character_, callback);
	return callback;
}

void _inputWindowPos(GWindow* window, int xpos, int ypos)
{
	if (window->callbacks_.pos_)
		window->callbacks_.pos_((GLUwindow*)window, xpos, ypos);
}

void _inputWindowSize(GWindow* window, int width, int height)
{
	if (window->callbacks_.size_)
		window->callbacks_.size_((GLUwindow*)window, width, height);
}

void _inputScroll(GWindow* window, double xoffset, double yoffset)
{
	if (window->callbacks_.scroll_)
		window->callbacks_.scroll_((GLUwindow*)window, xoffset, yoffset);
}
void _inputMouseClick(GWindow* window, int button, int action, int mods)
{
	if (button < 0 || button > GLU_MOUSE_BUTTON_LAST)
		return;

	window->mouseButtons_[button] = (char)action;

	if (window->callbacks_.mouseButton_)
		window->callbacks_.mouseButton_((GLUwindow*)window, button, action, mods);
}

void _inputCursorPos(GWindow* window, double xpos, double ypos)
{
	if (window->virtualCursorPosX_ == xpos && window->virtualCursorPosY_ == ypos)
		return;

	window->virtualCursorPosX_ = xpos;
	window->virtualCursorPosY_ = ypos;

	if (window->callbacks_.cursorPos_)
		window->callbacks_.cursorPos_((GLUwindow*)window, xpos, ypos);
}

void _inputCursorEnter(GWindow* window, bool entered)
{
	if (window->callbacks_.cursorEnter_)
		window->callbacks_.cursorEnter_((GLUwindow*)window, entered);
}

const char* gluGetClipboardString(GLUwindow*handle)
{
	return PlatformGetClipboardString();
}

void  gluSetClipboardString(GLUwindow*handle, const char*string)
{
	PlatformSetClipboardString(string);
}

GLUcharfun glfwSetCharCallback(GLUwindow* handle, GLUcharfun cbfun)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);

	SWAP_POINTERS(window->callbacks_.character_, cbfun);

	return cbfun;
}
void _inputChar(GWindow* window, unsigned int codepoint, int mods, bool plain)
{
	if (codepoint < 32 || (codepoint > 126 && codepoint < 160))
		return;

	if (plain)
	{
		if (window->callbacks_.character_)
			window->callbacks_.character_((GLUwindow*)window, codepoint);
	}
}

GLUcursor* gluCreateStandardCursor(int shape)
{
	GWcursor* cursor;

	if (shape != GLU_ARROW_CURSOR &&
		shape != GLU_IBEAM_CURSOR &&
		shape != GLU_CROSSHAIR_CURSOR &&
		shape != GLU_POINTING_HAND_CURSOR &&
		shape != GLU_RESIZE_EW_CURSOR &&
		shape != GLU_RESIZE_NS_CURSOR &&
		shape != GLU_RESIZE_NWSE_CURSOR &&
		shape != GLU_RESIZE_NESW_CURSOR &&
		shape != GLU_RESIZE_ALL_CURSOR &&
		shape != GLU_NOT_ALLOWED_CURSOR)
	{
		PRINT_ERROR( "Invalid standard cursor 0x%08X", shape);
		return NULL;
	}

	cursor = (GWcursor*) calloc(1, sizeof(GWcursor));
	cursor->next = opg.cursorListHead_;
	opg.cursorListHead_ = cursor;

	if (!PlatformCreateStandardCursor(cursor, shape))
	{
		gluDestroyCursor((GLUcursor*)cursor);
		return NULL;
	}

	return (GLUcursor*)cursor;
}

 void gluDestroyCursor(GLUcursor* handle)
{
	 GWcursor* cursor = (GWcursor*)handle;

	if (cursor == NULL)
		return;

	// Make sure the cursor is not being used by any window
	{
		GWindow* window;

		for (window = opg.windowListHead_; window; window = window->next_)
		{
			if (window->cursor_ == cursor)
				gluSetCursor((GLUwindow*)window, NULL);
		}
	}

	PlatformDestroyCursor(cursor);

	// Unlink cursor from global linked list
	{
		GWcursor** prev = &opg.cursorListHead_;

		while (*prev != cursor)
			prev = &((*prev)->next);

		*prev = cursor->next;
	}

	free(cursor);
}

void gluSetCursor(GLUwindow* windowHandle, GLUcursor* cursorHandle)
{
	GWindow* window = (GWindow*)windowHandle;
	GWcursor* cursor = (GWcursor*)cursorHandle;
	enAssert(window != NULL);
	SWAP_POINTERS(window->cursor_, cursor);

	PlatformSetCursor(window, cursor);
}

double gluGetTime(void)
{
	return (double)(PlatformGetTimerValue() - opg.timer_.offset_) /
		PlatformGetTimerFrequency();
}

int gluGetMouseButton(GLUwindow* handle, int button)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);

	if (button < GLU_MOUSE_BUTTON_1 || button > GLU_MOUSE_BUTTON_LAST)
	{
		PRINT_ERROR( "Invalid mouse button %i", button);
		return false;
	}

	if (window->mouseButtons_[button] == _GLU_STICK)
	{
		// Sticky mode: release mouse button now
		window->mouseButtons_[button] = GLU_RELEASE;
		return GLU_PRESS;
	}
	int tmp = (int)window->mouseButtons_[button];

	return (int)window->mouseButtons_[button];
}

void gluSetCursorPos(GLUwindow* handle, double xpos, double ypos)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);

	if (xpos != xpos || xpos < -DBL_MAX || xpos > DBL_MAX ||
		ypos != ypos || ypos < -DBL_MAX || ypos > DBL_MAX)
	{
		PRINT_ERROR(
			"Invalid cursor position %f %f",
			xpos, ypos);
		return;
	}

	if (!PlatformWindowFocused(window))
		return;
	
	{
		PlatformSetCursorPos(window, xpos, ypos);
	}
}
