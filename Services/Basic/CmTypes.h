//////////////////////////////////////////////////////////////////////////////
//
// CmTypes.h - Declaration of basic type and diagnostics classes
//
//////////////////////////////////////////////////////////////////////////////
//
// author: 	   Eckhard Kantz
// website:    http://wegalink.eu
//
//////////////////////////////////////////////////////////////////////////////
/* 
This is FREE software  

Permission is hereby granted, free of charge,  to any person obtaining  a copy 
of this software and associated documentation files (the "Software"),  to deal 
in the Software without restriction, including without limitation  the  rights 
to use,  copy,  modify,  merge,  publish,  distribute, sublicense, and/or sell 
copies  of  the  Software,   and  to  permit  persons  to  whom  the  Software 
is furnished to do so, subject to the following conditions: 

There are no conditions imposed on the use of this software. 

THE SOFTWARE IS PROVIDED "AS IS",  WITHOUT  WARRANTY  OF ANY KIND,  EXPRESS OR 
IMPLIED,  INCLUDING  BUT  NOT  LIMITED  TO  THE  WARRANTIES OF MERCHANTABILITY, 
FITNESS  FOR  A  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS  OR  COPYRIGHT  HOLDERS  BE  LIABLE  FOR  ANY CLAIM,  DAMAGES OR OTHER 
LIABILITY,  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,  ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN  THE 
SOFTWARE. 
*/

/* ToDo: possible improvements
*  - explizit type conversion double(), int32(),...
*  - pointer overloading operator -> for "intelligent pointer"
*  see: http://public.beuth-hochschule.de/~kempfer/skript_cpp/Kap11.html
*/



#ifndef CmTypesH
#define CmTypesH

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <sys/timeb.h>
#include <math.h>
#include <share.h>

// Include all return values that apply for Cm namespace
#include "Services/Basic/CmReturn.h"

#ifdef UNIX
//------------Unix------------
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/param.h>
#define MKDIR(path,mask) mkdir(path,mask)
#define GETCWD(buf,len)  getcwd(buf,len)
#define OPEN(buf,mode,access) open(buf,mode,access)
//------------Unix: 32-bit/64-bit compatibility------------
typedef __int64_t     int64;
typedef __uint64_t    uint64;
typedef __int32_t     int32;
typedef __uint32_t    uint32;
typedef __int16_t     int16;
typedef __uint16_t    uint16;
typedef char          int8;
typedef unsigned char uint8;
#define abs64	llabs
#else

//============================ Windows =========================================
//#include <windows.h>
#include <io.h>
#include <direct.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

//---Common MVC and BVC
#define MAXPATHLEN 1024
#define GETCWD(buf,len) GetCurrentDirectoryA(len,buf)

//--------- Borland C++ 6.0 -----------------
#ifdef __BORLANDC__

#define MKDIR(path,mask) mkdir(path)
#define OPEN(buf,mode,access) _open(buf,mode)
#define WRITE(fp,data,length) _write(fp,data,length)

#define open open
#define ftime ftime
#define max(a, b)  (((a) > (b)) ? (a) : (b))  
#define min(a, b)  (((a) < (b)) ? (a) : (b))

// 2007-01-21/EKantz: problem with non-existing abs64/llabs in BORLANDC
#define abs64 llabs

//--------- Microsoft Visual C++ -------------
#else

#define open  _open
#define read	_read
#define close _close
#define fstat _fstat
#define stat  _stat
#define ftime _ftime
#define abs64	_abs64
#define chdir _chdir

#ifdef O_RDONLY
  #undef O_RDONLY
#endif
#define O_RDONLY   _O_RDONLY
#define timeb	   _timeb
#define MKDIR(path,mask) _mkdir(path)
#define OPEN(buf,mode,access) _open(buf,mode)
#define WRITE(fp,data,length) _write(fp,data,length)

// Define all unix access masks
#define __S_IREAD       0400    /* Read by owner.  */
#define __S_IWRITE      0200    /* Write by owner.  */
#define __S_IEXEC       0100    /* Execute by owner.  */
#if defined __USE_MISC && defined __USE_BSD
# define S_IREAD        S_IRUSR
# define S_IWRITE       S_IWUSR
# define S_IEXEC        S_IXUSR
#endif
#define S_IRGRP (S_IRUSR >> 3)  /* Read by group.  */
#define S_IWGRP (S_IWUSR >> 3)  /* Write by group.  */
#define S_IXGRP (S_IXUSR >> 3)  /* Execute by group.  */
/* Read, write, and execute by group.  */
#define S_IRWXG (S_IRWXU >> 3)
#define S_IROTH (S_IRGRP >> 3)  /* Read by others.  */
#define S_IWOTH (S_IWGRP >> 3)  /* Write by others.  */
#define S_IXOTH (S_IXGRP >> 3)  /* Execute by others.  */
/* Read, write, and execute by others.  */
#define S_IRWXO (S_IRWXG >> 3)
/* file mask */
#define S_MASK  S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH

#endif //#ifdef __BORLANDC__

//------------Win: 32-bit/64-bit compatibility------------
typedef __int64            int64;
typedef unsigned __int64   uint64;
typedef __int32            int32;
typedef unsigned int       uint32;
typedef __int16            int16;
typedef unsigned __int16   uint16;
typedef char               int8;
typedef unsigned char      uint8;

#endif // #ifndef WIN32


//----------MSVS: Avoid safe functions warnings------------
#ifdef MSVS
	#define MEMCPY(a,b,c,d) memcpy_s(a,b,c,d)
	#define STRCPY(a,b,c) strcpy_s(a,b,c)
	#define STRNCPY(a,b,c,d) strncpy_s(a,b,c,d)
	#define SPRINTF3(a,b,c) sprintf_s(a,b,c)
  #define SPRINTF4(a,b,c,d) sprintf_s(a,b,c,d)
  #define SPRINTF5(a,b,c,d,e) sprintf_s(a,b,c,d,e)
  #define SPRINTF6(a,b,c,d,e,f) sprintf_s(a,b,c,d,e,f)
  #define SPRINTF7(a,b,c,d,e,f,g) sprintf_s(a,b,c,d,e,f,g)
  #define SPRINTF8(a,b,c,d,e,f,g,h) sprintf_s(a,b,c,d,e,f,g,h)
  #define SPRINTF9(a,b,c,d,e,f,g,h,i) sprintf_s(a,b,c,d,e,f,g,h,i)
#else
  #define STRCPY(a,b,c)  strcpy(a,c)
  #define STRNCPY(a,b,c,d)  strncpy(a,c,d)
  #define SPRINTF3(a,b,c) sprintf(a,c)
  #define SPRINTF4(a,b,c,d) sprintf(a,c,d)
  #define SPRINTF5(a,b,c,d,e) sprintf(a,c,d,e)
  #define SPRINTF6(a,b,c,d,e,f) sprintf(a,c,d,e,f)
  #define SPRINTF7(a,b,c,d,e,f,g) sprintf(a,c,d,e,f,g)
  #define SPRINTF8(a,b,c,d,e,f,g,h) sprintf(a,c,d,e,f,g,h)
  #define SPRINTF9(a,b,c,d,e,f,g,h,i) sprintf(a,c,d,e,f,g,h,i)
#endif  // #ifdef MSVS


//--------------------DEV C++ compatibility----------------
#ifdef gcc
  #define MIN_INT64         0x8000000000000000ull
  #define MAX_INT64         0x7FFFFFFFFFFFFFFFull
  #define MAX_UINT64        0xFFFFFFFFFFFFFFFFull
#else
  #define MIN_INT64         0x8000000000000000
  #define MAX_INT64         0x7FFFFFFFFFFFFFFF
  #define MAX_UINT64        0xFFFFFFFFFFFFFFFF
#endif // #ifdef gcc


#define MIN_INT16           0x8000
#define MAX_INT16           0x7FFF
#define MAX_UINT16	        0xFFFF

#define MIN_INT32           0x80000000
#define MAX_INT32           0x7FFFFFFF
#define MAX_UINT32	        0xFFFFFFFF

#define LO64(x)		        (uint32)(x&0xFFFFFFFF)
#define HI64(x)		        (uint32)(x>>32)

#define MIN_DOUBLE          1.7E-308
#define MAX_DOUBLE          1.7E+308

#define PI						3.141592653589793238462643383279502884197169399375105820974944592

namespace Cosmos
{
	/** CmFlex.
	*   A union that holds all data formats
	*/
	typedef union{
		uint64 uint64Value;
		int64 int64Value;
		double doubleValue;
		uint32 uint32Value;
		int32 int32Value;
		float floatValue;
		uint16 uint16Value;
		int16 int16Value;
		uint8 uint8Value;
		int8 int8Value;
		bool boolValue;
	} CmFlex;


	/** CmXY
	 *  This class represents a two dimensional size value consisting of a width and
	 *  a height value (or x and y).
	 */
	template <typename T>
	class CmXY
	{
		T _x;
		T _y;

	public:
		CmXY() :_x(0), _y(0){};
		CmXY(T const& x, T const& y) :_x(x), _y(y){}
		CmXY(double const& x, double const& y) :_x((T)x), _y((T)y){}
		CmXY(T const& t) :_x(t.x), _y(t.y){}
		~CmXY(){};

	public:
		// access functions
		T& x(){ return _x; }
		T& y(){ return _y; }
		T x()const{ return _x; }
		T y()const{ return _y; }
		T w()const{ return _x; }
		T h()const{ return _y; }
		T P1()const{ return _x; }
		T P2()const{ return _y; }
		T begin()const{ return _x; }
		T end()const{ return _y; }

		void operator=(CmXY const& t)
		{
			if (this != &t)
			{
				_x = t._x;
				_y = t._y;
			}
		}
		CmXY operator+(CmXY const& t)
		{
			CmXY sum;
			sum.x() = _x + t._x;
			sum.y() = _y + t._y;
			return sum;
		}
		void operator+=(CmXY const& t)
		{
			_x += t._x;
			_y += t._y;
		}
		void operator+=(float s)
		{
			_x += s;
			_y += s;
		}
	};

	typedef CmXY<int32> CmSize;
	typedef CmXY<int32> CmPoint;
	typedef CmXY<float> CmSize2D;
	typedef CmXY<float> CmPoint2D;
	typedef CmXY<float> CmRange;
	typedef CmXY<CmPoint2D> CmLine2D;
	typedef CmXY<CmPoint2D> CmRectangle2D;

	/** CmSmoothed.
	*  A series of values will be smoothed according to a smoothing factor.
	*  The values will be updated directly whenever a threshold is exceeded.
	*/
	class CmSmoothed
	{
#define CMSMOOTHED_THRESHOLD_DEFAULT		0.8
#define CMSMOOTHED_FACTOR_DEFAULT				1.0
	public:
		// constructor
		CmSmoothed(double _StartValue =0);
		// desctructor
		~CmSmoothed();
	
	public:
		/** smooth.
		*   A new value will be processed for smoothing
		*/
		double smooth(double _NewValue);
		double smooth(int32 NewValue);
	
	public:
		// control parameters
		double Threshold;
		double Factor;

	public:
		// state
		double Value;
	};

}  // namespace Cosmos

using namespace Cosmos;

#endif // ifndef CmTypesH
