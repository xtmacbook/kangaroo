

#include "base/timer.h"
#include <stdlib.h>

    ///////////////////////////////////////////////////////////////////////////////
    // constructor
    ///////////////////////////////////////////////////////////////////////////////
Timer::Timer()
{
#ifdef EN_OS_WIN32
    QueryPerformanceFrequency(&frequency_);
    startCount_.QuadPart = 0;
    endCount_.QuadPart = 0;
#else
    startCount_.tv_sec = startCount_.tv_usec = 0;
    endCount_.tv_sec = endCount_.tv_usec = 0;
#endif
    
    stopped_ = 0;
    startTimeInMicroSec_ = 0;
    endTimeInMicroSec_ = 0;
}



    ///////////////////////////////////////////////////////////////////////////////
    // distructor
    ///////////////////////////////////////////////////////////////////////////////
Timer::~Timer()
{
}



    ///////////////////////////////////////////////////////////////////////////////
    // start timer.
    // startCount will be set at this point.
    ///////////////////////////////////////////////////////////////////////////////
void Timer::start()
{
    stopped_ = 0; // reset stop flag
#ifdef EN_OS_WIN32
    QueryPerformanceCounter(&startCount_);
#else
    gettimeofday(&startCount_, NULL);
#endif
}



    ///////////////////////////////////////////////////////////////////////////////
    // stop the timer.
    // endCount will be set at this point.
    ///////////////////////////////////////////////////////////////////////////////
void Timer::stop()
{
    stopped_ = 1; // set timer stopped flag
    
#ifdef EN_OS_WIN32
    QueryPerformanceCounter(&endCount_);
#else
    gettimeofday(&endCount_, NULL);
#endif
}



    ///////////////////////////////////////////////////////////////////////////////
    // compute elapsed time in micro-second resolution.
    // other getElapsedTime will call this first, then convert to correspond resolution.
    ///////////////////////////////////////////////////////////////////////////////
double Timer::getElapsedTimeInMicroSec()
{
#ifdef EN_OS_WIN32
    if (!stopped_)
        QueryPerformanceCounter(&endCount_);
    
    startTimeInMicroSec_ = startCount_.QuadPart * (1000000.0 / frequency_.QuadPart);
    endTimeInMicroSec_ = endCount_.QuadPart * (1000000.0 / frequency_.QuadPart);
#else
    if (!stopped_)
        gettimeofday(&endCount_, NULL);
    
    startTimeInMicroSec_ = (startCount_.tv_sec * 1000000.0) + startCount_.tv_usec;
    endTimeInMicroSec_ = (endCount_.tv_sec * 1000000.0) + endCount_.tv_usec;
#endif
    
    return endTimeInMicroSec_ - startTimeInMicroSec_;
}



    ///////////////////////////////////////////////////////////////////////////////
    // divide elapsedTimeInMicroSec by 1000
    ///////////////////////////////////////////////////////////////////////////////
double Timer::getElapsedTimeInMilliSec()
{
    return this->getElapsedTimeInMicroSec() * 0.001;
}



    ///////////////////////////////////////////////////////////////////////////////
    // divide elapsedTimeInMicroSec by 1000000
    ///////////////////////////////////////////////////////////////////////////////
double Timer::getElapsedTimeInSec()
{
    return this->getElapsedTimeInMicroSec() * 0.000001;
}

    ///////////////////////////////////////////////////////////////////////////////
    // same as getElapsedTimeInSec()
    ///////////////////////////////////////////////////////////////////////////////
double Timer::getElapsedTime()
{
    return this->getElapsedTimeInSec();
}
