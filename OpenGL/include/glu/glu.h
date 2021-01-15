
#ifndef _GLU_H_
#define  _GLU_H_

#define GLU_RELEASE                0
#define GLU_PRESS                  1
#define GLU_REPEAT				   2

#define GLU_KEY_UNKNOWN            -1

/* Printable keys */
#define GLU_KEY_SPACE              32
#define GLU_KEY_APOSTROPHE         39  /* ' */
#define GLU_KEY_COMMA              44  /* , */
#define GLU_KEY_MINUS              45  /* - */
#define GLU_KEY_PERIOD             46  /* . */
#define GLU_KEY_SLASH              47  /* / */
#define GLU_KEY_0                  48
#define GLU_KEY_1                  49
#define GLU_KEY_2                  50
#define GLU_KEY_3                  51
#define GLU_KEY_4                  52
#define GLU_KEY_5                  53
#define GLU_KEY_6                  54
#define GLU_KEY_7                  55
#define GLU_KEY_8                  56
#define GLU_KEY_9                  57
#define GLU_KEY_SEMICOLON          59  /* ; */
#define GLU_KEY_EQUAL              61  /* = */
#define GLU_KEY_A                  65
#define GLU_KEY_B                  66
#define GLU_KEY_C                  67
#define GLU_KEY_D                  68
#define GLU_KEY_E                  69
#define GLU_KEY_F                  70
#define GLU_KEY_G                  71
#define GLU_KEY_H                  72
#define GLU_KEY_I                  73
#define GLU_KEY_J                  74
#define GLU_KEY_K                  75
#define GLU_KEY_L                  76
#define GLU_KEY_M                  77
#define GLU_KEY_N                  78
#define GLU_KEY_O                  79
#define GLU_KEY_P                  80
#define GLU_KEY_Q                  81
#define GLU_KEY_R                  82
#define GLU_KEY_S                  83
#define GLU_KEY_T                  84
#define GLU_KEY_U                  85
#define GLU_KEY_V                  86
#define GLU_KEY_W                  87
#define GLU_KEY_X                  88
#define GLU_KEY_Y                  89
#define GLU_KEY_Z                  90
#define GLU_KEY_LEFT_BRACKET       91  /* [ */
#define GLU_KEY_BACKSLASH          92  /* \ */
#define GLU_KEY_RIGHT_BRACKET      93  /* ] */
#define GLU_KEY_GRAVE_ACCENT       96  /* ` */
#define GLU_KEY_WORLD_1            161 /* non-US #1 */
#define GLU_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define GLU_KEY_ESCAPE             256
#define GLU_KEY_ENTER              257
#define GLU_KEY_TAB                258
#define GLU_KEY_BACKSPACE          259
#define GLU_KEY_INSERT             260
#define GLU_KEY_DELETE             261
#define GLU_KEY_RIGHT              262
#define GLU_KEY_LEFT               263
#define GLU_KEY_DOWN               264
#define GLU_KEY_UP                 265
#define GLU_KEY_PAGE_UP            266
#define GLU_KEY_PAGE_DOWN          267
#define GLU_KEY_HOME               268
#define GLU_KEY_END                269
#define GLU_KEY_CAPS_LOCK          280
#define GLU_KEY_SCROLL_LOCK        281
#define GLU_KEY_NUM_LOCK           282
#define GLU_KEY_PRINT_SCREEN       283
#define GLU_KEY_PAUSE              284
#define GLU_KEY_F1                 290
#define GLU_KEY_F2                 291
#define GLU_KEY_F3                 292
#define GLU_KEY_F4                 293
#define GLU_KEY_F5                 294
#define GLU_KEY_F6                 295
#define GLU_KEY_F7                 296
#define GLU_KEY_F8                 297
#define GLU_KEY_F9                 298
#define GLU_KEY_F10                299
#define GLU_KEY_F11                300
#define GLU_KEY_F12                301
#define GLU_KEY_F13                302
#define GLU_KEY_F14                303
#define GLU_KEY_F15                304
#define GLU_KEY_F16                305
#define GLU_KEY_F17                306
#define GLU_KEY_F18                307
#define GLU_KEY_F19                308
#define GLU_KEY_F20                309
#define GLU_KEY_F21                310
#define GLU_KEY_F22                311
#define GLU_KEY_F23                312
#define GLU_KEY_F24                313
#define GLU_KEY_F25                314
#define GLU_KEY_KP_0               320
#define GLU_KEY_KP_1               321
#define GLU_KEY_KP_2               322
#define GLU_KEY_KP_3               323
#define GLU_KEY_KP_4               324
#define GLU_KEY_KP_5               325
#define GLU_KEY_KP_6               326
#define GLU_KEY_KP_7               327
#define GLU_KEY_KP_8               328
#define GLU_KEY_KP_9               329
#define GLU_KEY_KP_DECIMAL         330
#define GLU_KEY_KP_DIVIDE          331
#define GLU_KEY_KP_MULTIPLY        332
#define GLU_KEY_KP_SUBTRACT        333
#define GLU_KEY_KP_ADD             334
#define GLU_KEY_KP_ENTER           335
#define GLU_KEY_KP_EQUAL           336
#define GLU_KEY_LEFT_SHIFT         340
#define GLU_KEY_LEFT_CONTROL       341
#define GLU_KEY_LEFT_ALT           342
#define GLU_KEY_LEFT_SUPER         343
#define GLU_KEY_RIGHT_SHIFT        344
#define GLU_KEY_RIGHT_CONTROL      345
#define GLU_KEY_RIGHT_ALT          346
#define GLU_KEY_RIGHT_SUPER        347
#define GLU_KEY_MENU               348

#define GLU_KEY_LAST               GLU_KEY_MENU

#define GLU_MOUSE_BUTTON_1         0
#define GLU_MOUSE_BUTTON_2         1
#define GLU_MOUSE_BUTTON_3         2
#define GLU_MOUSE_BUTTON_4         3
#define GLU_MOUSE_BUTTON_5         4
#define GLU_MOUSE_BUTTON_6         5
#define GLU_MOUSE_BUTTON_7         6
#define GLU_MOUSE_BUTTON_8         7
#define GLU_MOUSE_BUTTON_LAST      GLU_MOUSE_BUTTON_8
#define GLU_MOUSE_BUTTON_LEFT      GLU_MOUSE_BUTTON_1
#define GLU_MOUSE_BUTTON_RIGHT     GLU_MOUSE_BUTTON_2
#define GLU_MOUSE_BUTTON_MIDDLE    GLU_MOUSE_BUTTON_3


#define GLU_MOD_SHIFT           0x0001
#define GLU_MOD_CONTROL         0x0002
#define GLU_MOD_ALT             0x0004
#define GLU_MOD_SUPER           0x0008
#define GLU_MOD_CAPS_LOCK       0x0010
#define GLU_MOD_NUM_LOCK        0x0020


#define GLU_NO_ERROR               0
#define GLU_NOT_INITIALIZED        0x00010001
#define GLU_NO_CURRENT_CONTEXT     0x00010002
#define GLU_INVALID_ENUM           0x00010003
#define GLU_INVALID_VALUE          0x00010004
#define GLU_OUT_OF_MEMORY          0x00010005
#define GLU_API_UNAVAILABLE        0x00010006
#define GLU_VERSION_UNAVAILABLE    0x00010007
#define GLU_PLATFORM_ERROR         0x00010008
#define GLU_FORMAT_UNAVAILABLE     0x00010009
#define GLU_NO_WINDOW_CONTEXT      0x0001000A
#define GLU_CURSOR_UNAVAILABLE     0x0001000B
#define GLU_FOCUSED                0x00020001
#define GLU_ICONIFIED              0x00020002
#define GLU_RESIZABLE              0x00020003
#define GLU_VISIBLE                0x00020004
#define GLU_DECORATED              0x00020005
#define GLU_AUTO_ICONIFY           0x00020006
#define GLU_FLOATING               0x00020007
#define GLU_MAXIMIZED              0x00020008
#define GLU_CENTER_CURSOR          0x00020009
#define GLU_TRANSPARENT_FRAMEBUFFER 0x0002000A
#define GLU_HOVERED                0x0002000B
#define GLU_FOCUS_ON_SHOW          0x0002000C
#define GLU_RED_BITS               0x00021001
#define GLU_GREEN_BITS             0x00021002
#define GLU_BLUE_BITS              0x00021003
#define GLU_ALPHA_BITS             0x00021004
#define GLU_DEPTH_BITS             0x00021005
#define GLU_STENCIL_BITS           0x00021006
#define GLU_ACCUM_RED_BITS         0x00021007
#define GLU_ACCUM_GREEN_BITS       0x00021008
#define GLU_ACCUM_BLUE_BITS        0x00021009
#define GLU_ACCUM_ALPHA_BITS       0x0002100A
#define GLU_AUX_BUFFERS            0x0002100B
#define GLU_STEREO                 0x0002100C
#define GLU_SAMPLES                0x0002100D
#define GLU_SRGB_CAPABLE           0x0002100E
#define GLU_REFRESH_RATE           0x0002100F
#define GLU_DOUBLEBUFFER           0x00021010
#define GLU_CLIENT_API             0x00022001
#define GLU_CONTEXT_VERSION_MAJOR  0x00022002
#define GLU_CONTEXT_VERSION_MINOR  0x00022003
#define GLU_CONTEXT_REVISION       0x00022004
#define GLU_CONTEXT_ROBUSTNESS     0x00022005
#define GLU_OPENGL_FORWARD_COMPAT  0x00022006
#define GLU_OPENGL_DEBUG_CONTEXT   0x00022007
#define GLU_OPENGL_PROFILE         0x00022008

#define GLU_ARROW_CURSOR           0x00036001
#define GLU_IBEAM_CURSOR           0x00036002
#define GLU_CROSSHAIR_CURSOR       0x00036003
#define GLU_POINTING_HAND_CURSOR   0x00036004
#define GLU_RESIZE_EW_CURSOR       0x00036005
#define GLU_RESIZE_NS_CURSOR       0x00036006
#define GLU_RESIZE_NWSE_CURSOR     0x00036007
#define GLU_RESIZE_NESW_CURSOR     0x00036008
#define GLU_RESIZE_ALL_CURSOR      0x00036009
#define GLU_NOT_ALLOWED_CURSOR     0x0003600A
#define GLU_HRESIZE_CURSOR         GLU_RESIZE_EW_CURSOR
#define GLU_VRESIZE_CURSOR         GLU_RESIZE_NS_CURSOR
#define GLU_HAND_CURSOR            GLU_POINTING_HAND_CURSOR

typedef struct GLUwindow GLUwindow;
typedef struct GLUcursor GLUcursor;

typedef void(*GLUwindowposfun)(GLUwindow*, int, int);
typedef void(*GLUwindowsizefun)(GLUwindow*, int, int);
typedef void(*GLUwindowclosefun)(GLUwindow*);
typedef void(*GLUwindowfocusfun)(GLUwindow*, int);
typedef void(*GLUframebuffersizefun)(GLUwindow*, int, int);
typedef void(*GLUmousebuttonfun)(GLUwindow*, int, int, int);
typedef void(*GLUcursorposfun)(GLUwindow*, double, double);
typedef void(*GLUcursorenterfun)(GLUwindow*, int);
typedef void(*GLUscrollfun)(GLUwindow*, double, double);
typedef void(*GLUkeyfun)(GLUwindow*, int, int, int, int);
typedef void(*GLUcharfun)(GLUwindow*, unsigned int);

LIBENIGHT_EXPORT GLUkeyfun gluSetKeyCallback(GLUwindow* window, GLUkeyfun callback);
LIBENIGHT_EXPORT GLUmousebuttonfun gluSetMouseButtonCallback(GLUwindow* window, GLUmousebuttonfun callback);
LIBENIGHT_EXPORT GLUwindowposfun gluSetWindowPosCallback(GLUwindow* window, GLUwindowposfun callback);
LIBENIGHT_EXPORT GLUwindowsizefun gluSetWindowSizeCallback(GLUwindow* window, GLUwindowsizefun callback);
LIBENIGHT_EXPORT GLUwindowclosefun gluSetWindowCloseCallback(GLUwindow* window, GLUwindowclosefun callback);
LIBENIGHT_EXPORT GLUwindowfocusfun gluSetWindowFocusCallback(GLUwindow* window, GLUwindowfocusfun callback);
LIBENIGHT_EXPORT GLUframebuffersizefun gluSetFramebufferSizeCallback(GLUwindow* window, GLUframebuffersizefun callback);
LIBENIGHT_EXPORT GLUcursorposfun gluSetCursorPosCallback(GLUwindow* window, GLUcursorposfun callback);
LIBENIGHT_EXPORT GLUcursorenterfun gluSetCursorEnterCallback(GLUwindow* window, GLUcursorenterfun callback);
LIBENIGHT_EXPORT GLUscrollfun gluSetScrollCallback(GLUwindow* window, GLUscrollfun callback);
LIBENIGHT_EXPORT GLUcharfun gluSetCharCallback(GLUwindow* window, GLUcharfun callback);


LIBENIGHT_EXPORT void	gluWindowHint(int hint, int value);
LIBENIGHT_EXPORT void	defaultWindowHints(void);

LIBENIGHT_EXPORT bool		gluInitWindow();
LIBENIGHT_EXPORT GLUwindow *gluCreateWindow(int width,int height,const char*);
LIBENIGHT_EXPORT void		gluDestoryWindow(GLUwindow*);

LIBENIGHT_EXPORT void		gluMakeCurrentContext(GLUwindow*);
LIBENIGHT_EXPORT void		gluMakeCurrentShareContext(GLUwindow*);
LIBENIGHT_EXPORT void		gluMakeCurrentShareContext();


LIBENIGHT_EXPORT void		gluMakeCurrentContextNULL(GLUwindow*);
LIBENIGHT_EXPORT void		gluMakeCurrentShareContextNULL(GLUwindow*);
LIBENIGHT_EXPORT void		gluMakeCurrentShareContextNULL();

LIBENIGHT_EXPORT void		gluSwapBuffer(GLUwindow*);
LIBENIGHT_EXPORT void		gluTerminate();

LIBENIGHT_EXPORT	bool	gluWindowShouldClose(GLUwindow*);
LIBENIGHT_EXPORT	void	gluPollEvents();
LIBENIGHT_EXPORT	void	gluSwapInterval(int interval);
LIBENIGHT_EXPORT	void	gluGetCursorPos(GLUwindow* handle, double* xpos, double* ypos);
LIBENIGHT_EXPORT	int		gluGetKey(GLUwindow*, int key);
LIBENIGHT_EXPORT	void    gluSetCursorPos(GLUwindow* window, double xpos, double ypos);

LIBENIGHT_EXPORT	 void	gluSetWindowShouldClose(GLUwindow* window, int value);

LIBENIGHT_EXPORT	const char* gluGetClipboardString(GLUwindow*handle);
LIBENIGHT_EXPORT	void  gluSetClipboardString(GLUwindow*handle,const char*);


LIBENIGHT_EXPORT	void*		gluGetWin32Window(GLUwindow*);
LIBENIGHT_EXPORT GLUcursor*		gluCreateStandardCursor(int shape);
LIBENIGHT_EXPORT void gluSetCursor(GLUwindow* windowHandle, GLUcursor* cursorHandle);
LIBENIGHT_EXPORT void gluDestroyCursor(GLUcursor* handle);

LIBENIGHT_EXPORT void gluGetWindowSize(GLUwindow* window, int* width, int* height);
LIBENIGHT_EXPORT void gluGetFramebufferSize(GLUwindow* window, int* width, int* height);
LIBENIGHT_EXPORT double gluGetTime(void);
LIBENIGHT_EXPORT int gluGetMouseButton(GLUwindow* window, int button);
LIBENIGHT_EXPORT int gluGetWindowAttrib(GLUwindow* window, int attrib);


#endif _GLU_H_
