#include "glinternal.h"
#include "win32_platform.h"
#include "log.h"

#pragma comment(lib, "winmm.lib ")
bool WindowsVersionOrGreaterWin32(WORD major, WORD minor, WORD sp)
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), major, minor, 0, 0,{ 0 }, sp };
	DWORD mask = VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR;
	ULONGLONG cond = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
	cond = VerSetConditionMask(cond, VER_MINORVERSION, VER_GREATER_EQUAL);
	cond = VerSetConditionMask(cond, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
	// HACK: Use RtlVerifyVersionInfo instead of VerifyVersionInfoW as the
	//       latter lies unless the user knew to embed a non-default manifest
	//       announcing support for Windows 10 via supportedOS GUID
	return RtlVerifyVersionInfo(&osvi, mask, cond) == 0;
}



static void freeLibraries(void)
{

	if (opg.win32_.dwmapi.instance_)
		FreeLibrary(opg.win32_.dwmapi.instance_);
	if (opg.win32_.ntdll.instance_)
		FreeLibrary(opg.win32_.ntdll.instance_);
}

void platformTerminate()
{
	if (opg.win32_.helperWindowHandle_)
		DestroyWindow(opg.win32_.helperWindowHandle_);
	unregisterWindowClassWin32();
	_terminateWgl();
	freeLibraries();
}

char* createUTF8FromWideStringWin32(const WCHAR* source)
{
	char* target;
	int size;

	size = WideCharToMultiByte(CP_UTF8, 0, source, -1, NULL, 0, NULL, NULL);
	if (!size)
	{
		PRINT_ERROR(
			"Win32: Failed to convert string to UTF-8");
		return NULL;
	}

	target = (char*)calloc(size, 1);

	if (!WideCharToMultiByte(CP_UTF8, 0, source, -1, target, size, NULL, NULL))
	{
		PRINT_ERROR(
			"Win32: Failed to convert string to UTF-8");
		free(target);
		return NULL;
	}

	return target;
}


uint64_t PlatformGetTimerValue(void)
{
	if (opg.timer_.win_.hasPC_)
	{
		uint64_t value;
		QueryPerformanceCounter((LARGE_INTEGER*)&value);
		return value;
	}
	else
		return (uint64_t)timeGetTime();
}

uint64_t PlatformGetTimerFrequency(void)
{
	return opg.timer_.win_.frequency_;
}

void					initTimerWin32(void)
{
	uint64_t frequency;

	if (QueryPerformanceFrequency((LARGE_INTEGER*)&frequency))
	{
		opg.timer_.win_.hasPC_ = true;
		opg.timer_.win_.frequency_ = frequency;
	}
	else
	{
		opg.timer_.win_.hasPC_ = false;
		opg.timer_.win_.frequency_ = 1000;
	}
}
