
#include "glinter.h"
#include "window_w32.h"
#include "log.h"

void _inputWindowFocus(GWindow* window, bool focused)
{
	platformFocusWindow(window);
}

void _inputWindowDamage(GWindow* window)
{
	 
}

void _inputWindowCloseRequest(GWindow*window)
{
	window->shouldClose_ = true;
}

void gluGetCursorPos(GWindow* handle, double* xpos, double* ypos)
{
	platformGetCursorPos(handle, xpos, ypos);
}

int gluGetKey(GWindow*window, int key)
{
	if (key < GLU_KEY_SPACE || key > GLU_KEY_LAST)
	{
		return GLU_RELEASE;
	}
	return (int)window->keys_[key];
}
void gluMakeCurrentContext(GWindow*handle)
{
	GWindow* window = (GWindow*)handle;
	if (window)
		window->context_.makeCurrent_(&window->context_);
}

void gluSwapBuffer(GWindow*handle)
{
	GWindow* window = (GWindow*)handle;
	if (window)
		window->context_.swapBuffers_(&window->context_);
}

void gluTerminate()
{
	if (!opg.initialized_)
		return;
	while (opg.windowListHead_)
		gluDestoryWindow((GWindow*)opg.windowListHead_);

	platformTerminate();
	Base::destoryTls(&opg.contextSlot_);
	memset(&opg, 0, sizeof(opg));
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

void gluDestoryWindow(GWindow*handle)
{
	GWindow* window = (GWindow*)handle;

	if (window == NULL)
		return;

	// The window's context must not be current on another thread when the
	// window is destroyed
	if (&window->context_.wgl_ct_ == platformGetTls(&opg.contextSlot_))
		gluMakeCurrentContext(NULL);

	PlatformDestroyWindow(window);

	{
		GWindow** prev = &opg.windowListHead_;

		while (*prev != window)
			prev = &((*prev)->next_);

		*prev = window->next_;
	}
	free(window);
	window = NULL;
}

bool gluInitWindow()
{
	return platformInit();
}

Context*  gluCreateContext(HWND*hwd, HGLRC*share, const DeviceConfig*dc, const Bconfig*wc)
{
	Context* newcontext = new Context;
	if(__createContextWGL(hwd, share, newcontext, dc, wc)) return newcontext;
	delete newcontext;
	newcontext = NULL;
}
