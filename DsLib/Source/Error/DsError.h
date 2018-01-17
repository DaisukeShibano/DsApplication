#ifndef _DS_ERROR_H_
#define _DS_ERROR_H_

namespace DsLib
{

//#define DS_ASSERT( flag, ex ) ( (flag) ? (flag) : assert( (flag) && (ex) ) )
#ifdef _DEBUG
#define DS_ASSERTW( flag, ex, ...)\
	{\
		if(!(flag)){\
			wchar_t c[1024]; \
			int num = swprintf_s(c, 1023, ex, __VA_ARGS__); \
			c[num] = L'\n'; \
			c[num + 1] = L'\0'; \
			_wassert(c, _CRT_WIDE(__FILE__), __LINE__);\
		}\
	}\

#define DS_ASSERT( flag, ex, ...) DS_ASSERTW(flag, L#ex, __VA_ARGS__)

#else
#define DS_ASSERT( flag, ex, ...)
#endif



#define DS_ERROR( mes, ... ) DS_ASSERT(false, mes, __VA_ARGS__ )

#define DS_LOG( str, ... ) \
      { \
        char c[1024]; \
        int num = sprintf_s( c, 1023, str, __VA_ARGS__ ); \
		c[num]='\n'; \
		c[num+1]='\0'; \
		OutputDebugStringA( c ); \
      }\

}

#endif