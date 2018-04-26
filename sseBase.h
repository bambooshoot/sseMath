#ifndef SSEBASE
#define SSEBASE

#include <windows.h>
#include <xmmintrin.h> 

#define BEGIN_SSE_MATH_NAME namespace sseMath {

#define MEM_ALIGN16 __declspec(align(16))
#define INLINE __forceinline

#define END_SSE_MATH_NAME }

#endif