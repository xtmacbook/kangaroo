#include "util.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <assert.h>

#ifdef FMOD_HAVE




FMOD_RESULT F_CALLBACK systemcallback(FMOD_SYSTEM *system, FMOD_SYSTEM_CALLBACK_TYPE type, void *commanddata1,
                                      void *commanddata2, void* userdata)
{
    FMOD::System *sys = (FMOD::System *)system;

    switch (type)
    {
        case FMOD_SYSTEM_CALLBACK_DEVICELISTCHANGED:
        {
        int numdrivers;

        printf("NOTE : FMOD_SYSTEM_CALLBACK_DEVICELISTCHANGED occurred.\n");

        sys->getNumDrivers(&numdrivers);

        printf("Numdevices = %d\n", numdrivers);
        break;
        }
        case FMOD_SYSTEM_CALLBACK_MEMORYALLOCATIONFAILED:
        {
        printf("ERROR : FMOD_SYSTEM_CALLBACK_MEMORYALLOCATIONFAILED occurred.\n");
        printf("%s.\n", commanddata1);
        printf("%d bytes.\n", commanddata2);
        break;
        }
        case FMOD_SYSTEM_CALLBACK_THREADCREATED:
        {
        printf("NOTE : FMOD_SYSTEM_CALLBACK_THREADCREATED occurred.\n");
        printf("Thread ID = %d\n", commanddata1);
        printf("Thread Name = %s\n", (char *)commanddata2);
        break;
        }
        case FMOD_SYSTEM_CALLBACK_THREADDESTROYED:
        {
        printf("NOTE : FMOD_SYSTEM_CALLBACK_THREADDESTROYED occurred.\n");
        printf("Thread ID = %d\n", commanddata1);
        printf("Thread Name = %s\n", (char *)commanddata2);
        break;
        }
        case FMOD_SYSTEM_CALLBACK_BADDSPCONNECTION:
        {
        FMOD::DSP *source = (FMOD::DSP *)commanddata1;
        FMOD::DSP *dest = (FMOD::DSP *)commanddata2;

        printf("ERROR : FMOD_SYSTEM_CALLBACK_BADDSPCONNECTION occurred.\n");
        if (source)
            {
            char name[256];
            source->getInfo(name, 0,0,0,0);
            printf("SOURCE = %s\n", name);
            }
        if (dest)
            {
            char name[256];
            dest->getInfo(name, 0,0,0,0);
            printf("DEST = %s\n", name);
            }
        break;
        }
        case FMOD_SYSTEM_CALLBACK_PREMIX:
        {
        printf("NOTE : FMOD_SYSTEM_CALLBACK_PREMIX occurred.\n");
        break;
        }
        case FMOD_SYSTEM_CALLBACK_MIDMIX:
        {
        printf("NOTE : FMOD_SYSTEM_CALLBACK_MIDMIX occurred.\n");
        break;
        }
        case FMOD_SYSTEM_CALLBACK_POSTMIX:
        {
        printf("NOTE : FMOD_SYSTEM_CALLBACK_POSTMIX occurred.\n");
        break;
        }
        case FMOD_SYSTEM_CALLBACK_ERROR:
        {
        FMOD_ERRORCALLBACK_INFO* info = (FMOD_ERRORCALLBACK_INFO*)commanddata1;
        printf("NOTE : FMOD_SYSTEM_CALLBACK_ERROR occurred.\n");
        printf("  ERROR (%d) from %s(%s) with instance %p (type %d)\n", info->result, info->functionname, info->functionparams, info->instance, info->instancetype);
        break;
        }
    }

    return FMOD_OK;
}

#endif