#ifndef _DS_ERROR_H_
#define _DS_ERROR_H_

namespace DsLib
{

//#define DS_ASSERT( flag, ex ) ( (flag) ? (flag) : assert( (flag) && (ex) ) )
#ifdef _DEBUG
#define DS_ASSERT( flag, ex, ...) ( (flag) ? (flag) : DsError::Asset( ex ) )
#else
#define DS_ASSERT( flag, ex, ...)
#endif
//#define DS_ASSERT( flag, ex ) ( assert(flag && ex) )
#define DS_ERROR( mes, ... ) (DsError::Asset( mes ))

#define DS_LOG( str, ... ) \
      { \
        char c[1024]; \
        int num = sprintf_s( c, 1023, str, __VA_ARGS__ ); \
		c[num]='\n'; \
		c[num+1]='\0'; \
		OutputDebugStringA( c ); \
      }

	class DsError
	{
	public:
		DsError();
		virtual ~DsError();

	public:
		static void Panic(const char* format, ...);
		static void Asset( const char* format, ... );
	};
}

#endif