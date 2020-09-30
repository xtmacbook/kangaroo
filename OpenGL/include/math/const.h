
#ifndef _OPENGL_CONST_H
#define _OPENGL_CONST_H

#include <limits> 
namespace math
{

	template<class REAL>
	struct  Const
	{
		typedef REAL Real;
		typedef const REAL cReal;

		static inline const REAL pi() throw() { return REAL(3.141592653589793238462643383279502884197169399375105820974944592); }
		static inline const REAL tpi() throw() { return REAL(6.28318530718f); }
		static inline const REAL pi_2() throw() { return pi() / REAL(2.0); }
		static inline const REAL pi_180() throw() { return pi() / REAL(180.0); }
		static inline const REAL c180_pi() throw() { return REAL(180.0) / pi(); }
		static inline const REAL infinity() throw() { return std::numeric_limits<REAL>::infinity(); }
		static inline const REAL epsilon() throw() { return std::numeric_limits<REAL>::epsilon(); }
		static inline const REAL near_epsilon() throw() { return REAL(10e-5); }
		static inline const REAL zero() throw() { return REAL(0.0); }
		static inline const REAL max_value() throw() { return std::numeric_limits<REAL>::max(); }
		static inline const REAL min_value() throw() { return std::numeric_limits<REAL>::min(); }
	};

#define PI 3.14159265358979323846264338327950288
#define F32NAN           (0x7F800001)                         // Produces rock solid fp-exceptions.
#if defined(__GNUC__)
#define F64NAN          (0x7FF0000000000001ULL)       //!< Produces rock solid fp-exceptions.
#else
#define F64NAN          (0x7FF0000000000001)        //!< Produces rock solid fp-exceptions.
#endif
}
#endif
