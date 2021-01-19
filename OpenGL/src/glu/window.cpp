
#include "glinternal.h"
#include "log.h"
#include "debug.h"


void _inputWindowFocus(GWindow* window, bool focused)
{
	if (window->callbacks_.focus_)
		window->callbacks_.focus_((GLUwindow*)window, focused);

	if (!focused)
	{
		int key, button;

		for (key = 0; key <= GLU_KEY_LAST; key++)
		{
			if (window->keys_[key] == GLU_PRESS)
			{
				const int scancode = PlatformGetKeyScancode(key);
				_inputKey(window, key, scancode, GLU_RELEASE, 0);
			}
		}

		for (button = 0; button <= GLU_MOUSE_BUTTON_LAST; button++)
		{
			if (window->mouseButtons_[button] == GLU_PRESS)
				_inputMouseClick(window, button, GLU_RELEASE, 0);
		}
	}
}

void _inputWindowDamage(GWindow* window)
{
	 
}

void _inputWindowCloseRequest(GWindow*window)
{
	window->shouldClose_ = true;
}

void gluGetCursorPos(GLUwindow* handle, double* xpos, double* ypos)
{
	platformGetCursorPos((GWindow*)handle, xpos, ypos);
}

int gluGetKey(GLUwindow*handle, int key)
{
	GWindow* window = (GWindow*)handle;

	if (key < GLU_KEY_SPACE || key > GLU_KEY_LAST)
	{
		return GLU_RELEASE;
	}
	return (int)window->keys_[key];
}
void gluMakeCurrentContext(GLUwindow*handle)
{
	enAssert(handle != NULL);

	GWindow* window = (GWindow*)handle;
	if (window)
		window->context_.makeCurrent_(&window->context_);
}

void gluMakeCurrentShareContext()
{
	//gluMakeCurrentShareContext((GLUwindow*)g_main_window);
}

void gluMakeCurrentShareContext(GLUwindow*handle)
{
	enAssert(handle != NULL);

	GWindow* window = (GWindow*)handle;
	if (window)
	{
		enAssert(window->sharedContext_ != nullptr);
		window->sharedContext_->makeCurrent_(window->sharedContext_);
	}
}

void gluMakeCurrentShareContextNULL(GLUwindow*handle)
{
	enAssert(handle != NULL);
	GWindow* window = (GWindow*)handle;
	enAssert(window->sharedContext_ != nullptr);
	window->sharedContext_->makeCurrent_(NULL);
}

void gluMakeCurrentContextNULL(GLUwindow*handle)
{
	enAssert(handle != NULL);
	((GWindow*)handle)->context_.makeCurrent_(NULL);
}

void gluMakeCurrentShareContextNULL()
{
	//gluMakeCurrentShareContextNULL((GLUwindow*)g_main_window);
}

void gluSwapBuffer(GLUwindow*handle)
{
	enAssert(handle != NULL);

	GWindow* window = (GWindow*)handle;
	if (window)
		window->context_.swapBuffers_(&window->context_);
}

void gluSwapInterval(int interval)
{
	Context * context = (Context*)platformGetTls(&opg.contextSlot_);
	if (!context)
	{
		PRINT_ERROR("Cannot set swap interval without a current OpenGL or OpenGL ES context");
		return;
	}

	context->swapInterval_(interval);
}

void gluPollEvents()
{
	platformPollEvents();
}

void gluDestoryWindow(GLUwindow*handle)
{
	GWindow* window = (GWindow*)handle;

	if (window == NULL)
		return;

	memset(&window->callbacks_, 0, sizeof(window->callbacks_));
	// The window's context must not be current on another thread when the
	// window is destroyed
	if (&window->context_.wgl == platformGetTls(&opg.contextSlot_))
	{
		if (window->sharedContext_) gluMakeCurrentShareContextNULL();
		gluMakeCurrentContextNULL(handle);
	}

	PlatformDestroyWindow(window);

	{
		GWindow** prev = &opg.windowListHead_;

		while (*prev != window)
			prev = &((*prev)->next_);

		*prev = window->next_;
	}
	delete window;

	window = NULL;
}

bool						gluWindowShouldClose(GLUwindow*handle)
{
	GWindow* window = (GWindow*)handle;
	return window->shouldClose_;
}

GLUwindow * gluCreateWindow(int width, int height, const char*title)
{
	enAssert(title != NULL);
	enAssert(width >= 0);
	enAssert(height >= 0);

	Bconfig fbconfig;
	Context_Config ctxconfig;
	WindowConfig wndconfig;

	fbconfig = opg.hints.framebuffer_;
	ctxconfig = opg.hints.context_;
	wndconfig = opg.hints.window_;

	wndconfig.width_ = width;
	wndconfig.height_ = height;
	wndconfig.title_ = title;

	//const DeviceConfig*dc, const WindowConfig*wc
	GWindow* window = new GWindow;
	window->next_ = opg.windowListHead_;
	opg.windowListHead_ = window;
	memset(window->mouseButtons_,0, sizeof(window->mouseButtons_));
	platformCreateWindow(window, &wndconfig, &ctxconfig,&fbconfig);

	platformShowWindow(window);
	platformFocusWindow(window);
	return (GLUwindow*)window;
}
void gluWindowHint(int hint, int value)
{
	switch (hint)
	{
	case GLU_RED_BITS:
		opg.hints.framebuffer_.redBits = value;
		return;
	case GLU_GREEN_BITS:
		opg.hints.framebuffer_.greenBits = value;
		return;
	case GLU_BLUE_BITS:
		opg.hints.framebuffer_.blueBits = value;
		return;
	case GLU_ALPHA_BITS:
		opg.hints.framebuffer_.alphaBits = value;
		return;
	case GLU_DEPTH_BITS:
		opg.hints.framebuffer_.depthBits = value;
		return;
	case GLU_STENCIL_BITS:
		opg.hints.framebuffer_.stencilBits = value;
		return;
	case GLU_ACCUM_RED_BITS:
		opg.hints.framebuffer_.accumRedBits = value;
		return;
	case GLU_ACCUM_GREEN_BITS:
		opg.hints.framebuffer_.accumGreenBits = value;
		return;
	case GLU_ACCUM_BLUE_BITS:
		opg.hints.framebuffer_.accumBlueBits = value;
		return;
	case GLU_ACCUM_ALPHA_BITS:
		opg.hints.framebuffer_.accumAlphaBits = value;
		return;
	case GLU_AUX_BUFFERS:
		opg.hints.framebuffer_.auxBuffers = value;
		return;
	case GLU_STEREO:
		opg.hints.framebuffer_.stereo = value ? GLU_TRUE : GLU_FALSE;
		return;
	case GLU_DOUBLEBUFFER:
		opg.hints.framebuffer_.doublebuffer = value ? GLU_TRUE : GLU_FALSE;
		return;
	case GLU_TRANSPARENT_FRAMEBUFFER:
		opg.hints.framebuffer_.transparent = value ? GLU_TRUE : GLU_FALSE;
		return;
	case GLU_SAMPLES:
		opg.hints.framebuffer_.samples = value;
		return;
	case GLU_SRGB_CAPABLE:
		opg.hints.framebuffer_.sRGB = value ? GLU_TRUE : GLU_FALSE;
		return;
	case GLU_VISIBLE:
		opg.hints.window_.visible_ = value ? GLU_TRUE : GLU_FALSE;
		return;
	case GLU_CONTEXT_VERSION_MAJOR:
		opg.hints.context_.glMaxJor_ = value;
		return;
	case GLU_CONTEXT_VERSION_MINOR:
		opg.hints.context_.glMinJor_ = value;
		return;
	case GLU_CONTEXT_ROBUSTNESS:
		opg.hints.context_.robustness_ = (Context_Config::ROBUSTNESS)value;
		return;
	case GLU_OPENGL_FORWARD_COMPAT:
		opg.hints.context_.glforward_compat_ = value ? GLU_TRUE : GLU_FALSE;
		return;
	case GLU_OPENGL_DEBUG_CONTEXT:
		opg.hints.context_.debug_context_ = value ? GLU_TRUE : GLU_FALSE;
		return;
	case GLU_OPENGL_PROFILE:
		opg.hints.context_.glProfile_ = (Context_Config::PROFILE)value;
		return;
	}

	PRINT_MSG("no this hint %d , %d \n", hint, value);
}

GLUwindowfocusfun gluSetWindowFocusCallback(GLUwindow* handle, GLUwindowfocusfun callback)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);
	window->callbacks_.focus_ = callback;
	return callback;
}

GLUwindowposfun gluSetWindowPosCallback(GLUwindow* handle, GLUwindowposfun callback)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);
	window->callbacks_.pos_ = callback;
	return callback;
}
GLUwindowsizefun gluSetWindowSizeCallback(GLUwindow* handle, GLUwindowsizefun callback)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);
	window->callbacks_.size_ = callback;
	return callback;
}
GLUwindowclosefun gluSetWindowCloseCallback(GLUwindow* handle, GLUwindowclosefun callback)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);
	window->callbacks_.close_ = callback;
	return callback;
}

void	gluSetWindowShouldClose(GLUwindow* handle, int value)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);
	window->shouldClose_ = value;
}

void gluGetWindowSize(GLUwindow* handle, int* width, int* height)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);

	if (width)
		*width = 0;
	if (height)
		*height = 0;

	PlatformGetWindowSize(window, width, height);
}

void gluGetFramebufferSize(GLUwindow* handle, int* width, int* height)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);

	if (width)
		*width = 0;
	if (height)
		*height = 0;

	PlatformGetFramebufferSize(window, width, height);
}

int gluGetWindowAttrib(GLUwindow* handle, int attrib)
{
	GWindow* window = (GWindow*)handle;
	enAssert(window != NULL);


	switch (attrib)
	{
	case GLU_FOCUSED:
		return PlatformWindowFocused(window);
	case GLU_ICONIFIED:
		return PlatformWindowIconified(window);
	case GLU_VISIBLE:
		return PlatformWindowVisible(window);
	case GLU_MAXIMIZED:
		return PlatformWindowMaximized(window);
	case GLU_HOVERED:
		return PlatformWindowHovered(window);
	}

	PRINT_MSG( "Invalid window attribute 0x%08X", attrib);
	return 0;
}
