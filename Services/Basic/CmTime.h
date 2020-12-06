//////////////////////////////////////////////////////////////////////////////
//
// CmTime.h - Declaration of CmTime classes
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

#ifndef CmTimeH
#define CmTimeH

#include "Services/Basic/CmTypes.h"
#include "Services/Basic/CmString.h"

namespace Cosmos
{

//The MDateTime class allows for storing and processing date and time values.
#define DATETIME_FEMTOSECONDS	uint64(1000*1000*1000*1000*1000)
#define DATETIME_PICOSECONDS	uint64(1000*1000*1000*1000)
#define DATETIME_NANOSECONDS	uint64(1000*1000*1000)
#define DATETIME_MICROSECONDS	uint64(1000*1000)
#define DATETIME_MILLISECONDS	uint64(1000)
// Timebase based on NANOSECONDS
#define TIMEBASE_SECONDS		(DATETIME_NANOSECONDS)
#define TIMEBASE_MILLISECONDS	(DATETIME_NANOSECONDS/1000)
#define TIMEBASE_MICROSECONDS	(DATETIME_NANOSECONDS/(1000*1000))
#define TIMEBASE_NANOSECONDS	1
#define TIMEBASE_PICOSECONDS	0.001
#define TIMEBASE_FEMTOSECONDS	0.000001
#define TIMEBASE_DAILY			(86400*DATETIME_NANOSECONDS)
// Seconds
#define SECONDS_PER_MINUTE		60
#define SECONDS_PER_HOUR		3600
#define SECONDS_PER_DAY			86400

class CmDateTime
{
public:
	// Unit test
	static bool testCmTime();

	// Systime access function (nanoseconds)
	static int64 getSysClockNanoSec(bool isPerformanceCounter =true);

	// Time conversion
	static int64  getNanoSec(const char* szDateTime);
	static CmString getTimeUTC(uint32 uDateTime=0, int32 nDayTime =-1, bool isFilename =false);

  // Timestamp, e.g. as component for filenames
	static CmString getTimestamp(int64 Timestamp, int32 TimeOffset =0, bool WithTime = false, bool WithMilliSec = false);

	// Sidereal time GMST in [pico deg]
	static uint64  getGMST(int64 nUTC);

	//Constructor and Destructor
	CmDateTime();
	virtual ~CmDateTime();

private:
#ifndef __BORLANDC__
#ifdef gcc
	static const uint64 u64J2000		= 0x0D2374199AEA8000ull;
	static const uint64 u64GMST_A		= 0x0000FF13D3880BD0ull;
	static const uint64 u64GMST_B		= 0x000148508547C092ull;
	static const uint64 u64GMST_M		= 0x0001476B081E8000ull;
#else
	static const uint64 u64J2000		= 0x0D2374199AEA8000;
	static const uint64 u64GMST_A		= 0x0000FF13D3880BD0;
	static const uint64 u64GMST_B		= 0x000148508547C092;
	static const uint64 u64GMST_M		= 0x0001476B081E8000;
#endif
	static const uint64 u64MilliArcSec	= 0x0000000000043D12;
#else
	static uint64 u64J2000;
	static uint64 u64GMST_A;
	static uint64 u64GMST_B;
	static uint64 u64GMST_M;
	static uint64 u64MilliArcSec;
#endif
};

/** CmTimestamp
 *  This class represents a timestamp measured in nano-seconds
 */
class CmTimestamp : public CmDateTime
{
public:
  CmTimestamp();
  CmTimestamp(int64 Timestamp_ns);
  ~CmTimestamp();

  /** setTimestamp.
   *  A timestamp value will be assigned and converted if necessary.
   */
  void setTimestamp(int64 Timestamp_ns);
  void setTimestamp(uint64 Timestamp_ns);

  /** getTimestamp.
   *  Currently assigned timestamp value will be returned.
   */
	int64 getTimestamp_ns();
	int64 getTimestamp_us();
	int64 getTimestamp_ms();
	int32 getTimestamp_s();
	// return double [s]
	double getTimestamp();
	// get timestamp components
	struct tm& getDateTime(uint64 Timestamp_s =0);
	int32 getYear();			// 1970..
	int32 getMonth();			// 1..12
	int32 getDay();				// 1..31
	int32 getHour();			// 0..23
	int32 getMinute();			// 0..59
	int32 getSecond();			// 0..59
	int32 getDayOfWeek();		// 1..7
	int32 getDayOfYear();		// 1..366

	/** getDateTimestamp.
	*  A timestamp will be created as requested by the date's components.
	*  If all input parameter are zero then the timestamp refers to current date.
	*/
	int64 getDateTimestamp(int32 _Year = 0, int32 _Month = 0, int32 _Day = 0, int32 _Hour = 0, int32 _Minute = 0, int32 _Second = 0);

	/** isDST. DST (day light saving) will be determined */
	bool getDST(int32 _Year, int32 _Month, int32 _Day, int32 _Hour);
	bool getDST();

	/** operator
   *  Some operations with timestamps will be performed
   */
  int64 operator+(int Timestamp_s);
  int64 operator-(int Timestamp_s);
  int64 operator+=(int Period_s);
  int64 operator-=(int Period_s);

	/** getDataRate.
	 *  An average data rate will be estimated from the difference of 
	 *  current timestamp and the timestamp on initialization.
	 */
	double getDataRate(int DataLength);

	/** start/getRuntime.
	*   A precise runtime measurement based on performance counter will be performed.
	*   The start point is either from startRuntime() or from last getRuntime().
	*/
	void startRuntime();
	double getRuntime(bool isSetNow =true);
	int getRuntime_s(bool isSetNow = true);
	int getRuntime_ms(bool isSetNow = true);
	int getRuntime_us(bool isSetNow = true);
	int getRuntime_ns(bool isSetNow = true);

private:
	// current timestamp
  int64 Timestamp_ns;

private:
	// timestamp components
	struct tm stDateTime;
	bool isDST;

private:
	// runtime evaluation
	uint64 RuntimeBegin;
	uint64 RuntimeEnd;
};

// The benchmark class supports running benchmarks
class MBenchmark
{
private:
	// Time source
	CmDateTime mDateTime;
	// Time variables
	int64 n64CurTime;
	int64 n64TrialEnd;
	int64 n64TrialPeriod;
	// Result variables
	int64 n64Duration;
	int64 n64DurationMin;
	int64 n64ZeroOffset;
	int64 n64ZeroOffsetMin;
	// Loop variables
    int32 nLoops;
	int32 nCurLoop;
	int32 nCycles;
	int32 nCurCycle;
	int32 nTrials;
	int32 nCurTrial;
	// Dummy function
	bool dummy();
public:
	// Running and evaluating a benchmark
	bool run();
	CmString info(int64 nLocal=0,int64 n64Divisor=1);

	// Constructors and destructor
	MBenchmark(int32 _nLoops=1000, int32 _nTrialPeriodMilliSec=100, int32 _nTrials=10);
	~MBenchmark();
};

}  // namespace Cosmos

#endif // ifndef CmTimeH
