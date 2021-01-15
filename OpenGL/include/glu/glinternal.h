
#ifndef  _GL_INTER_H_
#define  _GL_INTER_H_

#include "decl.h"
#include "thread.h"
#include "glu.h"
#include "win32_platform.h"

typedef void(*GLUWglproc)(void);
typedef void(*Makecontextcurrentfun)(Context*);
typedef void(*Swapbuffersfun)(Context*);
typedef void(*Swapintervalfun)(int);
typedef int(*Extensionsupportedfun)(const char*);
typedef GLUWglproc(*Getprocaddressfun)(const char*);
typedef void(*Destroycontextfun)(Context*);
//Framebuffer config
struct LIBENIGHT_EXPORT Bconfig
{
	int         redBits;
	int         greenBits;
	int         blueBits;
	int         alphaBits;
	int         depthBits;
	int         stencilBits;

	int         accumRedBits;
	int         accumGreenBits;
	int         accumBlueBits;
	int         accumAlphaBits;

	int         auxBuffers;
	bool	    stereo;
	int         samples;
	bool	    sRGB;
	bool		doublebuffer;
	bool	    transparent;
	uintptr_t   handle;
};
 
struct LIBENIGHT_EXPORT Context_Config
{
	enum ROBUSTNESS
	{
		NO_RESET_NOTIFICATION,
		LOSE_CONTEXT_ON_RESET
	};
	enum RELEASE
	{
		ANY_RELEASE_BEHAVIOR,
		RELEASE_BEHAVIOR_FLUSH,
		RELEASE_BEHAVIOR_NONE
	};

	enum PROFILE
	{
		OPENGL_CORE_PROFILE,
		OPENGL_COMPAT_PROFILE
	};
	int					glMaxJor_ = 3;
	int					glMinJor_ = 3;
	int					glforward_compat_;
	PROFILE				glProfile_;

	bool				   debug_context_ = false;
	bool				   debug_sync_ = false;
	ROBUSTNESS			   robustness_;
	RELEASE				   release_;
	bool				   noerror_ = false;

};
 
struct LIBENIGHT_EXPORT WindowConfig
{
	int width_;
	int height_;
	int pos_x_;
	int pos_y_;

	bool visible_ = true;
	const char * title_;
	bool shouldCreateSharedContext_ = false;
	bool gui_ = false;
};

struct GWindow;
struct GWcursor;

struct LIBENIGHT_EXPORT DLLLibrary
{
	bool            initialized_ = false;
	struct
	{
		Bconfig   framebuffer_;
		Context_Config context_;
		WindowConfig   window_;
	} hints;

	struct {
		uint64_t        offset_;
		// This is defined in the platform's time.h
		PLATFORM_LIBRARY_TIMER_STATE;
	} timer_;

	LibraryWin32			win32_;
	base::TlsWin32			contextSlot_;
	GWindow *				windowListHead_ = nullptr;
	GWcursor*				cursorListHead_ = nullptr;
	_GLU_PLATFORM_LIBRARY_CONTEXT_STATE;
};

typedef struct Vidmode
{
	int width_;
	int height_;
	int redBits_;
	int greenBits_;
	int refreshRate_;
} Vidmode;

typedef struct Gammaramp
{
	unsigned short* red;
	unsigned short* green;
	unsigned short* blue;
	unsigned int size;
} Gammaramp;

struct Monitor
{
	char*           name;
	void*           userPointer;

	// Physical dimensions in millimeters.
	int             widthMM, heightMM;

	// The window whose video mode is current on this monitor
	GWindow*    window;

	Vidmode*    modes;
	int         modeCount;
	Vidmode     currentMode;

	Gammaramp   originalRamp;
	Gammaramp   currentRamp;

	// This is defined in the window API's platform.h
	PLATFORM_WINDOW_STATE;
};

struct Context
{
	Makecontextcurrentfun  makeCurrent_ = 0;
	Swapbuffersfun         swapBuffers_ = 0;
	Swapintervalfun        swapInterval_ = 0;
	Extensionsupportedfun  extensionSupported_ = 0;
	Getprocaddressfun      getProcAddress_ = 0;
	Destroycontextfun      destroy_ = 0;

	_GLU_PLATFORM_CONTEXT_STATE;
};

struct GWcursor
{
	GWcursor*    next;

	// This is defined in the window API's platform.h
	PLATFORM_CURSOR_STATE;
};

struct LIBENIGHT_EXPORT GWindow
{
	~GWindow();

	GWindow *			next_ = NULL;
 
	Context				context_;
	Monitor*			monitor_ = NULL;
	GWcursor*			cursor_ = NULL;

	bool				shouldClose_ = false;

	int                 cursorMode_;
	char                mouseButtons_[GLU_MOUSE_BUTTON_LAST + 1];
	char                keys_[GLU_KEY_LAST + 1];

	double              virtualCursorPosX_;
	double				virtualCursorPosY_;

	Context*			sharedContext_ = nullptr;

	struct {
		GLUwindowposfun        pos_ = nullptr;
		GLUwindowsizefun       size_ = nullptr;
		GLUwindowclosefun      close_ = nullptr;
		GLUwindowfocusfun      focus_ = nullptr;
		GLUframebuffersizefun  fbsize_ = nullptr;
		GLUmousebuttonfun      mouseButton_ = nullptr;
		GLUcursorposfun        cursorPos_ = nullptr;
		GLUcursorenterfun      cursorEnter_ = nullptr;
		GLUscrollfun           scroll_ = nullptr;
		GLUkeyfun              key_ = nullptr;
		GLUcharfun			   character_ = nullptr;
	} callbacks_;

	PLATFORM_WINDOW_STATE;
};

extern struct DLLLibrary opg;

int platformInit();
int platformCreateWindow(GWindow* window,
	const WindowConfig* wndconfig,
	const Context_Config* ctxconfig,
	const Bconfig* fbconfig);
void platformGetCursorPos(GWindow* window, double* xpos, double* ypos);
void PlatformSetCursorPos(GWindow* window, double xpos, double ypos);

bool platformRefreshContextAttribs(GWindow* window,const Context_Config* ctxconfig);
void platformPollEvents();
void platformFocusWindow(GWindow* window);
void platformShowWindow(GWindow* window);
void PlatformDestroyWindow(GWindow*);
int  PlatformGetKeyScancode(int key);

int PlatformCreateStandardCursor(GWcursor* cursor, int shape);
void PlatformDestroyCursor(GWcursor* cursor);
void PlatformSetCursor(GWindow* window, GWcursor* cursor);

void PlatformSetClipboardString(const char* string);
const char* PlatformGetClipboardString(void);
void PlatformGetWindowSize(GWindow* window, int* width, int* height);
void PlatformGetFramebufferSize(GWindow* window, int* width, int* height);
uint64_t PlatformGetTimerValue(void);
uint64_t PlatformGetTimerFrequency(void);

int PlatformWindowFocused(GWindow* window);
int PlatformWindowIconified(GWindow* window);
int PlatformWindowVisible(GWindow* window);
int PlatformWindowMaximized(GWindow* window);
int PlatformWindowHovered(GWindow* window);

//////////////////////////////////////////////////////////////////////////
//////                          event API                       //////
//////////////////////////////////////////////////////////////////////////

void _inputWindowFocus(GWindow* window, bool focused);
void _inputWindowDamage(GWindow* window);
void _inputWindowCloseRequest(GWindow*);
void _inputMouseClick(GWindow* window, int button, int action, int mods);
void _inputCursorPos(GWindow* window, double xpos, double ypos);
void _inputCursorEnter(GWindow* window, bool entered);
void _inputWindowSize(GWindow* window, int width, int height);
void _inputWindowPos(GWindow* window, int xpos, int ypos);
void _inputKey(GWindow* window,
	int key, int scancode, int action, int mods);
void _inputScroll(GWindow* window, double xoffset, double yoffset);
void _inputChar(GWindow* window,unsigned int codepoint, int mods, bool plain);


#endif
