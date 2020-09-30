#include "thread.h"
#include "sys.h"
#include "log.h"
#include <assert.h>
#include <iostream>

#ifdef EN_OS_WIN32
#include <Windows.h>
#endif


Base::Thread::Thread()
	: _handle(0)
{}


Base::Thread::~Thread()
{
		CloseHandle(_handle);
}

void Base::Thread::start(const source_location &loc)
{
    _handle = CreateThread(
		0,
		0,
		(LPTHREAD_START_ROUTINE)Thread::thread_fnc,
		this,
		0,
		0);

	if(_handle==0) {
		PRINT_ERROR("create thread fail !\n");
	}
}

void Base::Thread::stop(const Base::source_location &loc)
{	
	loc;
}

void* Base::Thread::thread_fnc(void* data)
{
	try {
		reinterpret_cast<Thread*>(data)->run();
	}
	catch(const std::exception &e) {
		PRINT_ERROR(" thread start error !\n");
		return (void*)-1;
	}
	PostQuitMessage(-1);

	return 0;
}

bool Base::Thread::wait_for_end(const unsigned int time /* = -1 */)
{
	return WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)_handle, time);
}

void Base::Thread::terminate()
{
	TerminateThread((HANDLE)_handle, DWORD(-1));
}

bool Base::createTls(TlsWin32* tls)
{
	assert(tls->allocated_ == false);
	tls->index_ = TlsAlloc();
	if (tls->index_ == TLS_OUT_OF_INDEXES)
	{
		PRINT_ERROR("Win32: Failed to allocate TLS index");
		return false;
	}

	tls->allocated_ = true;
	return true;
}

bool Base::platformSetTls(TlsWin32* tls, void* value)
{
	assert(tls->allocated_ == true);
	TlsSetValue(tls->index_, value);
	return true;
}

void Base::destoryTls(TlsWin32*tls)
{
	if (tls->allocated_)
		TlsFree(tls->index_);
	memset(tls, 0, sizeof(TlsWin32));
}

void* Base::platformGetTls(TlsWin32* tls)
{
	assert(tls->allocated_ == true);
	return  TlsGetValue(tls->index_);
}
