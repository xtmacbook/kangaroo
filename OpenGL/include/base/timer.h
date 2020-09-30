//
//  timer.h
//  openGLTest
//
//  Created by xt on 18/7/6.
//  Copyright © 2018年 xt. All rights reserved.
//

#ifndef timer_h
#define timer_h

#include "sys.h"

#ifdef EN_OS_WIN32   // Windows system specific
#include <windows.h>
#else          // Unix based system specific
#include <sys/time.h>
#endif

class Timer
{
public:
    Timer();                                    // default constructor
    ~Timer();                                   // default destructor

    void									start();                             // start timer
    void									stop();                              // stop the timer
    double									getElapsedTime();                    // get elapsed time in second
    double									getElapsedTimeInSec();               // get elapsed time in second (same as getElapsedTime)
    double									getElapsedTimeInMilliSec();          // get elapsed time in milli-second
    double									getElapsedTimeInMicroSec();          // get elapsed time in micro-second


private:
    double									startTimeInMicroSec_;                 // starting time in micro-second
    double									endTimeInMicroSec_;                   // ending time in micro-second
    int										stopped_;                             // stop flag
#ifdef EN_OS_WIN32
    LARGE_INTEGER							frequency_;                    // ticks per second
    LARGE_INTEGER							startCount_;                   //
    LARGE_INTEGER							endCount_;                     //
#else
    timeval startCount_;                         //
    timeval endCount_;                           //
#endif
};

#endif /* timer_h */
