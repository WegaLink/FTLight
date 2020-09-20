//////////////////////////////////////////////////////////////////////////////
//
// CmTime.cpp: Implementation of CmTime classes
//
//////////////////////////////////////////////////////////////////////////////
//
// Author: 	   Eckhard Kantz
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

#include "Services/Basic/CmTime.h"


//--------------------------------------------------------------------
// CmDateTime class
//--------------------------------------------------------------------
CmDateTime::CmDateTime()
{
#ifdef __BORLANDC__
	//u64J2000		    = 0x0D2374199AEA8000;
	//u64GMST_A       = 0x0000FF13D3880BD0;
	//u64GMST_B       = 0x000148508547C092;
	//u64GMST_M       = 0x0001476B081E8000;
	//u64MilliArcSec	= 0x0000000000043D12;
#endif  
}

CmDateTime::~CmDateTime()
{

}

bool CmDateTime::testCmTime()
{
	uint64 u64UTC;
	uint64 u64GMST;
	int32  deltaMilliArcSec;

	//printf("CmDateTime unitTest()\n");
	//printf("=====================\n\n");

#define DATETIME_TIMESTAMP1		"2005-03-28 18:00:00"
#define DATETIME_TIMESTAMP2		"2038-01-01 12:00:00"
#ifdef gcc
  #define DATETIME_PICO_DEG1		96250441924342ull
  #define DATETIME_PICO_DEG2		281246062475200ull
#else
  #define DATETIME_PICO_DEG1		96250441924342
  #define DATETIME_PICO_DEG2		281246062475200
#endif

	u64UTC = getNanoSec(DATETIME_TIMESTAMP1);	// 06h25m00.106 = 96.250441924342 pico deg (Scientific calculator)
												// 06h25m00.053 = 96.250222119888 pico deg (this algorithm)
												// 06h25m00     = 96.25 deg					http://www.go.ednet.ns.ca/~larry/orbits/jsjdetst.html
												// 06h25m00     = 96.25 deg					http://www.jgiesen.de/astro/astroJS/sunriseJS/
	u64GMST = getGMST(u64UTC);
	deltaMilliArcSec = abs((int32)((u64GMST - DATETIME_PICO_DEG1)/u64MilliArcSec));
	//printf("DateTime: %s  ",DATETIME_TIMESTAMP1);
	//printf("GMST delta: %5u milli arc sec\n",deltaMilliArcSec);

	u64UTC = getNanoSec(DATETIME_TIMESTAMP2);
	u64GMST = getGMST(u64UTC);
	deltaMilliArcSec = abs((int32)((u64GMST - DATETIME_PICO_DEG2)/u64MilliArcSec));
	//printf("DateTime: %s  ",DATETIME_TIMESTAMP2);
	//printf("GMST delta: %5u milli arc sec\n\n",deltaMilliArcSec);

	// CmTimestamp test
	const uint32 TestTime = 1524562000; // 2018-04-24 09:26:40 UTC, Tuesday (=3), 114 day of year 
	CmTimestamp TestTimestamp(TestTime * DATETIME_NANOSECONDS);
	if (TestTimestamp.getYear() != 2018) return false;
	if (TestTimestamp.getMonth() != 4) return false;
	if (TestTimestamp.getDay() != 24) return false;
	if (TestTimestamp.getHour() != 9) return false;
	if (TestTimestamp.getMinute() != 26) return false;
	if (TestTimestamp.getSecond() != 40) return false;
	if (TestTimestamp.getDayOfWeek() != 3) return false;
	if (TestTimestamp.getDayOfYear() != 114) return false;

	return true;
}

// Systime access function (nanoseconds)
int64 CmDateTime::getSysClockNanoSec(bool _isPerformanceCounter)
{
	if (_isPerformanceCounter){
		uint64 Ticks = 0;
		uint64 Frequency = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&Ticks);
		QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);

		double Now = (double)Ticks * DATETIME_NANOSECONDS;
		Now /= Frequency;

		return (int64)Now;
	}
	else{
		#ifndef WIN32
			struct timeval  stTimeValue;
			struct timezone stTimeZone;
		    gettimeofday(&stTimeValue,&stTimeZone);
			return (((int64)1000000000*stTimeValue.tv_sec)+(int64)1000*stTimeValue.tv_usec);
		#else
		   struct timeb timebuffer;
			#ifdef MSVS
				_ftime_s( &timebuffer );
			#else
				ftime( &timebuffer );
			#endif
		
		   return (int64)1000000*((int64)1000*timebuffer.time+timebuffer.millitm);
		#endif
	}
}

// Time conversion
uint64 CmDateTime::getNanoSec(const char* szDateTime)
{
#define TIMEZONE 60*60
	CmString  mDateTime(szDateTime);
	CmMString mNum(&mDateTime,CmString(" -_:.cdehilmnostu").getText());

	// Date/time components
	time_t	  nTime;
	int32     nMilliSeconds;
	struct tm stDateTime;
	stDateTime.tm_isdst = 0;
	// Year
	mNum.resetPosition();
	if (NULL==mNum.getNextDelimitedString()){
		throw CmException("Invalid year");
	}
	stDateTime.tm_year = (int32)(mNum.getNumAsUint64() - 1900);
	// Month
	if (NULL==mNum.getNextDelimitedString()){
		throw CmException("Invalid month");
	}
	stDateTime.tm_mon = (int32)(mNum.getNumAsUint64() - 1);
	// Day
	if (NULL==mNum.getNextDelimitedString()){
		throw CmException("Invalid day");
	}
	stDateTime.tm_mday = (int32)mNum.getNumAsUint64();
	// Hour
	if (NULL==mNum.getNextDelimitedString()){
		throw CmException("Invalid hour");
	}
	stDateTime.tm_hour = (int32)mNum.getNumAsUint64();
	// Minute
	if (NULL==mNum.getNextDelimitedString()){
		throw CmException("Invalid minute");
	}
	stDateTime.tm_min = (int32)mNum.getNumAsUint64();
	// Second
	if (NULL==mNum.getNextDelimitedString()){
		throw CmException("Invalid second");
	}
	stDateTime.tm_sec = (int32)mNum.getNumAsUint64();
	// Millisecond
	if (NULL==mNum.getNextDelimitedString()){
		nMilliSeconds = 0;
	}else{
		nMilliSeconds = (int32)mNum.getNumAsUint64();
	}
	// Convert
	if (-1==(nTime=mktime(&stDateTime))){
		throw CmException("Time not recognized",szDateTime);
	}
	return (((uint64)(nTime + TIMEZONE) * 1000) + nMilliSeconds) * 1000000;
}

CmString CmDateTime::getTimeUTC(uint32 _uDateTime, int32 _nDayTime, bool _isFilename)
{
	if (0==_uDateTime){
		_uDateTime = (uint32)time(NULL);
	}

  // Adjust day time (negative = current time)
  if( _nDayTime >= 0 )
    _uDateTime += _nDayTime - _uDateTime % (int32)(TIMEBASE_DAILY/TIMEBASE_SECONDS);

	struct tm stDateTime;
	struct tm* pDateTime = &stDateTime;
	#ifdef __BORLANDC__
		pDateTime = gmtime((time_t*)&uDateTime);
	#else
		int64 u64DateTime = _uDateTime;
		gmtime_s(pDateTime,(time_t*)&u64DateTime);
	#endif
	char achUTC[20];
	if (_isFilename){
		SPRINTF9(achUTC, sizeof(achUTC), "%4u-%02u-%02u_%02u-%02u-%02u",
			pDateTime->tm_year + 1900,
			pDateTime->tm_mon + 1,
			pDateTime->tm_mday,
			pDateTime->tm_hour,
			pDateTime->tm_min,
			pDateTime->tm_sec);
	}
	else{
		SPRINTF9(achUTC, sizeof(achUTC), "%4u-%02u-%02u %02u:%02u:%02u",
			pDateTime->tm_year + 1900,
			pDateTime->tm_mon + 1,
			pDateTime->tm_mday,
			pDateTime->tm_hour,
			pDateTime->tm_min,
			pDateTime->tm_sec);
	}

	return CmString(achUTC);
}

CmString CmDateTime::getTimestamp(int64 Timestamp, int32 TimeOffset, bool WithTime, bool WithMilliSec)
{
	char achUTC[256];
	int64 Timestamp_s = 0;
	int64 Timestamp_ms = 0;

  // Generate timestamp [sec.]
	if( 0 == Timestamp ){
		Timestamp_s = getSysClockNanoSec() / DATETIME_NANOSECONDS;
	}else{
		Timestamp_s = (Timestamp / TIMEBASE_SECONDS);
		Timestamp_ms = (Timestamp - (Timestamp_s * TIMEBASE_SECONDS)) / TIMEBASE_MILLISECONDS;
  }

  // Accept a timestamp offset
	Timestamp_s += TimeOffset;

	// Calculate timestamp components
	struct tm stDateTime = { 0 };
	struct tm* pDateTime = &stDateTime;
	#ifdef __BORLANDC__
		pDateTime = gmtime((time_t*)&uDateTime);
	#else
	if (0 != gmtime_s(pDateTime, (const time_t*)&Timestamp_s)){
		// time conversion failed
		memset(&stDateTime, 0, sizeof(stDateTime));
		stDateTime.tm_sec = Timestamp_s % 60;
		stDateTime.tm_min = (Timestamp_s / 60) % 60;
		stDateTime.tm_hour = (Timestamp_s / 60) % 24;
	}
	#endif

  // Generate a timestamp string
		if (true == WithTime && false == WithMilliSec)
		{
			SPRINTF9(achUTC, sizeof(achUTC), "%4u-%02u-%02u_%02u-%02u-%02u",
				pDateTime->tm_year + 1900,
				pDateTime->tm_mon + 1,
				pDateTime->tm_mday,
				pDateTime->tm_hour,
				pDateTime->tm_min,
				pDateTime->tm_sec);
		}
		else
		if (true == WithTime && true == WithMilliSec)
		{
			SPRINTF7(achUTC, sizeof(achUTC), "%02u-%02u-%02u.%03u",
				pDateTime->tm_hour,
				pDateTime->tm_min,
				pDateTime->tm_sec,
				(int)Timestamp_ms);
		}
		else
		{
    SPRINTF6(achUTC, sizeof(achUTC), "%4u-%02u-%02u",
      pDateTime->tm_year + 1900,
      pDateTime->tm_mon + 1,
      pDateTime->tm_mday);
  }

	return CmString(achUTC);
}


uint64 CmDateTime::getGMST(int64 n64UTC)
{
	// GMST = 280,460618370000 + 360,985647366290 * d
	// where
	// d = UT days since J2000.0, including parts of a day
	// http://www.xylem.f2s.com/kepler/sidereal.htm
	//
	// 360985647366290 = 0x000148508547C092	[pico deg]
	// 280460618370000 = 0x0000FF13D3880BD0 [pico deg]
	// 360000000000000 = 0x0001476B081E8000 [pico deg]
	//
	// J2000 = ((30*365+7)*24+12)*60*60*1000*1000*1000
	//       = 946728000000000000
	//		 = 0x0D2374199AEA8000
	//
	// MilliArcSec = 1000*1000*1000*1000 / (60*60*1000)
	//			   = 277778
	//		       = 0x0000000000043D12
	//
	// Remarks: Implemented accuracy is about 20 arc sec
	//			in the supported period J2000 +/- 136.
	//			If needed, this can be improved by doing
	//			the multiplication in two parts, a lower
	//			part and a higher part.
	//
#define OVERFLOW_PREVENTION 17
	int64  n64Sec2000;
	uint64 u64GMST;

	// Seconds till epoche 2000 (max +/- 136 years

	// TODO
	n64Sec2000 = (n64UTC - u64J2000)/(1000*1000*1000);
/*
    if (abs64(n64Sec2000) > (uint64)MAX_UINT32){
		throw CmException("Date exceeded range: ",(uint32)abs64(n64Sec2000));
	}
*/
	// sidereal progress since J2000
	u64GMST  = (u64GMST_B >> OVERFLOW_PREVENTION) * n64Sec2000;		// this is the critical multiplication
	u64GMST /= (24*60*60);
	u64GMST <<= OVERFLOW_PREVENTION;
	u64GMST += u64GMST_A;
	u64GMST %= u64GMST_M;
	return u64GMST;
}

//--------------------------------------------------------------------
// CmTimestamp class
//--------------------------------------------------------------------
CmTimestamp::CmTimestamp()
{
	// initialize timestamp with current time
	Timestamp_ns = getSysClockNanoSec(false);

	// clear runtime workspace
	RuntimeBegin = 0;
	RuntimeEnd = 0;
}

CmTimestamp::CmTimestamp(int64 Timestamp_ns)
{
  setTimestamp(Timestamp_ns);

  // clear runtime workspace
  RuntimeBegin = 0;
  RuntimeEnd = 0;
}

CmTimestamp::~CmTimestamp()
{

}

void CmTimestamp::setTimestamp(int64 _Timestamp_ns)
{
  Timestamp_ns = _Timestamp_ns;
}

void CmTimestamp::setTimestamp(uint64 _Timestamp_ns)
{
  Timestamp_ns = _Timestamp_ns;
}

int64 CmTimestamp::getTimestamp_ns()
{
  return Timestamp_ns;
}

int64 CmTimestamp::getTimestamp_us()
{
  return Timestamp_ns/1000;
}

int64 CmTimestamp::getTimestamp_ms()
{
  return Timestamp_ns/(1000*1000);
}

int32 CmTimestamp::getTimestamp_s()
{
  return (int)(Timestamp_ns/DATETIME_NANOSECONDS);
}
double CmTimestamp::getTimestamp()
{
	double Timestamp = (double)Timestamp_ns; 
	Timestamp /= DATETIME_NANOSECONDS;

	return Timestamp;
}
struct tm& CmTimestamp::getDateTime(uint64 Timestamp_s)
{
	// take current time in case of a zero timestamp
	Timestamp_s == 0 ? Timestamp_s = time(NULL) : 0;

	// decompose timestamp	
	if (0 != gmtime_s(&stDateTime, (const time_t*)(&Timestamp_s))){
		// time conversion failed
		memset(&stDateTime, 0, sizeof(stDateTime));
		// estimate seconds, minutes and hours
		stDateTime.tm_sec = Timestamp_s % 60;
		stDateTime.tm_min = (Timestamp_s / 60) % 60;
		stDateTime.tm_hour = (Timestamp_s / 3600) % 24;
	}
	return stDateTime;
}
int32 CmTimestamp::getYear()
{
	return getDateTime((uint32)(Timestamp_ns / DATETIME_NANOSECONDS)).tm_year + 1900;
}
int32 CmTimestamp::getMonth()
{
	return getDateTime((uint32)(Timestamp_ns / DATETIME_NANOSECONDS)).tm_mon + 1;
}
int32 CmTimestamp::getDay()
{
	return getDateTime((uint32)(Timestamp_ns / DATETIME_NANOSECONDS)).tm_mday;
}
int32 CmTimestamp::getHour()
{
	return getDateTime((uint32)(Timestamp_ns / DATETIME_NANOSECONDS)).tm_hour;
}
int32 CmTimestamp::getMinute()
{
	return getDateTime((uint32)(Timestamp_ns / DATETIME_NANOSECONDS)).tm_min;
}
int32 CmTimestamp::getSecond()
{
	return getDateTime((uint32)(Timestamp_ns / DATETIME_NANOSECONDS)).tm_sec;
}
int32 CmTimestamp::getDayOfWeek()
{
	return getDateTime((uint32)(Timestamp_ns / DATETIME_NANOSECONDS)).tm_wday + 1;
}
int32 CmTimestamp::getDayOfYear()
{
	return getDateTime((uint32)(Timestamp_ns / DATETIME_NANOSECONDS)).tm_yday + 1;
}

double CmTimestamp::getDateTimestamp(int32 _Year, int32 _Month, int32 _Day, int32 _Hour, int32 _Minute, int32 _Second)
{
	// check whether current time was requested
	bool isCurrentTime = _Year == 0 && _Month == 0 && _Day == 0 && _Hour == 0 && _Minute == 0 && _Second == 0 ? true : false;

	// time components
	struct tm Time = { _Second, _Minute, _Hour, _Day, _Month == 0 ? 0 : _Month - 1, _Year<1900 ? 0 : _Year - 1900 };
	int32 DayTimestamp = isCurrentTime ? (int32)time(NULL) : (int32)mktime(&Time);
	// go back to last midnight for current date
	DayTimestamp -= isCurrentTime ? DayTimestamp % SECONDS_PER_DAY : 0;
	// convert to nanoseconds
	Timestamp_ns = DayTimestamp * DATETIME_NANOSECONDS;

	return (double)Timestamp_ns;
}

int64 CmTimestamp::operator+(int Timestamp_s)
{
  return Timestamp_ns + Timestamp_s * DATETIME_NANOSECONDS;
}

int64 CmTimestamp::operator-(int Timestamp_s)
{
  return Timestamp_ns - Timestamp_s * DATETIME_NANOSECONDS;
}

int64 CmTimestamp::operator+=(int Period_s)
{
  Timestamp_ns += Period_s * DATETIME_NANOSECONDS;
  return Timestamp_ns;
}

int64 CmTimestamp::operator-=(int Period_s)
{
  Timestamp_ns -= Period_s * DATETIME_NANOSECONDS;
  return Timestamp_ns;
}

double CmTimestamp::getDataRate(int DataLength)
{
	// get current timestamp
	CmTimestamp Now;

	// estimate data rate
	double DataRate = Now.getTimestamp();		// end time
	DataRate -= this->getTimestamp();				// minus start time = duration
	if ( DataRate != 0)
		DataRate = DataLength / DataRate;			// amount of data / duration

	return DataRate;
}

void CmTimestamp::startRuntime()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&RuntimeBegin);
}

double CmTimestamp::getRuntime(bool _isSetNow)
{
	uint64 Frequency;
	QueryPerformanceCounter((LARGE_INTEGER*)&RuntimeEnd);
	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);

	double Runtime = 0;
	if (0 != Frequency && 0 != RuntimeBegin){
		Runtime = (double)(RuntimeEnd - RuntimeBegin) / double(Frequency);
	}
	// make current timestamp the new starting point
	_isSetNow ? RuntimeBegin = RuntimeEnd : 0;

	return Runtime;
}
int CmTimestamp::getRuntime_s(bool _isSetNow)
{
	return (int)getRuntime(_isSetNow);
}
int CmTimestamp::getRuntime_ms(bool _isSetNow)
{
	return (int)(getRuntime(_isSetNow) * 1000);
}
int CmTimestamp::getRuntime_us(bool _isSetNow)
{
	return (int)(getRuntime(_isSetNow) * 1000 * 1000);
}
int CmTimestamp::getRuntime_ns(bool _isSetNow)
{
	return (int)(getRuntime(_isSetNow) * 1000 * 1000 * 1000);
}

//--------------------------------------------------------------------
// MBenchmark class
//--------------------------------------------------------------------
#define TOLERANCE	  150
#define PRECISION	  10L
#define RANGE_MILLI   1000000L
#define RANGE_MICRO   1000L
#define RANGE_MIN     100L
#define RANGE_MAX     1000000L

MBenchmark::MBenchmark(int32 _nLoops, int32 _nTrialPeriodMilliSec, int32 _nTrials)
{
	nLoops          = _nLoops > 0 ? _nLoops : 1;
	nTrials         = _nTrials > 0 ? _nTrials : 1;
	n64TrialPeriod  = _nTrialPeriodMilliSec > 0 ? _nTrialPeriodMilliSec : 0; 
	n64TrialPeriod  *= 1000000; //convert to nanoseconds
	// Set start condition
	nCurLoop  = 0;
	nCurTrial = 0;
}
MBenchmark::~MBenchmark()
{

}
bool MBenchmark::run()
{
	// Check loop status
	if(--nCurLoop > 0){
		return true;
	}
	// Check for start condition
	if(0==nCurTrial){
		// Initiate new run
		nCycles          = 1;
		nCurLoop         = nLoops;
		nCurTrial        = nTrials;
		n64DurationMin   = MAX_INT64;
		n64ZeroOffsetMin = MAX_INT64;
		n64TrialEnd      = mDateTime.getSysClockNanoSec() + n64TrialPeriod;
		return true;
	}
	// Check for trial end
    if(n64TrialEnd > (n64CurTime = mDateTime.getSysClockNanoSec())){
		nCycles++;
		nCurLoop = nLoops;
		return true;
	}else{
		// Evaluate trial results
		if(nCycles * nLoops != 0){
			n64Duration = (n64TrialPeriod + n64CurTime - n64TrialEnd)* PRECISION / (nCycles * nLoops); 
			// Evaluate empty loop (zero offset)
			nCurLoop   = nLoops;
			nCurCycle  = nCycles;
			n64CurTime = mDateTime.getSysClockNanoSec();
			while(dummy());
			n64TrialEnd   = mDateTime.getSysClockNanoSec();
			n64ZeroOffset = (n64TrialEnd - n64CurTime)* PRECISION / (nCycles * nLoops); 
		}else{
			n64Duration = -1;
		}
		if(n64Duration > 0 && n64Duration < n64DurationMin){
			n64DurationMin = n64Duration;
		}
		if(n64ZeroOffset >= 0 && n64ZeroOffset < n64ZeroOffsetMin){
			n64ZeroOffsetMin = n64ZeroOffset;
		}
		if(--nCurTrial > 0){
			nCycles          = 1;
			nCurLoop         = nLoops;
			n64DurationMin   = MAX_INT64;
			n64ZeroOffsetMin = MAX_INT64;
			n64TrialEnd      = mDateTime.getSysClockNanoSec() + n64TrialPeriod;
			return true;
		}
		return false;
	}
}
bool MBenchmark::dummy()
{
	if(nCurLoop-- > 0){
		return true;
	}
	if(nCurCycle-- > 0){
		nCurLoop = nLoops;
		return true;
	}
	return false;
}
CmString MBenchmark::info(int64 n64Local,int64 n64Divisor)
{
	int8 achInfo[1000];
	int8 achFlag[1000];

	if(n64Divisor > 1){
		n64DurationMin = (n64DurationMin -n64ZeroOffsetMin) /n64Divisor;
	}else{
		n64DurationMin = n64DurationMin -n64ZeroOffsetMin;
	}
	if((int32)n64DurationMin < (int32)n64Local){
		SPRINTF3(achFlag, sizeof(achFlag), "--");
	}else if(100*(int32)n64DurationMin > TOLERANCE*(int32)n64Local){
		SPRINTF3(achFlag, sizeof(achFlag), "++");
	}else{
		SPRINTF3(achFlag, sizeof(achFlag), " ");
	}
	if((int32)n64DurationMin <= 0){
	 // Invalid
		SPRINTF3(achInfo, sizeof(achInfo), "       n.a.         ");
	}else if((int32)(n64DurationMin/(PRECISION*RANGE_MILLI)) > RANGE_MAX){
	 // Range exceeded
		SPRINTF3(achInfo, sizeof(achInfo), "    - out of range -");
	}else if((int32)(n64DurationMin/(PRECISION*RANGE_MILLI)) >= RANGE_MIN && 
	   (int32)(n64DurationMin/RANGE_MILLI) >= 0){
	    // Millisecond range
		SPRINTF6(achInfo, sizeof(achInfo), "%6u ms ref:%6u %s",
			(int32)(n64DurationMin/(PRECISION*RANGE_MILLI)),
			(int32)(n64Local/(PRECISION*RANGE_MILLI)),
			achFlag);
	}else if((int32)(n64DurationMin/(PRECISION*RANGE_MICRO)) >= RANGE_MIN && 
	   (int32)(n64DurationMin/(RANGE_MICRO*RANGE_MIN)) >= 0){
	    // Microsecond range
		SPRINTF6(achInfo, sizeof(achInfo), "%6u us ref:%6u %s",
			(int32)(n64DurationMin/(PRECISION*RANGE_MICRO)),
			(int32)(n64Local/(PRECISION*RANGE_MICRO)),
			achFlag);
	}else if((int32)(n64DurationMin/PRECISION) >= RANGE_MIN){
	    // Nanosecond range
		SPRINTF6(achInfo, sizeof(achInfo), "%6u ns ref:%6u %s",
			(int32)(n64DurationMin/PRECISION),
			(int32)(n64Local/PRECISION),
			achFlag);
	}else{
	    // Sub-nanosecond range
		SPRINTF6(achInfo, sizeof(achInfo), "%6.1f ns ref:%6.1f %s",(float)n64DurationMin/PRECISION,(float)n64Local/PRECISION,achFlag);
	}
	return CmString(achInfo);
}


