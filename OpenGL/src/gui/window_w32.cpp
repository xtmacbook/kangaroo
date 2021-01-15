
#ifdef  EN_OS_WIN32
#include <windowsx.h>
#include <shellapi.h>
#endif

#include "gls.h"
#include "glinternal.h"
#include "log.h"



LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HINSTANCE hInstance;

static int getKeyMods(void)
{
	int mods = 0;

	if (GetKeyState(VK_SHIFT) & 0x8000)
		mods |= GLU_MOD_SHIFT;
	if (GetKeyState(VK_CONTROL) & 0x8000)
		mods |= GLU_MOD_CONTROL;
	if (GetKeyState(VK_MENU) & 0x8000)
		mods |= GLU_MOD_ALT;
	if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
		mods |= GLU_MOD_SUPER;
	if (GetKeyState(VK_CAPITAL) & 1)
		mods |= GLU_MOD_CAPS_LOCK;
	if (GetKeyState(VK_NUMLOCK) & 1)
		mods |= GLU_MOD_NUM_LOCK;

	return mods;
}

extern void InputErrorWin32(int error, const char* description)
{
	WCHAR buffer[516] = L"";
	char message[516] = "";

	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL,
		GetLastError() & 0xffff,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buffer,
		sizeof(buffer) / sizeof(WCHAR),
		NULL);
	WideCharToMultiByte(CP_UTF8, 0, buffer, -1, message, sizeof(message), NULL, NULL);

	char buf[1024];
	snprintf(buf,1024,"%s: %s",description,message);
	PRINT_ERROR(buf);
}

bool registerWindowClassWin32()
{
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = windowProc;
	wc.hInstance = hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = _WNDCLASSNAME;

	if (!RegisterClassEx(&wc))
	{
		PRINT_ERROR("Win32: Failed to register window class! \n");
		return false;
	}

	return true;
}

void unregisterWindowClassWin32()
{
	UnregisterClass(_WNDCLASSNAME, GetModuleHandleW(NULL));
}

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	GWindow* window = (GWindow*)GetProp(hwnd, "GLU");

	if (!window)
	{
		switch (msg)
		{

		}
		return DefWindowProcW(hwnd, msg, wParam, lParam);

	}
	switch (msg) {
	
	case WM_MOUSEACTIVATE:
	{
		if (HIWORD(lParam) == WM_LBUTTONDOWN)
		{
			if (LOWORD(lParam) != HTCLIENT)
				window->win_.frameAction_ = true;
		}
		break;
	}

	case WM_CAPTURECHANGED:
	{
		if (lParam == 0 && window->win_.frameAction_)
		{
			window->win_.frameAction_ = false;
		}
		break;
	}
	case WM_SETFOCUS:
		_inputWindowFocus(window, true);
		return 0;
	case WM_KILLFOCUS:
	{
		_inputWindowFocus(window, false);
		return 0;
	}
	case WM_CLOSE:
	{
		_inputWindowCloseRequest(window);
		return 0;
	}
	case WM_SIZE:
		_inputWindowSize(window, LOWORD(lParam), HIWORD(lParam));
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		ValidateRect(hwnd, 0);
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
	{
		int i, button, action;

		if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
			button = GLU_MOUSE_BUTTON_LEFT;
		else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
			button = GLU_MOUSE_BUTTON_RIGHT;
		else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
			button = GLU_MOUSE_BUTTON_MIDDLE;
		else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
			button = GLU_MOUSE_BUTTON_4;
		else
			button = GLU_MOUSE_BUTTON_5;

		if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN ||
			msg == WM_MBUTTONDOWN || msg == WM_XBUTTONDOWN)
		{
			action = GLU_PRESS;
		}
		else
			action = GLU_RELEASE;

		for (i = 0; i <= GLU_MOUSE_BUTTON_LAST; i++)
		{
			if (window->mouseButtons_[i] == GLU_PRESS)
				break;
		}

		if (i > GLU_MOUSE_BUTTON_LAST)
			SetCapture(hwnd);

		_inputMouseClick(window, button, action, getKeyMods());

		for (i = 0; i <= GLU_MOUSE_BUTTON_LAST; i++)
		{
			if (window->mouseButtons_[i] == GLU_PRESS)
				break;
		}

		if (i > GLU_MOUSE_BUTTON_LAST)
			ReleaseCapture();

		if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP)
			return true;

		return 0;
	}
	case WM_MOUSEMOVE:
	{
		const int x = GET_X_LPARAM(lParam);
		const int y = GET_Y_LPARAM(lParam);
		_inputCursorPos(window, x, y);
		window->win_.lastCursorPosX_ = x;
		window->win_.lastCursorPosY_ = y;
		return 0;
	}
	case WM_CHAR:
	case WM_SYSCHAR:
	case WM_UNICHAR:
	{
		const bool plain = (msg != WM_SYSCHAR);

		if (msg == WM_UNICHAR && wParam == UNICODE_NOCHAR)
		{
			// WM_UNICHAR is not sent by Windows, but is sent by some
			// third-party input method engine
			// Returning TRUE here announces support for this message
			return TRUE;
		}

		_inputChar(window, (unsigned int)wParam, getKeyMods(), plain);

		return 0;
	}

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		int key, scancode;
		const int action = (HIWORD(lParam) & KF_UP) ? GLU_RELEASE : GLU_PRESS;
		const int mods = getKeyMods();

		scancode = (HIWORD(lParam) & (KF_EXTENDED | 0xff));
		if (!scancode)
		{
			scancode = MapVirtualKey((UINT)wParam, MAPVK_VK_TO_VSC);
		}

		key = opg.win32_.keycodes[scancode];

		// The Ctrl keys require special handling
		if (wParam == VK_CONTROL)
		{
			if (HIWORD(lParam) & KF_EXTENDED)
			{
				key = GLU_KEY_RIGHT_CONTROL;
			}
			else
			{
				// NOTE: Alt Gr sends Left Ctrl followed by Right Alt
				// HACK: We only want one event for Alt Gr, so if we detect
				//       this sequence we discard this Left Ctrl message now
				//       and later report Right Alt normally
				MSG next;
				const DWORD time = GetMessageTime();

				if (PeekMessageW(&next, NULL, 0, 0, PM_NOREMOVE))
				{
					if (next.message == WM_KEYDOWN ||
						next.message == WM_SYSKEYDOWN ||
						next.message == WM_KEYUP ||
						next.message == WM_SYSKEYUP)
					{
						if (next.wParam == VK_MENU &&
							(HIWORD(next.lParam) & KF_EXTENDED) &&
							next.time == time)
						{
							// Next message is Right Alt down so discard this
							break;
						}
					}
				}

				// This is a regular Left Ctrl message
				key = GLU_KEY_LEFT_CONTROL;
			}
		}
		else if (wParam == VK_PROCESSKEY)
		{
			// IME notifies that keys have been filtered by setting the
			// virtual key-code to VK_PROCESSKEY
			break;
		}

		if (action == GLU_RELEASE && wParam == VK_SHIFT)
		{
			_inputKey(window, GLU_KEY_LEFT_SHIFT, scancode, action, mods);
			_inputKey(window, GLU_KEY_RIGHT_SHIFT, scancode, action, mods);
		}
		else if (wParam == VK_SNAPSHOT)
		{
			_inputKey(window, key, scancode, GLU_PRESS, mods);
			_inputKey(window, key, scancode, GLU_RELEASE, mods);
		}
		else
			_inputKey(window, key, scancode, action, mods);

		break;
	}
	case WM_MOVE:
	{
		_inputWindowPos(window,
			GET_X_LPARAM(lParam),
			GET_Y_LPARAM(lParam));
		return 0;
	}
	case WM_MOUSELEAVE:
	{
		_inputCursorEnter(window, false);
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		_inputScroll(window, 0.0, (SHORT)HIWORD(wParam) / (double)WHEEL_DELTA);
		return 0;
	}
	case WM_MOUSEHWHEEL:
	{
		_inputScroll(window, -((SHORT)HIWORD(wParam) / (double)WHEEL_DELTA), 0.0);
		return 0;
	}
	case WM_ACTIVATEAPP:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}
void _inputKey(GWindow* window, int key, int scancode, int action, int mods)
{
	if (key >= 0 && key <= GLU_KEY_LAST)
	{
		bool repeated = false;
		if (action == GLU_RELEASE && window->keys_[key] == GLU_RELEASE)
			return;

		if (action == GLU_PRESS && window->keys_[key] == GLU_PRESS)
			repeated = true;
		
		window->keys_[key] = (char)action;

		if (repeated)
			action = GLU_REPEAT;
	}

	if(window->callbacks_.key_)
		window->callbacks_.key_((GLUwindow*)window, key, scancode, action, mods);
}
void defaultWindowHints(void)
{
	opg.hints.framebuffer_.redBits = 8;
	opg.hints.framebuffer_.greenBits = 8;
	opg.hints.framebuffer_.blueBits = 8;
	opg.hints.framebuffer_.alphaBits = 8;
	opg.hints.framebuffer_.depthBits = 24;
	opg.hints.framebuffer_.stencilBits = 8;
	opg.hints.framebuffer_.doublebuffer = true;

	opg.hints.context_.glMaxJor_ = 3.0;
	opg.hints.context_.glMinJor_ = 3.0;

	opg.hints.window_.visible_ = true;
	opg.hints.window_.gui_ = false;
}

 
bool createNativeWindow( GWindow * win, 
	const char *window_name,
	int width, int height)
{

	int xpos, ypos;
	xpos = 50;
	ypos = 50;

	win->win_.handle_ = CreateWindowEx(
		0,                        // Optional window styles.
		_WNDCLASSNAME,                     // Window class
		window_name,					// Window text
		WS_OVERLAPPEDWINDOW,            // Window style
		xpos,							//// Size and position			
		ypos, 
		width, 
		height,
		NULL,       // Parent window    
		NULL,       // Menu
		GetModuleHandle(NULL),  // Instance handle
		NULL        // Additional application data
	);

	if (win->win_.handle_ == 0)
	{
		PRINT_ERROR("CreateWindow failed!");
		return false;
	}

	SetProp(win->win_.handle_, "GLU", win);

	return true;
}


void platformGetCursorPos(GWindow* window, double* xpos, double* ypos)
{
	POINT pos;
	if (GetCursorPos(&pos))
	{
		ScreenToClient(window->win_.handle_, &pos);
		if (xpos)
			*xpos = pos.x;
		if (ypos)
			*ypos = pos.y;
	}
}

void PlatformSetCursorPos(GWindow* window, double xpos, double ypos)
{
	POINT pos = { (int)xpos, (int)ypos };

	// Store the new position so it can be recognized later
	window->win_.lastCursorPosX_ = pos.x;
	window->win_.lastCursorPosY_ = pos.y;

	ClientToScreen(window->win_.handle_, &pos);
	SetCursorPos(pos.x, pos.y);
}
bool platformRefreshContextAttribs(GWindow* window, const Context_Config* ctxconfig)
{
	return true;
}

void platformShowWindow(GWindow* window)
{
	ShowWindow(window->win_.handle_, SW_SHOWNA);
}

void PlatformDestroyWindow(GWindow*window)
{

	if (window->context_.destroy_)
		window->context_.destroy_(&window->context_);

	if(window->sharedContext_)
		if(window->sharedContext_->destroy_)
			window->sharedContext_->destroy_(window->sharedContext_);

	if (window->win_.handle_)
	{
		RemoveProp(window->win_.handle_, "GLU");
		DestroyWindow(window->win_.handle_);
		window->win_.handle_ = NULL;
	}

	if (window->win_.bigIcon_)
		DestroyIcon(window->win_.bigIcon_);

	if (window->win_.smallIcon_)
		DestroyIcon(window->win_.smallIcon_);
}

void platformPollEvents()
{
	MSG msg;
	HWND handle;
	GWindow* window;

	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			// NOTE: While GLFW does not itself post WM_QUIT, other processes
			//       may post it to this one, for example Task Manager
			// HACK: Treat WM_QUIT as a close on all windows

			window = opg.windowListHead_;
			while (window)
			{
				_inputWindowCloseRequest(window);
				window = window->next_;
			}
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	handle = GetActiveWindow();
	if (handle)
	{
		window = (GWindow*)GetPropW(handle, L"GLU");
		if (window)
		{
			int i;
			const int keys[4][2] =
			{
				{ VK_LSHIFT, GLU_KEY_LEFT_SHIFT },
				{ VK_RSHIFT, GLU_KEY_RIGHT_SHIFT },
				{ VK_LWIN, GLU_KEY_LEFT_SUPER },
				{ VK_RWIN, GLU_KEY_RIGHT_SUPER }
			};

			for (i = 0; i < 4; i++)
			{
				const int vk = keys[i][0];
				const int key = keys[i][1];
				const int scancode = opg.win32_.scancodes[key];

				if ((GetKeyState(vk) & 0x8000))
					continue;
				if (window->keys_[key] != GLU_PRESS)
					continue;

				_inputKey(window, key, scancode, GLU_RELEASE, getKeyMods());
			}
		}
	}
	
}

void platformFocusWindow(GWindow* window)
{
	BringWindowToTop(window->win_.handle_);
	SetForegroundWindow(window->win_.handle_);
	SetFocus(window->win_.handle_);
}

int platformCreateWindow(GWindow* window,
	const WindowConfig* wndconfig,
	const Context_Config* ctxconfig,
	const Bconfig* fbconfig)
{
	if (!createNativeWindow(window, wndconfig->title_, wndconfig->width_, wndconfig->height_))
	{
		delete window;
		return NULL;
	}

	if (!_initWGL())
	{
		delete window;
		return NULL;
	}

	if (_createContextWGL(window, ctxconfig, fbconfig))
	{
		if (wndconfig->shouldCreateSharedContext_)
		{
			window->sharedContext_ = new Context;
			window->sharedContext_->wgl.dc_ = window->context_.wgl.dc_;

			if (!__createContextWGL(&window->win_.handle_, window->sharedContext_, &window->context_.wgl.handle_, ctxconfig, fbconfig, true))
			{
				gluDestoryWindow((GLUwindow*)window);
				return NULL;
			}
		}
	}
	else
	{
		gluDestoryWindow((GLUwindow*)window);
		return NULL;
	}
	if (!platformRefreshContextAttribs(window, ctxconfig))
	{
		gluDestoryWindow((GLUwindow*)window);
		return NULL;
	}
}


GWindow::~GWindow()
{
	if (sharedContext_) {
		delete sharedContext_;
		sharedContext_ = nullptr;
	}
}

int  PlatformGetKeyScancode(int key)
{
	return opg.win32_.scancodes[key];
}
void PlatformSetClipboardString(const char* string)
{
	int characterCount;
	HANDLE object;
	WCHAR* buffer;

	characterCount = MultiByteToWideChar(CP_UTF8, 0, string, -1, NULL, 0);
	if (!characterCount)
		return;

	object = GlobalAlloc(GMEM_MOVEABLE, characterCount * sizeof(WCHAR));
	if (!object)
	{
		
		PRINT_ERROR("Win32: Failed to allocate global handle for clipboard");
		return;
	}

	buffer = (WCHAR*)GlobalLock(object);
	if (!buffer)
	{
		PRINT_ERROR(
			"Win32: Failed to lock global handle");
		GlobalFree(object);
		return;
	}

	MultiByteToWideChar(CP_UTF8, 0, string, -1, buffer, characterCount);
	GlobalUnlock(object);

	if (!OpenClipboard(opg.win32_.helperWindowHandle_))
	{
		PRINT_ERROR(
			"Win32: Failed to open clipboard");
		GlobalFree(object);
		return;
	}

	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, object);
	CloseClipboard();
}

const char* PlatformGetClipboardString(void)
{
	HANDLE object;
	WCHAR* buffer;

	if (!OpenClipboard(opg.win32_.helperWindowHandle_))
	{
		PRINT_ERROR(
			"Win32: Failed to open clipboard");
		return NULL;
	}

	object = GetClipboardData(CF_UNICODETEXT);
	if (!object)
	{
		PRINT_ERROR(
			"Win32: Failed to convert clipboard to string");
		CloseClipboard();
		return NULL;
	}

	buffer = (WCHAR*)GlobalLock(object);
	if (!buffer)
	{
		PRINT_ERROR(
			"Win32: Failed to lock global handle");
		CloseClipboard();
		return NULL;
	}

	free(opg.win32_.clipboardString_);
	opg.win32_.clipboardString_ = createUTF8FromWideStringWin32(buffer);

	GlobalUnlock(object);
	CloseClipboard();

	return opg.win32_.clipboardString_;
}

int PlatformCreateStandardCursor(GWcursor* cursor, int shape)
{
	int id = 0;

	if (shape == GLU_ARROW_CURSOR)
		id = OCR_NORMAL;
	else if (shape == GLU_IBEAM_CURSOR)
		id = OCR_IBEAM;
	else if (shape == GLU_CROSSHAIR_CURSOR)
		id = OCR_CROSS;
	else if (shape == GLU_POINTING_HAND_CURSOR)
		id = OCR_HAND;
	else if (shape == GLU_RESIZE_EW_CURSOR)
		id = OCR_SIZEWE;
	else if (shape == GLU_RESIZE_NS_CURSOR)
		id = OCR_SIZENS;
	else if (shape == GLU_RESIZE_NWSE_CURSOR)
		id = OCR_SIZENWSE;
	else if (shape == GLU_RESIZE_NESW_CURSOR)
		id = OCR_SIZENESW;
	else if (shape == GLU_RESIZE_ALL_CURSOR)
		id = OCR_SIZEALL;
	else if (shape == GLU_NOT_ALLOWED_CURSOR)
		id = OCR_NO;
	else
	{
		PRINT_ERROR( "Win32: Unknown standard cursor");
		return false;
	}

	cursor->win_.handle_ = (HCURSOR)LoadImageW(NULL,
		MAKEINTRESOURCEW(id), IMAGE_CURSOR, 0, 0,
		LR_DEFAULTSIZE | LR_SHARED);
	if (!cursor->win_.handle_)
	{
		PRINT_ERROR(
			"Win32: Failed to create standard cursor");
		return false;
	}

	return false;
}

static bool cursorInContentArea(GWindow* window)
{
	RECT area;
	POINT pos;

	if (!GetCursorPos(&pos))
		return false;

	if (WindowFromPoint(pos) != window->win_.handle_)
		return false;

	GetClientRect(window->win_.handle_, &area);
	ClientToScreen(window->win_.handle_, (POINT*)&area.left);
	ClientToScreen(window->win_.handle_, (POINT*)&area.right);

	return PtInRect(&area, pos);
}

static void updateCursorImage(GWindow* window)
{
	SetCursor(NULL);
}

void PlatformDestroyCursor(GWcursor* cursor)
{
	if (cursor->win_.handle_)
		DestroyIcon((HICON)cursor->win_.handle_);
}

void PlatformSetCursor(GWindow* window, GWcursor* cursor)
{
	if (cursorInContentArea(window))
		updateCursorImage(window);
}


void*		gluGetWin32Window(GLUwindow*handle)
{
	GWindow* window = (GWindow*)handle;
	return window->win_.handle_;
}
void PlatformGetWindowSize(GWindow* window, int* width, int* height)
{
	RECT area;
	GetClientRect(window->win_.handle_, &area);

	if (width)
		*width = area.right;
	if (height)
		*height = area.bottom;
}

void PlatformGetFramebufferSize(GWindow* window, int* width, int* height)
{
	PlatformGetWindowSize(window, width, height);
}

int PlatformWindowFocused(GWindow* window)
{
	return window->win_.handle_ == GetActiveWindow();
}

int PlatformWindowIconified(GWindow* window)
{
	return IsIconic(window->win_.handle_);
}

int PlatformWindowVisible(GWindow* window)
{
	return IsWindowVisible(window->win_.handle_);
}

int PlatformWindowMaximized(GWindow* window)
{
	return IsZoomed(window->win_.handle_);
}

int PlatformWindowHovered(GWindow* window)
{
	return cursorInContentArea(window);
}
