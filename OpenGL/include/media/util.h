
#ifdef FMOD_HAVE
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#endif

#ifdef FMOD_HAVE

#ifndef Common_Sin
#define Common_Sin sin
#endif

#ifndef Common_snprintf
#define Common_snprintf snprintf
#endif

#ifndef Common_vsnprintf
#define Common_vsnprintf vsnprintf
#endif

#define NUM_COLUMNS 50
#define NUM_ROWS 25



FMOD_RESULT F_CALLBACK systemcallback(FMOD_SYSTEM *system, FMOD_SYSTEM_CALLBACK_TYPE type,
                                        void *commanddata1, void *commanddata2, void* userdata);
#endif

#define ENGINE_FORBID_COPY(Class) \
    private: \
        Class(const Class &); \
        void operator=(const Class &); \
    public:

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

#ifdef WIN32
#define SNPRINTF _snprintf_s
#define VSNPRINTF vsnprintf_s
#define RANDOM rand
#define SRANDOM srand((unsigned)time(NULL))
#else
#define SNPRINTF snprintf
#define VSNPRINTF vsnprintf
#define RANDOM random
#define SRANDOM srandom(getpid())
#endif

#define F32NAN           (0x7F800001)                         // Produces rock solid fp-exceptions.
#if defined(__GNUC__)
#define F64NAN          (0x7FF0000000000001ULL)       //!< Produces rock solid fp-exceptions.
#else
#define F64NAN          (0x7FF0000000000001)        //!< Produces rock solid fp-exceptions.
#endif

#define FVF_NONE             0x000;
#define FVF_XYZ              0x001
#define FVF_NORMAL           0x002
#define FVF_TEXT0            0x004
#define FVF_TANGENT          0x008
#define FVF_BITANGENT        0x010
#define SQR( number )			( number*number )


#define DRAW_ARRAYS							0 //gldrawarrays glmultidrawarrays
#define DRAW_ARRAYS_INDIRECT				1 //glMultiDrawArraysIndirect 
#define DRAW_ARRAYS_INSTANC					2 //glDrawArraysInstancedBaseInstance
#define DRAW_ELEMENTS	    				3 //gldrawelement glmultidrawelement
#define DRAW_ELEMENTS_BASE_VERTEX			4 //glMultiDrawElementsBaseVertex  glDrawElementBaseVertex
#define DRAW_ELEMENT_INSTANCE_BASE_VERTEX	5 // //glDrawElementsInstancedBaseVertex
#define DRAW_ELEMENT_INSTANCE_BASE_INSTANCE	6  //glDrawElementsInstancedBaseInstance
#define DRAW_ELEMENT_INDIRECT			    7  //glMultiDrawElementsIndirect 

#define  PI 3.14159265358979323846264338327950288
#define  PI_2  1.57079632679485
#define  PI_180 PI / (180.0)
#define  INFINITY std::numeric_limits<float>::infinity()
#define  EPSILION  std::numeric_limits<float>::epsilon() 
#define  NEAR_EPSILION 10e-5
#define  MAX_VALUE std::numeric_limits<float>::max(£©
#define  MIN_VALUE std::numeric_limits<float>::min()