//////////////////////////////////////////////////////////////////////////////
//
// CmFileFTL.h: Declaration of the CmFileFTL class
//
//////////////////////////////////////////////////////////////////////////////
//
// Author:     Eckhard Kantz
// Website:    wegalink.eu
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

#define INIT_FTLight
#ifndef CmFileFTLH
#define CmFileFTLH

#include "FTLight/CmValueFTL.h"

namespace Cosmos
{

// Parameter definition of hierarchically related CmFileFTL components
enum TIME_BASE { TB_ASTRO, TB_EARTH, TB_LIGHT, TB_FTL };
#define TIME   (int64)
// Time base ASTRO
#define YEAR   1/100
#define KYEAR  1000*YEAR
#define MYEAR  1000*KYEAR
#define GYEAR  1000*MYEAR
#define TYEAR  1000*GYEAR
#define PYEAR  1000*TYEAR
#define EYEAR  1000*PYEAR
// Time base EARTH
#define NANO   1
#define MICRO  1000*NANO
#define MILLI  1000*MICRO
#define SECOND 1000*MILLI
#define MINUTE 60*SECOND
#define HOUR   60*MINUTE
#define DAY    24*HOUR
#define WEEK   7*DAY
#define SYEAR  52*TIME WEEK+TIME DAY+TIME DAY/4-TIME DAY/100+TIME DAY/400
#define SDAY   SYEAR/366
// Time base LIGHT
#define YOCTO  10000
#define ZEPTO  1000*YOCTO
#define ATTO   1000*ZEPTO
#define FEMTO  1000*ATTO
#define PICO   1000*FEMTO
// Time base FTL
#define FTL    1/10
#define KFTL   1000*FTL
#define MFTL   1000*KFTL
#define GFTL   1000*MFTL
#define TFTL   1000*GFTL
#define PFTL   1000*TFTL
#define EFTL   1000*PFTL

/** formatting types when facing different time periods */
enum F_TYPE { 
	F_TOP,
	F_U,			// universe, million years and higher
	F_10K, 			// 10'000 years
	F_K, 			// 1000 years
	F_C, 			// 1 century
	F_D, 			// 1 decade
	F_Y, 			// 1 year
	F_5M, 			// 5 minutes
	F_10S, 			// 10 seconds
	F_1S,			// 1 second
	F_SU,			// milli/micro seconds
	F_SL, 			// 100 nano/femto/zepto seconds/FTL
	F_SLL,			// 1/10 nano/femto/zepto/yocto seconds
	F_FTL,			// 1/10 FTL
};
struct stInitFTLight{
	int64 samples;      // number of samples to be collected (<=16399)
						// 0 = special case (month,year,decade,century)
	TIME_BASE t_base;   // domain that the subsequent 64bit value is accounted to
	int64 slot;         // periode of a time slot, dependent also on domain
	char* format;       // format string of the subdirectory name
	int   factor;       // forward current value multiplied by factor to next level
						// in addition to forwarding the subdirectory name itself
						// - negative values forward the previous subdirectory name
	F_TYPE ftype;       // type of formatting
};
// time period based directory hierarchy
struct stTimePeriodHierarchy {
	unsigned int period;
	char*		 dir_format;
	char*		 file_format;
};

// forward declaration
typedef struct _FileFTLinfo FileFTLinfo;

/** CmFileFTL. working with FTLight files in a FTLight folder hierarchy
*
*  The data to be inserted to FTLight is supposed to be provided in a matrix as follows:
*
*  Data:::array of C columns with M measure values, [`] = FTLight header, [c] = format descriptor, [0, m] = timestamp, [c, m] = measure values
*
*  Procedure:
*  First the FTLight header will be checked whether it is not empty. In that case 
*  it will be inserted into the FTLight file before the data. Further, the parent 
*  entries will be inserted after the header (for parallel writing in FTLight format).
*  The next step will be insertion of all measure values line by line until the 
*  measure count has been reached.
*  Finally, the header will be deleted but the matrix with measure values will be
*  preserved so that it can be used multiple times if needed.
*/
class CmFileFTL
{
public:
	// Constructor and destructor
	CmFileFTL();
	virtual ~CmFileFTL();

public:
	// test FileFTL module
	bool testFileFTL();

public:
	/** putFTLightData.
	*   A set of FTLight data will be inserted into the matrix at the Data element.
	*   Further, a write-to-disk operation will be done when the timestamp is lower
	*   than start-of-period or higher or equal than end-of-period.
	*   A FTLight header will be generated before a write-to-disk operation based
	*   on the StringINI content of the UURI argument.
	*/
	bool putFTLightData(CmValueINI& _UURI, CmValueINI& _Return);

public: 
	// FTLight identifier (UURI)
	bool setIdentifier(const int8 *_Identifier);
	CmUURI& getUURI();

public:
	// FTLight folders
	bool setFTLightRoot(const int8 *_FTLightroot);
	bool initFTLightDir(const CmString& _Location);
	bool setFileName(const int8 *_FileName);
	bool setFilePeriod(uint32 _FilePeriod);
	const char * getFTLightroot();
	const char * getIdentifier();
	const char * getFileName();
	uint32 getFilePeriod();

public:
	// setup FTLight information
	bool setupUURI(CmValueFTL& _InfoUURI);
	bool setupHeader(CmValueFTL& _Data, CmStringFTL& _InfoStringFTL);

public:
	/** write/readData.
	*  Transfer data to and from a FTLight repository. 
	*  In case of a matrix (at Data), a multi-dimensional field is assumed:
	*   [`]   - header in the FTLight file
	*   [0]   - unit and scaling of the first column (timestamp)
	*   [c]   - unit and scaling of data in column c
	*   [0,m] - timestamp of data row m
	*   [c,m] - measurement value m of column c
	*   [0,0,0] - next scheduled write to disk
	*   [0,0,1] - begin of period
	*   [0,0,2] - end of period
	*/
	bool writeData(CmValueFTL& _Data);
	bool readData(CmValueFTL& _Data, CmStringFTL& _StringFTL, CmString _AddrData, int32& _IndexCount);
	bool writeData(time_t _SampleTime, CmString& _Data, int _FileSeqNum = 0);
	bool writeData(uint64 _SampleTime, CmString& _Data, int _FileSeqNum = 0);
	bool writeData(CmTimestamp _SampleTime, const CmString& _Data, int _FileSeqNum = 0);
	bool writeData(time_t _SampleTime, char* _Data, int _DataLen, int _FileSeqNum = 0);
	bool writeData(uint64 _SampleTime, char* _Data, int _DataLen, int _FileSeqNum = 0);
	bool readData(time_t _SampleTime, CmString* _pData);
	bool closeFTLightFile();

public:
	// set FTLight stream parameters
	uint64 setStartTime(CmTimestamp _StartTime, bool _fAdjust =false);
	uint64 setEndTime(CmTimestamp _EndTime, bool _fAdjust =false);
	uint64 setStreamLength_s(uint64 _StreamLength);
	uint64 getStartTime();
	uint64 getEndTime();
	uint64 getStreamLength_s();

public:
	// access files between start time and end time
	bool getStartFiles(CmLString& _StartFiles);
	bool getNextFileFTL(CmStringFTL& _StringFTL);

	// helper functions for managing FTLight folder/file structure
protected:
	bool openTimePeriodFile(time_t _SampleTime, bool _fCreate = false, int _FileSeqNum = 0);
	bool enterSubDirectory(struct stTimePeriodHierarchy* _pTimePeriodHierarchy, unsigned int* _pTimeVal, CmString& _Prefix, bool _fCreate = false);
	CmString* getDataVector(uint64 _u64SampleTime, CmString* _pDataVector, FTLight_DATA_TYPE_IDENTIFIER _eFTLightIdentifier = BINX_TIME);

private:
	// FTLight identifier (UURI)
	CmUURI Identifier;				// unique identifier for a FTLight repository (operator,locator,location,subject)

private:
	// FTLight folders
	CmString InitialDir;			// current directory before FTLight changed it
	CmString FTLightRoot;			// root directory of the FTLight repository
	CmString FTLightDir;			// path to identifier folder below root & location
	CmString Delimiter;				// operating system's path delimiter character

private:
	// FTLight files
	CmString	FileName;			// descriptive component in a FTLight file name
	uint32		FilePeriod;			// time period between two FTLight files
	int32		FileSeqNum;			// a number added to the filename when separate files are requested
	int32		data_fd;			// file descriptor
	uint32		TimeIndex;			// 
	uint32		FileIndex;			//

private:
	// FTLight data
	bool		isNewHeader;		// a new header will be written to file
	CmString	ParentCollection;	// Starting point for parallel read/write operations

private:
	// FTLight stream
	CmTimestamp StartTime;
	CmTimestamp EndTime;
	CmTimestamp FileTime;
	uint64 StreamLength_s;
	bool fMeasurement;
	CmString FTLightFile;
	CmMString FTLightLine;
	CmMString FTLightCollection;

};

//------FileFTL-info--------------------------------------------------------

// survey values
#pragma warning (disable : 4510 4610)
struct _FileFTLinfo {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;
	// test
	CmValueINI Header;
	// data
	CmValueINI Data;

};

#ifdef Info_FTLight
// initialize Info's FTL addresses
FileFTLinfo Info =
{
	// identification
	"EKD@JN58nc_Türkenfeld.Info",
	"/Timestamp",			
	// header
	"Header:Item-1",
	// data
	"Data",

	//Data:::array of C columns with M measure values, [`] = header, [c] = format descriptor, [0, m] = timestamp, [c, m] = measure values
}
#endif
;

} // namespace Cosmos
#endif
