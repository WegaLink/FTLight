//////////////////////////////////////////////////////////////////////////////
//
// CmFileFTL.cpp: Implementation of the CmFileFTL class
//
//////////////////////////////////////////////////////////////////////////////
//
// Author: 	   Eckhard Kantz
// website:    wegalink.eu
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

#define Info_FTLight
#include "FTLight/CmFileFTL.h"

// Initialize FTLight hierarchy structures
#ifdef INIT_FTLight

#define FTLIGHT_TEST_ROOT					"C:\\_FTLightTest"
#define FTLIGHT_TEST_IDENTIFIER		"ekd@JN58nc_Türkenfeld.FTLightTest"
#define FTLIGHT_TEST_FILENAME 		"sample.csv"
#define FTLIGHT_TEST_FILEPERIOD		(10*60)

// Initialize time period hierarchy
static struct stTimePeriodHierarchy TimePeriodHierarchy[] = {
	{  MAX_INT32, "%u",       "%u"        },
	{  MAX_INT32, "-%u",      "-%02u"	    },
	{   24*60*60, "%uth",     "-%02u"	    },		// one file per day
	{ 	   60*60, "utc%02uh", "_utc%02uh" },		// one file per hour
	{ 	      60, "%02um",    "%02um"	    },		// one file per minute
	{ 	 			 1, "%02us",    "%02us"	    },		// one file per second
	{	   	     0, "",         "(%u)"      },		// one file for each data chunk
};
// Initialize months and days tables
char* MonthsShortcut[] = { "0", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
char* DaysShortcut[] = { "0", "1st", "2nd", "3rd", "4th", "5th", "6th", "7th", "8th", "9th", "10th", "11th", "12th", "13th", "14th", "15th", "16th", "17th", "18th", "19th", "20th", "21st", "22nd", "23rd", "24th", "25th", "26th", "27th", "28th", "29th", "30th", "31st" };

static struct stInitFTLight InitFTLight[] = {
    // Domain ASTRO
    {   922, TB_ASTRO, TIME EYEAR     ,       "%uxZY",   1, F_U   }, // 10Z year
    { 10000, TB_ASTRO, TIME PYEAR*10  ,        "%uEY",   1, F_U   }, // 100E year
    { 10000, TB_ASTRO, TIME TYEAR*100 ,        "%uEY",   1, F_U   }, // 1E year
    { 10000, TB_ASTRO, TIME TYEAR     ,       "%uxPY",   1, F_U   }, // 10P year
    { 10000, TB_ASTRO, TIME GYEAR*10  ,      "%uxxTY",   1, F_U   }, // 100T year
    { 10000, TB_ASTRO, TIME MYEAR*100 ,        "%uTY",   1, F_U   }, // 1T year
    { 10000, TB_ASTRO, TIME MYEAR     ,       "%uxGY",   1, F_U   }, // 10G  year
    { 10000, TB_ASTRO, TIME KYEAR*10  ,      "%uxxMY",   1, F_U   }, // 100M  year
    { 10000, TB_ASTRO, TIME YEAR*100  ,        "%uMY",   1, F_U   }, // 1M  year
    { 10000, TB_ASTRO, TIME SYEAR     ,       "%uxKY",   1, F_10K }, // 10K year
    { 12000, TB_ASTRO, TIME SYEAR/12  ,        "%uKY",   1, F_K   }, // 1K year

    // Domain EARTH
    {     0, TB_EARTH, TIME DAY*3     ,        "%uxx",  10, F_C   }, // 1 century (~12175 * 3*DAY)
    {     0, TB_EARTH, TIME HOUR*8    ,         "%ux",  10, F_D   }, // 1 decade (~10958 * 8*HOUR)
    {     0, TB_EARTH, TIME HOUR      ,          "%u",   0, F_Y   }, // 1 year (~8766 * HOUR)
    {     0, TB_EARTH, TIME MINUTE*5  ,     "%s-%02u",   0, F_SU  }, // 1 month (~8640 * 5*MINUTE)
    {  8640, TB_EARTH, TIME SECOND*10 ,     "%s-%02u",   0, F_SU  }, // 1 day
    { 14400, TB_EARTH, TIME MILLI*250 ,    "%s %02uh",   0, F_SU  }, // 1 hour
    { 12000, TB_EARTH, TIME MILLI*25  ,     "%s%02um", -30, F_5M  }, // 5 minutes
    { 10000, TB_EARTH, TIME MILLI     ,  "%s%02um%ux", -10, F_10S }, // 10 seconds
    { 10000, TB_EARTH, TIME MICRO*100 , "%s%02um%02u",   0, F_1S  }, // 1 second
    { 10000, TB_EARTH, TIME MICRO*10  ,       "%s,%u",   0, F_SU  }, // 100 milliseconds
    { 10000, TB_EARTH, TIME MICRO     ,        "%s%u",   0, F_SU  }, // 10 milliseconds
    { 10000, TB_EARTH, TIME NANO*100  ,        "%s%u",   0, F_SU  }, // 1 millisecond
    { 10000, TB_EARTH, TIME NANO*10   ,       "%s.%u",   0, F_SU  }, // 100 microseconds
    { 10000, TB_EARTH, TIME NANO      ,        "%s%u",   0, F_SU  }, // 10 microseconds  (e-9)

    // Domain LIGHT
    { 10000, TB_LIGHT, TIME PICO*10   ,     "%s%u.%u",   0, F_SL  }, // 100 NANO
    { 10000, TB_LIGHT, TIME FEMTO*100 ,      "%s%02u",   0, F_SLL }, // 1 NANO
    { 10000, TB_LIGHT, TIME FEMTO     ,     "%s,%02u",   0, F_SLL }, // 10 PICO          (e-15)
    { 10000, TB_LIGHT, TIME ATTO*10   ,     "%s%u.%u",   0, F_SL  }, // 100 FEMTO
    { 10000, TB_LIGHT, TIME ZEPTO*100 ,      "%s%02u",   0, F_SLL }, // 1 FEMTO
    { 10000, TB_LIGHT, TIME ZEPTO     ,     "%s.%02u",   0, F_SLL }, // 10 ATTO          (e-21)
    { 10000, TB_LIGHT, TIME YOCTO*10  ,     "%s%u,%u",   0, F_SL  }, // 100 ZEPTO
    { 10000, TB_LIGHT, TIME YOCTO/10  ,      "%s%02u",   0, F_SLL }, // 1 ZEPTO
    { 10000, TB_LIGHT, TIME YOCTO/1000,     "%s.%02u",   0, F_SLL }, // 10 YOCTO         (e-27)

    // Domain FTL
    { 10000,   TB_FTL, TIME EFTL*10   ,     "%s%u.%u",   0, F_SL  }, // 100,000 EFTL
    { 10000,   TB_FTL, TIME PFTL*100  ,      "%s%02u",   0, F_FTL }, // 1,000 EFTL
    { 10000,   TB_FTL, TIME PFTL      ,     "%s,%02u",   0, F_FTL }, // 10 EFTL          (e-33)
    { 10000,   TB_FTL, TIME TFTL*10   ,     "%s%u.%u",   0, F_SL  }, // 100 PFTL
    { 10000,   TB_FTL, TIME GFTL*100  ,      "%s%02u",   0, F_FTL }, // 1 PFTL
    { 10000,   TB_FTL, TIME GFTL      ,     "%s.%02u",   0, F_FTL }, // 10 TFTL          (e-39)
    { 10000,   TB_FTL, TIME MFTL*10   ,     "%s%u,%u",   0, F_SL  }, // 100 GFTL
    { 10000,   TB_FTL, TIME KFTL*100  ,      "%s%02u",   0, F_FTL }, // 1 GFTL
    { 10000,   TB_FTL, TIME KFTL      ,     "%s.%02u",   0, F_FTL }, // 10 MFTL          (e-45)
    { 10000,   TB_FTL, TIME FTL*10    ,     "%s%u.%u",   0, F_SL  }, // 100 KFTL
                                                                    //						1 FTL  (e-48)
    {     0,   TB_FTL,               0,            "",   0, F_TOP }
};
#endif


// CmFileFTL class.
CmFileFTL::CmFileFTL()
{
  // Initialize FTLight internal state
	data_fd = 0;				// indicate that no data file is open
	TimeIndex = 0;
	FileIndex = 0;
	FileSeqNum = 0;

	// Initialize device directory
	FTLightRoot = "C:";
	Delimiter = "\\";

	// Remember initial (current) directory
	char curdir[MAXPATHLEN];
	GETCWD((LPSTR)curdir, MAXPATHLEN);
	InitialDir = curdir;

	// Initialize FileFTL parser
	FTLightLine.setSourceString(&FTLightFile);
	FTLightLine.setDelimiters("\n\r");
	FTLightCollection.setSourceString(&FTLightLine);
	FTLightCollection.setDelimiters(",;:=");
	fMeasurement = false;

	// Initialize time settings for FTLight stream
	const int32 StreamDefaultLength = (24 * 60 * 60);
	StreamLength_s = StreamDefaultLength;
	StreamLength_s *= DATETIME_NANOSECONDS;
	FilePeriod = 3600;
}
CmFileFTL::~CmFileFTL()
{
	// Close data file if it is still open
	if (data_fd > 0){
		close(data_fd);
	}
}

// test FTLight module
bool CmFileFTL::testFileFTL()
{
	// create root, set identifier
	setFTLightRoot(FTLIGHT_TEST_ROOT);
	if(false == setIdentifier(FTLIGHT_TEST_IDENTIFIER)) return false;

	// initialize files
	if (false == setFileName(FTLIGHT_TEST_FILENAME)) return false;
	if (false == setFilePeriod(FTLIGHT_TEST_FILEPERIOD)) return false;

	// adjust identifier from FTLight information
	CmValueINI LogLevel;	// only LogLevel used, message and context are ignored
	LogLevel = CMLOG_Info;
	CmString DefaultInfo(DEFAULT_Info_FileFTL);
	Info.UURI.updateInformation(DefaultInfo, LogLevel);
	if (false == setupUURI(Info.UURI)) return false;
	 
	// generate a data set
	const int32 Loops = 3;
	const int32 Columns = 3;
	const int32 Count = 5;
	uint64 Timestamp;

	// set header
	CmStringFTL InfoStringFTL;
	InfoStringFTL.processStringFTL(DEFAULT_Info_FileFTL);
	Info.UURI.syncSettings(InfoStringFTL);
	if (false == setupHeader(Info.Data, InfoStringFTL)) return false;

	// matrix access
	CmMatrix& M = Info.Data.getMatrix();

	// [c] = format descriptor

	// set column format
	CmString Format;
	// time
	CmStringFTL T;
	T.encodeTimeFormat(Format, 1, DATETIME_NANOSECONDS);
	M(0)=Format;
	for (int32 c = 1; c < Columns; c++){
		T.encodeValueFormat(Format,UNIT_VOLTAGE, 1, 1000);
		M(c) = Format;
	}

	// [0, m] = timestamp, [c, m] = measure values

	// fill matrix with values
	double Value;
	for (int32 l = 0; l < Loops; l++){
		// new header
		if (l > 0 && (l % 2 == 0)){
			CmString NewItem = "Item-";
			NewItem += l;
			Info.TestItem.setText(NewItem);
			Info.UURI.syncSettings(InfoStringFTL);
			if (false == setupHeader(Info.Data, InfoStringFTL)) return false;
		}
		for (int32 m = 0; m < Count; m++){
			// timestamp
			Sleep(10);
			Timestamp = CmTimestamp::getSysClockNanoSec(false);
			M(0, m) = Timestamp;
			for (int32 c = 1; c < Columns; c++){
				Value = m*Columns + c;
				M(c, m) = Value;
			}
		}
		// write data to disk
		if (false == writeData(Info.Data)) return false;
		// clear Matrix@Info_FTLight
		M.clearMatrix();
	}

	return true;
}

// FTLight identifier (UURI)
bool CmFileFTL::setIdentifier(const int8 *_Identifier)
{
	Identifier.setUURI(NULL, _Identifier);

	// Extract location from identifier
	CmString Location = Identifier.getLocation();

	// Initialize device dir
	return initFTLightDir(Location);
}
CmUURI& CmFileFTL::getUURI()
{
	return Identifier;
}

// FTLight folders
bool CmFileFTL::initFTLightDir(const CmString& _Location)
{
  // Check path component's validity
  if (0==FTLightRoot.getLength() || 0==_Location.getLength()) return false;

	// check root directory
	if (-1 == _chdir(FTLightRoot.getText())){
		// Create root directory
		if (-1 == MKDIR(FTLightRoot.getText(), S_MASK)) return false;
	}

  // Generate location dir (= root + location)
	FTLightDir	 = FTLightRoot;
	FTLightDir += Delimiter;
	FTLightDir += _Location;

	// check location directory
	if (-1 == _chdir(FTLightDir.getText())){
		// Create location directory if it does not exist
		if (-1 == MKDIR(FTLightDir.getText(), S_MASK)) return false;
	}

  // Complete device dir (= root + location + identifier)
	FTLightDir += Delimiter;
	FTLightDir += Identifier.getText();

	return true;
}
bool CmFileFTL::setFTLightRoot(const int8 *_FTLightRoot)
{
	this->FTLightRoot = _FTLightRoot;

	return true;
}
bool CmFileFTL::setFileName(const int8 *_FileName)
{
	this->FileName = _FileName;
	return true;
}
bool CmFileFTL::setFilePeriod(uint32 _FilePeriod)
{
	this->FilePeriod = _FilePeriod;
	return true;
}
const char * CmFileFTL::getFTLightroot()
{
	return FTLightRoot.getText();
}
const char * CmFileFTL::getIdentifier()
{
	return Identifier.getText();
}
const char * CmFileFTL::getFileName()
{
	return FileName.getText();
}
uint32 CmFileFTL::getFilePeriod()
{
  return FilePeriod;
}

// setup FTLight information
bool CmFileFTL::setupUURI(CmValueFTL& _InfoUURI)
{
	if (false == setIdentifier(_InfoUURI.asStringConvert().getText())) return false;

	return true;
}
bool CmFileFTL::setupHeader(CmValueFTL& _Data, CmStringFTL& _InfoStringFTL)
{
	// serialize StringFTL
	CmString Info;
	_InfoStringFTL.serializeStringFTL(Info);
	// strip first line (UURI, timestamp) from info string
	int32 FirstEOL = Info.CmString::findPosition('\n');

	CmString Header;
	Header.assignSubString((int32)Info.getLength(), &Info, FirstEOL + 1);

	// matrix access
	CmMatrix& M = _Data.getMatrix();

	// [`] = header

	// assign new header
	M() = Header;

	return true;
}

// Transfer data to and from FTLight files
bool CmFileFTL::writeData(CmValueFTL& _Data)

{
	// Data:::array of C columns with M measure values, [`] = header, [c] = format descriptor, [0, m] = timestamp, [c, m] = measure values

	// matrix access
	CmMatrix& M = _Data.getMatrix();
	int32 Columns = M().getSizeLastLevel();
	int32 Count = M(0).getSizeLastLevel();

	// check for new header
	CmString Header;
	Header.setText(CmString(M()));
	if (Header.getLength() > 0){
		// set new header
		ParentCollection = Header;
		isNewHeader = true;
		// add format descriptors for parallel writing
		for (int32 i = 0; i < Columns; i++){
			ParentCollection += CmString(M(i));
			ParentCollection += ";";
		}
		ParentCollection += "@\n";
		// clear header: this will now be done one layer up
		//D.setString("", I.clear());
	}

	// add measure data
	uint64 Timestamp;
	CmString Line;
	CmString Num;
	for (int32 m = 0; m < Count; m++){
		// timestamp
		Timestamp = uint64(M(0, m));
		// reject invalid timestamps
		if(0 == Timestamp) continue;
		_Data.num2BinX(&Num, Timestamp);
		Line = Num;
		// columns with measure values
		for (int32 c = 1; c < Columns; c++){
			Line += ";";

			float Value = float(M(c, m));
			_Data.num2BinX(&Num, *(uint32*)&Value);
			Line += Num;
		}
		Line += "\n";
		// write data to FTLight file
		if(false == writeData(Timestamp, Line)) return false;
	}
	return true;
}
bool CmFileFTL::readData(CmValueFTL& _Data, CmStringFTL& _StringFTL, CmString _AddrData, int32& _IndexCount)
{
	// Data:::array of C columns with M measure values, [`] = header, [c] = column descriptor, [0, m] = timestamp, [c, m] = measure values

	// matrix access
	CmMatrix& M = _Data.getMatrix();
	int32 Column;
	int32 Row;
	CmString Addr;
	CmString Text;
	CmString Descriptor;

	// get column headers
	_AddrData += "-";
	for (Column = 0;; Column++){
		// get headline
		Addr = _AddrData;
		Addr += Column;
		if (false == _StringFTL.getValue(Addr, Text)) break;
		Descriptor = Text;
		// get unit
		Addr += "-0";
		if (false == _StringFTL.getValue(Addr, Text)) break;
		Descriptor += ";";
		Descriptor += Text;
		// write column descriptor to matrix
		M(Column) = Descriptor;
	}

	// lower timestamp limit
	CmTimestamp LowestTimestamp;
	LowestTimestamp.getDateTimestamp(2000); // year 2000

	// transfer measurement data
	int64 Timestamp;
	CmStringFTL ValueFTL;
	// row
	for (Row = 0;; Row++){
		// timestamp
		Addr = _AddrData;
		Addr += "0-0-";
		Addr += Row;
		if (false == _StringFTL.getValue(Addr, ValueFTL)) break;
		ValueFTL.decodeNumX2uint64((uint64*)&Timestamp);
		// validate timestamp
		if (Timestamp > 0){
			while (Timestamp < LowestTimestamp.getTimestamp_ns()){
				Timestamp *= 10;
			}
		}
		// write timestamp to matrix
		M(0, _IndexCount + Row) = Timestamp;

		// columns
		float ValueFloat;
		for (Column = 1;; Column++){
			// generate address
			Addr = _AddrData;
			Addr += Column;
			Addr += "-0-";
			Addr += Row;
			// restore measurement value
			if (false == _StringFTL.getValue(Addr, ValueFTL)) break;
			ValueFTL.decodeNumX2uint32((uint32*)&ValueFloat);
			// write value to matrix
			M(Column, _IndexCount + Row) = ValueFloat;
		}
	}
	_IndexCount += Row;

	return true;
}
bool CmFileFTL::writeData(time_t _SampleTime, CmString& _FTLight, int _FileSeqNum)
{
	return writeData(_SampleTime, _FTLight.getBuffer(), (int32)_FTLight.getLength(), _FileSeqNum);
}
bool CmFileFTL::writeData(uint64 _u64SampleTime, CmString& _FTLight, int _FileSeqNum)
{
	return writeData((time_t)(_u64SampleTime / DATETIME_NANOSECONDS), _FTLight.getBuffer(), (int32)_FTLight.getLength(), _FileSeqNum);
}
bool CmFileFTL::writeData(CmTimestamp _StartTime, const CmString& _FTLight, int _FileSeqNum)
{
	return writeData((time_t)(_StartTime.getTimestamp_s()), _FTLight.getBuffer(), (int32)_FTLight.getLength(), _FileSeqNum);
}
bool CmFileFTL::writeData(uint64 _u64SampleTime, char* _Data, int _DataLen, int _FileSeqNum)
{
	return writeData((time_t)(_u64SampleTime / DATETIME_NANOSECONDS), _Data, _DataLen, _FileSeqNum);
}
bool CmFileFTL::writeData(time_t _SampleTime, char* _Data, int _DataLen, int _FileSeqNum)
{
	// Check time period index
	if (FilePeriod != 0 && TimeIndex == _SampleTime / FilePeriod && this->FileSeqNum == _FileSeqNum){
		// Check if the data file needs to be opened
		if (data_fd <= 0){
			if (false == openTimePeriodFile(_SampleTime, true, _FileSeqNum)) return false;
		}
	}
	else{
		this->FileSeqNum = _FileSeqNum;
		if (data_fd > 0){
			close(data_fd);
			data_fd = 0;
		}
		if (false == openTimePeriodFile(_SampleTime, true, _FileSeqNum)) return false;
	}
	// check if file is open for writing
	if (data_fd <= 0) return false;
	// append a new header if appropriate
	if (isNewHeader){
		if (-1 == WRITE(data_fd, ParentCollection.getBuffer(), (uint32)ParentCollection.getLength())){
			return false;
		}
		isNewHeader = false;
	}

	// Append data to file
	if (-1 == WRITE(data_fd, _Data, _DataLen)){
		//throw (CmException("System error 'write'",errno));
		return false;
	}
	return true;
}
bool CmFileFTL::readData(time_t _SampleTime, CmString* _pData)
{
	if (false == openTimePeriodFile(_SampleTime)) return false;
		//Determine file size
		struct stat stat_buf;
		uint32 uFileSize;
		if (0 == fstat(data_fd, &stat_buf)){
			uFileSize = stat_buf.st_size;
			_pData->setLength(uFileSize);
	}
	else{
		//throw CmException("File status not accessible:");
		return false;
	}
	// Get data from file into memory
	uFileSize = read(data_fd, _pData->getBuffer(), uFileSize);
	close(data_fd);
	data_fd = 0;

	return true;
}
bool CmFileFTL::closeFTLightFile()
{
	// Close data file if still open
	if (data_fd > 0)
	{
		close(data_fd);
		data_fd = 0;
	}
	return true;
}

// set FTLight stream parameters
uint64 CmFileFTL::setStartTime(CmTimestamp _StartTime, bool _fAdjust)
{
	StartTime.setTimestamp(_StartTime.getTimestamp_ns());
	if (true == _fAdjust){
		EndTime.setTimestamp(StartTime.getTimestamp_ns() + StreamLength_s * DATETIME_NANOSECONDS);
	}
	FilePeriod > 0 ? FileTime.setTimestamp(StartTime.getTimestamp_ns() - (StartTime.getTimestamp_s() % FilePeriod) * DATETIME_NANOSECONDS) : FileTime.setTimestamp(StartTime.getTimestamp_ns());
	return StartTime.getTimestamp_ns();
}
uint64 CmFileFTL::setEndTime(CmTimestamp _EndTime, bool _fAdjust)
{
	EndTime.setTimestamp(_EndTime.getTimestamp_ns());
	if (true == _fAdjust){
		StartTime.setTimestamp(EndTime.getTimestamp_ns() - StreamLength_s * DATETIME_NANOSECONDS);
	}
	FilePeriod > 0 ? FileTime.setTimestamp(StartTime.getTimestamp_ns() - (StartTime.getTimestamp_s() % FilePeriod) * DATETIME_NANOSECONDS) : FileTime.setTimestamp(StartTime.getTimestamp_ns());
	return EndTime.getTimestamp_ns();
}
uint64 CmFileFTL::setStreamLength_s(uint64 _StreamLength_s)
{
	StreamLength_s = _StreamLength_s;
	return StreamLength_s;
}
uint64 CmFileFTL::getStartTime()
{
	return StartTime.getTimestamp_ns();
}
uint64 CmFileFTL::getEndTime()
{
	return EndTime.getTimestamp_ns();
}
uint64 CmFileFTL::getStreamLength_s()
{
	return (EndTime.getTimestamp_s() - StartTime.getTimestamp_s());
}

// access files between start and end time
bool CmFileFTL::getStartFiles(CmLString& _StartFiles)
{
	// path to FTLight repository
	CmString FTLightPath(FTLightDir);
	FTLightPath += Delimiter;

	// determine first yearly folder
	CmString PathYear;
	for (int32 Year = StartTime.getYear(); Year <= EndTime.getYear(); Year++){
		PathYear = FTLightPath;
		PathYear += Year;
		if (PathYear.isFolderExists()) break;
		PathYear.clear();
	}
	if (PathYear.isEmpty()) return false;
	// add files on yearly path
	PathYear += Delimiter;
	PathYear.addFilesOnPath(_StartFiles);

	// determine first monthly folder
	CmString PathMonth;
	for (int32 Month = 1; Month <= 12; Month++){
		PathMonth = PathYear;
		PathMonth += MonthsShortcut[Month];
		if (PathMonth.isFolderExists()) break;
		PathMonth.clear();
	}
	if (PathMonth.isEmpty()) return false;
	// add files on monthly path
	PathMonth += Delimiter;
	PathMonth.addFilesOnPath(_StartFiles);

	// determine first daily folder
	CmString PathDay;
	for (int32 Day = 1; Day <= 31; Day++){
		PathDay = PathMonth;
		PathDay += DaysShortcut[Day];
		if (PathDay.isFolderExists()) break;
		PathDay.clear();
	}
	if (PathDay.isEmpty()) return false;
	// add files on daily path
	PathDay += Delimiter;
	PathDay.addFilesOnPath(_StartFiles);

	// get all files from each of first subfolders
	CmString PathFolder(PathDay);
	CmString SearchFolder;
	CmLString Folders;
	for (;;){
		// get all folders
		SearchFolder = PathFolder;
		SearchFolder += "*";
		if (false == SearchFolder.getFoldersOnPath(Folders)) break;
		// extend path by first folder name
		PathFolder += Folders.getLowestString();
		PathFolder += Delimiter;
		// add all files on path
		PathFolder.addFilesOnPath(_StartFiles);
	}

	// check whether start files have been found
	if (_StartFiles.getSize() == 0) return false;

	return true;
}
bool CmFileFTL::getNextFileFTL(CmStringFTL& _StringFTL)
{
	// clear result
	_StringFTL.clearFTLightHierarchy();

	// step through matching file candidates
	CmString FileFTL;
	int32 CurrentTime;
	while ((CurrentTime = FileTime.getTimestamp_s()) <= EndTime.getTimestamp_s() + (int32)FilePeriod){
		// step forward by file period
		FileTime.setTimestamp(FileTime.getTimestamp_ns() + FilePeriod * DATETIME_NANOSECONDS);
		// read file from disk if one is available
		if (readData(CurrentTime, &FileFTL)) break;
	}
	// check whether endtime has exceeded
	if (FileTime.getTimestamp_s() > EndTime.getTimestamp_s() + (int32)FilePeriod) return false;

	// convert file content to result StringFTL
	if (false == _StringFTL.processStringFTL(FileFTL)) return false;

	// TEST: write file FTL back to disk
	//FileFTL.clear();
	//_StringFTL.serializeStringFTL(FileFTL);
	//FileFTL.writeBinary("FileFTL.txt");

	return true;
}

bool CmFileFTL::openTimePeriodFile(time_t _SampleTime, bool _fCreate, int _FileSeqNum)
{
	CmString mNum;
	CmString mMeta;
	CmString	mParams;
	CmString	mParamsFile;

	// validate time period
	FilePeriod == 0 ? FilePeriod = 3600 : 0;

	_SampleTime -= _SampleTime%FilePeriod;
	struct tm stDateTime;
	struct tm* pDateTime = &stDateTime;
#ifdef __BORLANDC__
	pDateTime = gmtime(&SampleTime);
#else
	gmtime_s(pDateTime, &_SampleTime);
#endif
	uint32 TimeVal[] = {
		(uint32)(pDateTime->tm_year + 1900),
		(uint32)(pDateTime->tm_mon + 1),
		(uint32)pDateTime->tm_mday,
		(uint32)pDateTime->tm_hour,
		(uint32)pDateTime->tm_min,
		(uint32)pDateTime->tm_sec,
		0
	};
	CmString Prefix("");

	// Enter (create) root directory in data storage
	if (-1 == chdir(FTLightDir.getText())){
		if (true == _fCreate){
			if (-1 == MKDIR(FTLightDir.getText(), S_MASK)){
				//throw (CmException("System error 'mkdir'",errno));
				return false;
			}
			if (-1 == chdir(FTLightDir.getText())){
				//throw (CmException("System error 'chdir'",errno));
				return false;
			}
		}
		else{
			//throw CmException("Folder does not exist",FTLightDir.getText());
			return false;
		}
	}
	// Enter (create) subsequent directories in data storage
	try{
		enterSubDirectory(TimePeriodHierarchy, TimeVal, Prefix, _fCreate);
	}
	catch (CmException e){
		// Change back to initial dir
		if (-1 == chdir(InitialDir.getText())){
			//throw (CmException("System error 'chdir'",errno));
			return false;
		}
		//throw e;
		return false;
	}
	// Complete file name
	Prefix += "_";
	Prefix += FileName;
	if (true == _fCreate)
	{
		// Check if separate filenames are requested
		if (_FileSeqNum > 0)
		{
			// Determine file extension
			int PosExt = (int32)Prefix.getLength() - 1;
			for (; PosExt > 0; PosExt--)
			{
				if (Prefix[PosExt] == '\\' || Prefix[PosExt] == '.')
					break;
			}

			// Check if an extension does exists
			if (PosExt > 0 && Prefix[PosExt] == '.')
			{
				// Extend the filename in front of the extension by current sequence number
				size_t LenExt = Prefix.getLength() - PosExt;
				CmString NewName(PosExt);
				CmString Extension((int32)LenExt);
				memcpy(NewName.getBuffer(), Prefix.getBuffer(), PosExt);
				memcpy(Extension.getBuffer(), Prefix.getBuffer() + PosExt, LenExt);
				NewName += "_";
				NewName += (uint32)_FileSeqNum;
				NewName += Extension;
				Prefix = NewName;
			}
		}

		// Open data file for writing
#ifdef MSVS
		_sopen_s(&data_fd, Prefix.getText(), O_RDWR | O_BINARY | O_APPEND, _SH_DENYNO, S_IREAD | S_IWRITE);
		if (-1 == data_fd){
			// Maybe file does not exist so creat it
			_sopen_s(&data_fd,
				Prefix.getText(),
				O_RDWR |		// read/write access
				O_BINARY |		// binary (untranslated) 
				O_CREAT,		// create file
				_SH_DENYNO,	// allow read/write access
				S_IREAD |		// user can read
				S_IWRITE		// others can read
				);
			if (-1 == data_fd){
				//throw (CmException("System error '_sopen_s'",errno));
				return false;
			}
#else
			if (-1 == (data_fd = OPEN(Prefix.getText(), O_RDWR | O_BINARY | O_APPEND, S_IREAD | S_IWRITE))){
				// Maybe file does not exist so creat it
				if (-1 == (data_fd = OPEN(Prefix.getText(), O_RDWR |		// read/write access
					O_BINARY |		// binary (untranslated) 
					O_CREAT,		// create file
					S_IREAD |		// user can read
					S_IWRITE |		// user can write
					S_IRGRP |		// group can read
					S_IROTH))){	// others can read
					//throw (CmException("System error 'open'",errno));
					return false;
				}
#endif
				// Generate identification line
				mMeta = Identifier.getText();
				mMeta += ",";
				mNum = (uint64)_SampleTime;
				mMeta += mNum;
				mMeta += ",duration:";
				mNum = FilePeriod;
				mMeta += mNum;
				mMeta += "\n";
				// Append parameter file content
				mParamsFile = FTLightDir;
				mParamsFile += Delimiter;
				// Append parent collection for parallel writing
				mMeta += ParentCollection;
				isNewHeader = false;
				// Write meta information to file
				if (-1 == WRITE(data_fd, mMeta.getBuffer(), (int32)mMeta.getLength())){
					//throw (CmException("System error 'write'",errno));
					return false;
				}
			}
		}
	else{
		// Open data file for reading
#ifdef MSVS
		_sopen_s(&data_fd, Prefix.getText(), O_RDONLY, _SH_DENYWR, 0);
		if (-1 == data_fd){
			//throw (CmException("System error '_sopen_s'",errno));
			return false;
		}
#else
		if (-1 == (data_fd = OPEN(Prefix.getText(), O_RDONLY, 0))){
			//throw (CmException("File does not exist"));
			return false;
		}
#endif
	}
	// Remember opened file period
	if (FilePeriod>0){
		TimeIndex = (uint32)(_SampleTime / FilePeriod);
	}
	else{
		TimeIndex = 0;
	}
	// Change back to initial dir

	if (-1 == chdir(InitialDir.getText())){
		//throw (CmException("System error 'chdir'",errno));
		return false;
	}
	return true;
	}
bool CmFileFTL::enterSubDirectory(struct stTimePeriodHierarchy* _pTimePeriodHierarchy, unsigned int* _pTimeVal,CmString& _Prefix,bool _fCreate)
{
    // Escape period==0
    if (0==_pTimePeriodHierarchy->period){
        char Index[100];
        SPRINTF4(Index,sizeof(Index),_pTimePeriodHierarchy->file_format,FileIndex++);
        _Prefix += Index;
        return false;
    }
	// Generate subsequent names
    char dir[100];
	char prefix[100];
	SPRINTF4(dir,sizeof(dir),_pTimePeriodHierarchy->dir_format,*_pTimeVal);
    // Convert month to string
    if (dir[0]=='-'){
        STRCPY(dir,sizeof(dir),MonthsShortcut[*_pTimeVal]);
    }
    // Adjust day numeral
    if (!strcmp(dir,"1th")){
        STRCPY(dir,sizeof(dir),"1st");
    }
    if (!strcmp(dir,"2th")){
        STRCPY(dir,sizeof(dir),"2nd");
    }
    if (!strcmp(dir,"3th")){
        STRCPY(dir,sizeof(dir),"3rd");
    }
    if (!strcmp(dir,"21th")){
        STRCPY(dir,sizeof(dir),"21st");
    }
    if (!strcmp(dir,"22th")){
        STRCPY(dir,sizeof(dir),"22nd");
    }
    if (!strcmp(dir,"23th")){
        STRCPY(dir,sizeof(dir),"23rd");
    }
    if (!strcmp(dir,"31th")){
        STRCPY(dir,sizeof(dir),"31st");
    }
	SPRINTF4(prefix,sizeof(prefix),_pTimePeriodHierarchy->file_format,*_pTimeVal);
	_Prefix += prefix;
	// Enter (create) subsequent directory level
	if (-1==chdir(dir)){
		if (true==_fCreate){
			if (-1==MKDIR(dir,S_MASK)){
				//throw (CmException("System error 'mkdir'",errno));
				return false;
			}
			if (-1==chdir(dir)){
				//throw (CmException("System error 'chdir'",errno));
				return false;
			}
		}else{
			//throw CmException("Folder does not exist",_Prefix);
			return false;
		}
	}
	// Check if another directory level is needed
	if (FilePeriod<_pTimePeriodHierarchy->period){
		return enterSubDirectory(_pTimePeriodHierarchy+1,_pTimeVal+1,_Prefix,_fCreate);
	}
	return true;
}
CmString* CmFileFTL::getDataVector(uint64 _u64SampleTime, CmString* _pDataVector, FTLight_DATA_TYPE_IDENTIFIER _eFTLightIdentifier)
{
	uint32   uFileTimestamp;
	// not needed?	uint64   u64TimeStamp;
	CmString  mTimeStamp;
	CmString	 mFile;
	CmMString mLine(&mFile, "\r\n");
	CmMString mCollection(&mLine, ":=");
	CmMString mItem(&mCollection, ",;");
	// Obtain file content
	uFileTimestamp = (uint32)(_u64SampleTime / DATETIME_NANOSECONDS);
	uFileTimestamp -= uFileTimestamp % FilePeriod;
	readData(uFileTimestamp, &mFile);
	// Split file into lines
	mLine.resetPosition();
	while (NULL != mLine.getNextDelimitedString()){
		// Check for BinX type
		if (true == CmStringFTL::isDataTypeIdentifier(mLine, _eFTLightIdentifier)){
			// Proceed to desired timestamp
			for (;;){
				// Obtain next measurement line
				if (NULL != mLine.getNextDelimitedString()){
					// Read complete measurement line
					mCollection.resetPosition();
					if (NULL != mCollection.getNextDelimitedString()){
						// Read timestamp
						mItem.resetPosition();
						if (NULL != mItem.getNextDelimitedString()){
							// Check for desired timestamp
							// not needed? if (u64SampleTime > (u64TimeStamp = 1000*1000*BinX2num(mItem))){
							if (_u64SampleTime > (1000 * 1000 * CmStringFTL::BinX2num(mItem))){
								continue;
							}
							else if (NULL != mItem.getNextDelimitedString()){
								// Return found data vector
								*_pDataVector = mItem;
								return _pDataVector;
							}
						}
					}
				}
			}
		}
	}
	return NULL;
}

//void CmFileFTL::dumpData(time_t _SampleTime,CmString* _pData,CmString& _mStyle)
//{
//	uint32   uCount;
//	uint64   u64MilliTimeStamp;
//	CmString  mTimeStamp;
//	CmString	 mFile;
//	CmString  mPrefix;
//	CmString  mNum;
//	CmString  mBin;
//	CmString  CmFileFTLVector;
//	CmMString mLine(&mFile,"\r\n");
//	CmMString mCollection(&mLine,":=");
//	CmMString mItem(&mCollection,",;");
//	// Obtain file content
//	readData(_SampleTime-_SampleTime%FilePeriod,&mFile);
//	// Generate header
//	if (_mStyle=="meta"){
//		(*_pData) += "\n\rMeta data:\n\r----------\n\r";
//	}
//	// Split file into lines
//	mLine.resetPosition();
//	while (NULL!=mLine.getNextDelimitedString()){
//		// Check for BinX type 
//		if (true == CmStringFTL::isDataTypeIdentifier(mLine, BINX_TIME) ||
//			true == CmStringFTL::isDataTypeIdentifier(mLine, BINX_VALUE) ||
//			true == CmStringFTL::isDataTypeIdentifier(mLine, BINX_STRING) ||
//			true == CmStringFTL::isDataTypeIdentifier(mLine, BINX_BINARY)){
//			// Proceed to desired timestamp
//			for(;;){
//				// Obtain next measurement line
//			if (NULL!=mLine.getNextDelimitedString()){
//				mCollection.resetPosition();
//					// Read complete measurement line
//				if (NULL!=mCollection.getNextDelimitedString()){
//					mItem.resetPosition();
//					// Read timestamp
//					if (NULL!=mItem.getNextDelimitedString()){
//							// Check for desired timestamp
//							if ((uint32)_SampleTime > (uint32)((u64MilliTimeStamp = CmStringFTL::BinX2num(mItem))/1000)){
//								continue;
//							}
//						// Read data vector
//						if (NULL!=mItem.getNextDelimitedString()){
//							if (_mStyle=="binx"){
//									(*_pData) += "\n\rBinX data:\n\r-----------\n\r";
//							(*_pData) += mItem;
//							}
//							mBin.setLength(mItem.getLength());
//							uCount = CmStringFTL::BinX2bin((uint8*)mBin.getBuffer(), (int32)mBin.getLength(), mItem);
//							if (_mStyle=="table"){
//									(*_pData) += "\n\rTable data:\n\r------------\n\r";
//							}else if (_mStyle=="data"){
//									(*_pData) += "\n\rData:\n-----\n\r";
//							}
//							for (uint32 i=0;i<uCount;i++){
//								mNum         = i;
//								if (_mStyle=="table"){
//								CmFileFTLVector += mNum;
//								CmFileFTLVector += ",";
//									mNum         = (uint32)(*((uint8*)(mBin.getBuffer()+i)));
//								CmFileFTLVector += mNum;
//										CmFileFTLVector += "\n\r";
//								}else if (_mStyle=="data"){
//									mNum         = (uint32)(*((uint8*)(mBin.getBuffer()+i)));
//									CmFileFTLVector += mNum;
//									CmFileFTLVector += ",";
//								}
//							}
//							(*_pData) += CmFileFTLVector;
//								(*_pData) += "\n\rTimestamp: ";
//								mTimeStamp = u64MilliTimeStamp/1000;
//							(*_pData) += mTimeStamp;
//							(*_pData) += ".";
//								mTimeStamp = u64MilliTimeStamp%1000;
//							(*_pData) += mTimeStamp;
//								break;
//						}
//					}
//				}
//			}
//				break;
//			}
//			return;
//		}
//		// Meta data
//		if (_mStyle=="meta"){
//		// Adjust path
//		mCollection.resetPosition();
//			mPrefix = "";
//			while (NULL!=mCollection.getNextDelimitedString()){
//			// Split path into folders
//			mItem.resetPosition();
//			while (NULL!=mItem.getNextDelimitedString()){
//				(*_pData) += mPrefix;
//				(*_pData) += mItem;
//					(*_pData) += "\n\r";
//				mPrefix  += "  ";
//			}
//		}
//	}
//}
//	(*_pData) += "\n";
//}

