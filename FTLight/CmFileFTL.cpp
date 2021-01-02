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

#define CmFileFTL_Config
#include "FTLight/CmFileFTL.h"

// Initialize FTLight hierarchy structures

#define FTLIGHT_TEST_ROOT					"C:\\_FTLightTest"
#define FTLIGHT_TEST_IDENTIFIER		"ekd@JN58nc_Türkenfeld.FTLightTest"
#define FTLIGHT_TEST_FILENAME 		"sample.csv"
#define FTLIGHT_TEST_FILEPERIOD		(10*60)

// Initialize time period hierarchy
static struct stTimePeriodHierarchy TimePeriodHierarchy[] = {
	{  MAX_INT32, "%u",       "%u"        },
	{  MAX_INT32, "-%u",      "-%02u"	  },
	{   24*60*60, "%uth",     "-%02u"	  },		// one file per day
	{ 	   60*60, "utc%02uh", "_utc%02uh" },		// one file per hour
	{ 	      60, "%02um",    "%02um"	  },		// one file per minute
	{ 	 	   1, "%02us",    "%02us"	  },		// one file per second
	{	   	   0, "",         "(%u)"      },		// one file for each data chunk
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
    {     0, TB_EARTH, TIME MINUTE*59 ,     "%s-%02u",   0, F_SU  }, // 1 month (~8640 * 5*MINUTE)
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


//----------------------------------------------------------------------------
// PROVIDER_CmFileFTL                                             PROVIDER
//----------------------------------------------------------------------------
//
PROVIDER_CmFileFTL::PROVIDER_CmFileFTL()
// Initialize SERVICE UURI (=interface) for a 'CmFileFTL' root UURI
: CmPlugNode(UURI_SERVICE_CmFileFTL, UURI_CmFileFTL)
{
	// initialize workspace parameters
	Pro = NULL;
	Dyn = NULL;
	Ctr = NULL;
	Msg = NULL;
	Pln = NULL;
	Rdn = NULL;

	// initialize CmValueINI arrays
	CmValueINI::setDefaultInfoFTL(pro_.UURI);
	CmValueINI::setDefaultInfoFTL(dyn_.UURI);
	CmValueINI::setDefaultInfoFTL(ctr_.UURI);
	CmValueINI::setDefaultInfoFTL(msg_.UURI);
	CmValueINI::setDefaultInfoFTL(pln_.UURI);
	CmValueINI::setDefaultInfoFTL(rdn_.UURI);

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
PROVIDER_CmFileFTL::~PROVIDER_CmFileFTL()
{
	// Close data file if it is still open
	if (data_fd > 0){
		close(data_fd);
	}

	// delete config data
	NULL != Pro ? delete Pro : 0;
	NULL != Dyn ? delete Dyn : 0;
	NULL != Ctr ? delete Ctr : 0;
	NULL != Msg ? delete Msg : 0;
	NULL != Pln ? delete Pln : 0;
	NULL != Rdn ? delete Rdn : 0;
}

// test FTLight Bot
bool PROVIDER_CmFileFTL::testBot()
{
	// ToDo...

	return true;
}
// test FTLight Bot
bool PROVIDER_CmFileFTL::testFileFTL()
{
	// create root, set identifier
	setFTLightRoot(FTLIGHT_TEST_ROOT);
	if(false == setIdentifier(FTLIGHT_TEST_IDENTIFIER)) return false;

	// initialize files
	if (false == setFileName(FTLIGHT_TEST_FILENAME)) return false;
	if (false == setFilePeriod(FTLIGHT_TEST_FILEPERIOD)) return false;

	// adjust identifier from FTLight information
	if (NULL == msg().UURI.getStringINI()) return false;
	clearLogLevel();
	msg().UURI.updateInformation(*msg().UURI.getStringINI(), getReturn());
	if (false == setupUURI(msg().UURI)) return false;
	 
	// generate a data set
	const int32 Loops = 3;
	const int32 Columns = 5;
	const int32 Count = 7;
	uint64 Timestamp;

	// set header
	CmStringFTL InfoStringFTL;
	InfoStringFTL.processStringFTL(*msg().UURI.getStringINI());
	msg().UURI.syncSettings(InfoStringFTL);
	if (false == setupHeader(msg().Data, InfoStringFTL)) return false;

	// matrix access
	CmMatrix& M = msg().Data.getMatrix();

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
	for (int32 loop = 0; loop < Loops; loop++){
		// new header
		if (loop > 0 && (loop % 2 == 0)){
			CmString NewItem = "Item-";
			NewItem += loop;
			msg().Header.setText(NewItem);
			msg().UURI.syncSettings(InfoStringFTL);
			if (false == setupHeader(msg().Data, InfoStringFTL)) return false;
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
		if (false == writeData(msg().Data)) return false;
		// clear matrix
		M().clearMatrix();
	}

	// remove conversion tables
	T.clearConversionTable();

	return true;
}

bool PROVIDER_CmFileFTL::testPingData(CmMatrix& _M, int32 _Index, bool _isHeader)
{
	// write file header
	CmString UURI = _M(_Index);
	if (UURI == "EKD@JN58nc_Tuerkenfeld.FTLightPing#051" || UURI == "SUG@JO53jv_Luebeck.FTLightPing#078"){
		CmString Filename = "..\\..\\Data\\";
		Filename += UURI;
		CmString FilenameDelay(Filename);
		CmString FilenameHighMin(Filename);
		CmString FilenameHighMax(Filename);
		CmString FilenameTrvMin(Filename);
		CmString FilenameTrvMax(Filename);
		FilenameDelay += ".Delay.dat";
		FilenameHighMin += ".HighMin.dat";
		FilenameHighMax += ".HighMax.dat";
		FilenameTrvMin += ".TrvMin.dat";
		FilenameTrvMax += ".TrvMax.dat";
		if (_isHeader){
			CmString File;
			File = CmDateTime::getTimeUTC(int32(msg().Timestamp));
			File += " + t [min],Delay [ns]\n";
			File.writeBinary(FilenameDelay.getText());
			File = CmDateTime::getTimeUTC(int32(msg().Timestamp));
			File += " + t [min],HighMin\n";
			File.writeBinary(FilenameHighMin.getText());
			File = CmDateTime::getTimeUTC(int32(msg().Timestamp));
			File += " + t [min],HighMax\n";
			File.writeBinary(FilenameHighMax.getText());
			File = CmDateTime::getTimeUTC(int32(msg().Timestamp));
			File += " + t [min],TrvMin\n";
			File.writeBinary(FilenameTrvMin.getText());
			File = CmDateTime::getTimeUTC(int32(msg().Timestamp));
			File += " + t [min],TrvMax\n";
			File.writeBinary(FilenameTrvMax.getText());
		}
		else{
			CmString FileDelay;
			CmString FileHighMin;
			CmString FileHighMax;
			CmString FileTrvMin;
			CmString FileTrvMax;
			int32 Size = _M(_Index,0).getSizeLastLevel();
			CmString Timestamp, Temperature, Delay0, Delay1, HighMin0, HighMax0, TrvMin0, TrvMax0;
			double ScalingTimestamp = _M(_Index, 0);
			double ScalingTemperature = _M(_Index, 3);
			double ScalingDelay = _M(_Index, 4);
			double ScalingHighMin = _M(_Index, 5);
			double ScalingHighMax = _M(_Index, 6);
			double ScalingTrvMin = _M(_Index, 7);
			double ScalingTrvMax = _M(_Index, 8);
			for (int32 i = 0; i < Size; i++){
				double TimestampRelative = ScalingTimestamp*(double(_M(_Index, 0, i)) - double(msg().Timestamp)) / SECONDS_PER_MINUTE;
				Timestamp.double2String(TimestampRelative, 0);
				Temperature.double2String(ScalingTemperature * double(_M(_Index, 3, i)), 1);
				// delay
				double Delay = ScalingDelay * double(_M(_Index, 4, i));
				(Delay < 100) || (Delay > 2400) ? Delay = ScalingDelay * double(_M(_Index, 4, i - 1)) : 0;
				double DelayCorr = Delay - 75000 * ScalingDelay * (double(_M(_Index, 3, i)) - 300);
				Delay0.double2String(Delay, 3);
				Delay1.double2String(DelayCorr, 3);
				FileDelay += Timestamp;
				FileDelay += ",";
				FileDelay += Delay1;	// temperature corrected delay
				FileDelay += "\n";
				// high min
				double HighMin = ScalingHighMin * double(_M(_Index, 5, i));
				HighMin0.double2String(HighMin, 0);
				FileHighMin += Timestamp;
				FileHighMin += ",";
				FileHighMin += HighMin0;
				FileHighMin += "\n";
				// high max
				double HighMax = ScalingHighMax * double(_M(_Index, 6, i));
				HighMax0.double2String(HighMax, 0);
				FileHighMax += Timestamp;
				FileHighMax += ",";
				FileHighMax += HighMax0;
				FileHighMax += "\n";
				// trv min
				double TrvMin = ScalingTrvMin * double(_M(_Index, 7, i));
				TrvMin0.double2String(TrvMin, 0);
				FileTrvMin += Timestamp;
				FileTrvMin += ",";
				FileTrvMin += TrvMin0;
				FileTrvMin += "\n";
				// trv max
				double TrvMax = ScalingTrvMax * double(_M(_Index, 8, i));
				TrvMax0.double2String(TrvMax, 0);
				FileTrvMax += Timestamp;
				FileTrvMax += ",";
				FileTrvMax += TrvMax0;
				FileTrvMax += "\n";
			}
			FileDelay.appendBinary(FilenameDelay.getText());
			FileHighMin.appendBinary(FilenameHighMin.getText());
			FileHighMax.appendBinary(FilenameHighMax.getText());
			FileTrvMin.appendBinary(FilenameTrvMin.getText());
			FileTrvMax.appendBinary(FilenameTrvMax.getText());
		}
	}

	return true;
}

//------Remote-service-access-------------------------------------PROVIDER----
bool PROVIDER_CmFileFTL::processInformation(CmString& _Information)
{
	// update local profile
	CmValueFTL InfoFTL;
	InfoFTL.processStringFTL(_Information);

	// ToDo: process information

	return true;
}

//------Background-data-processing--------------------------------PROVIDER----
bool PROVIDER_CmFileFTL::runParallel()
{
	enterSerialize();

	// ToDo...

	leaveSerialize();

	return true;
}

//------Bot-functionality-----------------------------------------PROVIDER----

bool PROVIDER_CmFileFTL::putFTLightData(CmValueINI& _UURI, CmValueINI& /*_Return*/)
{
	// Data:::vector of U matrices with UURI strings at position [`]
	// U:::array of C columns with M measure values, [`] = UURI, [c] = base value/format descriptor, [0, m] = timestamp, [c, m] = measure values
	//       management: [0, 0, 0] = header, [0, 0, 1] = begin of period, [0, 0, 2] = end of period

	// find Data item
	CmValueFTL* Data = &_UURI;
	while (Data->allValuesFTL(&Data)){
		if (*Data == "Data") break;
	}
	if (NULL == Data) return false; // Data item not found

	// NOTE: going reference-pointer-reference caused the MSVC compiler to ignore the high part of 64-bit values
	//CmValueFTL& Item = *Data; DEPRECATED!

	// get Data matrix
	CmMatrix& M = Data->getMatrix();
	CmValueFTL* ValueFTL;

	// find existing or create a new UURI matrix node
	//CmMatrix& U = M.findScalarItem(_UURI.getText());

	int32 u;
	int32 VectorLength = M.getLength();
	for (u = 0; u < VectorLength; u++){
		CmString Item = M(u);
		if (Item == _UURI.getText()) break;
	}
	if (u >= VectorLength){
		// insert new item
		M(u) = _UURI.getText();

		// test ping headers
		testPingData(M, u, true);
	}

	// check if the Data matrix has been initialized already
	if (0 == M(u).getSizeLastLevel()){
		// check StringINI availability
		if (_UURI.getStringINI() == NULL) return false;
		// initialize Data matrix: put raw values as double to M(u,i)
		ValueFTL = Data;
		for (int i = 0; ValueFTL->allValuesFTL(&ValueFTL); i++){
			double Val;
			M(u,i) = Val = double(*ValueFTL);
		}
		// restore base values
		CmValueINI Return;
		_UURI.setDefaultInfoFTL(_UURI, Return);
		// exchange U(c) by base value(c), estimate and write relative measure value(c) to first data row U(c,0) where c is the channel
		ValueFTL = Data;
		for (int c = 0; ValueFTL->allValuesFTL(&ValueFTL); c++){
			double Value = M(u,c);
			double Base = ValueFTL->getNumAsDouble();
			Base == 0 ? Base = 1 : 0;
			M(u,c) = Base;
			int64 Val;
			M(u, c, 0) = Val = int64(Value / Base);
		}
		// determine file start and end time based on period
		double AcquisitionTimestamp = double(M(u, 1, 0)) * double(M(u, 1));
		int32 IndexPeriod = 2;
		ValueFTL = _UURI.getConfigValue(IndexPeriod);
		if (0 != IndexPeriod) return false;
		double Period = ValueFTL->getNumAsDouble();
		if (0 == Period) return false;
		double Val;
		M(u, 0, 0, 1) = Val = Period * floor(AcquisitionTimestamp / Period);
		M(u, 0, 0, 2) = Val = Period * (floor(AcquisitionTimestamp / Period) + 1);
	}
	else{
		// check if the acquisition timestamp belongs to current period
		int32 IndexAcquisition = 2;
		CmValueFTL* AcquisitionTimestamp = Data->getConfigValue(IndexAcquisition);
		if (0 != IndexAcquisition) return false;
		double Timestamp = *AcquisitionTimestamp;
		if (Timestamp >= double(M(u, 0, 0, 1)) && Timestamp < double(M(u, 0, 0, 2)))
		{
			// add new data set to data matrix
			int32 Index = M(u, 0).getSizeLastLevel();
			// estimate and write relative measure values(c) to next data row M(u, c,Index)
			ValueFTL = Data;
			for (int c = 0; ValueFTL->allValuesFTL(&ValueFTL); c++){
				double Value = double(*ValueFTL);
				double Base = M(u, c);
				int64 Val;
				M(u, c, Index) = Val = int64(Value / Base);
			}
		}
		else{
			// test ping data
			testPingData(M,u);

			// write current dataset to disk


			// tbd.

			// clear matrix
			M(u).clearMatrix();
			// restore UURI
			M(u) = _UURI.getText();

		}
	}











	//// TEST: write data to disk
	//CmStringFTL DataFTL;
	//DataFTL.processStringFTL(*_UURI.StringINI);
	//_UURI.writeInfoFTL("./", DataFTL, _Return);

	return true;
}

// FTLight identifier (UURI)
bool PROVIDER_CmFileFTL::setIdentifier(const int8 *_Identifier)
{
	Identifier.setUURI(NULL, _Identifier);

	// Extract location from identifier
	CmString Location = Identifier.getLocation();

	// Initialize device dir
	return initFTLightDir(Location);
}

// FTLight folders
bool PROVIDER_CmFileFTL::initFTLightDir(const CmString& _Location)
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
bool PROVIDER_CmFileFTL::setFTLightRoot(const int8 *_FTLightRoot)
{
	FTLightRoot = _FTLightRoot;

	return true;
}
bool PROVIDER_CmFileFTL::setFileName(const int8 *_FileName)
{
	FileName = _FileName;
	return true;
}
bool PROVIDER_CmFileFTL::setFilePeriod(uint32 _FilePeriod)
{
	FilePeriod = _FilePeriod;
	return true;
}
const char * PROVIDER_CmFileFTL::getFTLightroot()
{
	return FTLightRoot.getText();
}
const char * PROVIDER_CmFileFTL::getIdentifier()
{
	return Identifier.getText();
}
const char * PROVIDER_CmFileFTL::getFileName()
{
	return FileName.getText();
}
uint32 PROVIDER_CmFileFTL::getFilePeriod()
{
  return FilePeriod;
}

// setup FTLight information
bool PROVIDER_CmFileFTL::setupUURI(CmValueFTL& _InfoUURI)
{
	if (false == setIdentifier(_InfoUURI.asStringConvert().getText())) return false;

	return true;
}
bool PROVIDER_CmFileFTL::setupHeader(CmValueFTL& _Data, CmStringFTL& _InfoStringFTL)
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
bool PROVIDER_CmFileFTL::writeData(CmValueFTL& _Data)

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
bool PROVIDER_CmFileFTL::readData(CmValueFTL& _Data, CmStringFTL& _StringFTL, CmString _AddrData, int32& _IndexCount)
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
bool PROVIDER_CmFileFTL::writeData(time_t _SampleTime, CmString& _FTLight, int _FileSeqNum)
{
	return writeData(_SampleTime, _FTLight.getBuffer(), (int32)_FTLight.getLength(), _FileSeqNum);
}
bool PROVIDER_CmFileFTL::writeData(uint64 _u64SampleTime, CmString& _FTLight, int _FileSeqNum)
{
	return writeData((time_t)(_u64SampleTime / DATETIME_NANOSECONDS), _FTLight.getBuffer(), (int32)_FTLight.getLength(), _FileSeqNum);
}
bool PROVIDER_CmFileFTL::writeData(CmTimestamp _StartTime, const CmString& _FTLight, int _FileSeqNum)
{
	return writeData((time_t)(_StartTime.getTimestamp_s()), _FTLight.getBuffer(), (int32)_FTLight.getLength(), _FileSeqNum);
}
bool PROVIDER_CmFileFTL::writeData(uint64 _u64SampleTime, char* _Data, int _DataLen, int _FileSeqNum)
{
	return writeData((time_t)(_u64SampleTime / DATETIME_NANOSECONDS), _Data, _DataLen, _FileSeqNum);
}
bool PROVIDER_CmFileFTL::writeData(time_t _SampleTime, char* _Data, int _DataLen, int _FileSeqNum)
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
bool PROVIDER_CmFileFTL::readData(time_t _SampleTime, CmString* _pData)
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
		return false;
	}
	// Get data from file into memory
	uFileSize = read(data_fd, _pData->getBuffer(), uFileSize);
	close(data_fd);
	data_fd = 0;

	return true;
}
bool PROVIDER_CmFileFTL::closeFTLightFile()
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
uint64 PROVIDER_CmFileFTL::setStartTime(CmTimestamp _StartTime, bool _fAdjust)
{
	StartTime.setTimestamp(_StartTime.getTimestamp_ns());
	if (true == _fAdjust){
		EndTime.setTimestamp(StartTime.getTimestamp_ns() + StreamLength_s * DATETIME_NANOSECONDS);
	}
	FilePeriod > 0 ? FileTime.setTimestamp(StartTime.getTimestamp_ns() - (StartTime.getTimestamp_s() % FilePeriod) * DATETIME_NANOSECONDS) : FileTime.setTimestamp(StartTime.getTimestamp_ns());
	return StartTime.getTimestamp_ns();
}
uint64 PROVIDER_CmFileFTL::setEndTime(CmTimestamp _EndTime, bool _fAdjust)
{
	EndTime.setTimestamp(_EndTime.getTimestamp_ns());
	if (true == _fAdjust){
		StartTime.setTimestamp(EndTime.getTimestamp_ns() - StreamLength_s * DATETIME_NANOSECONDS);
	}
	FilePeriod > 0 ? FileTime.setTimestamp(StartTime.getTimestamp_ns() - (StartTime.getTimestamp_s() % FilePeriod) * DATETIME_NANOSECONDS) : FileTime.setTimestamp(StartTime.getTimestamp_ns());
	return EndTime.getTimestamp_ns();
}
uint64 PROVIDER_CmFileFTL::setStreamLength_s(uint64 _StreamLength_s)
{
	StreamLength_s = _StreamLength_s;
	return StreamLength_s;
}
uint64 PROVIDER_CmFileFTL::getStartTime()
{
	return StartTime.getTimestamp_ns();
}
uint64 PROVIDER_CmFileFTL::getEndTime()
{
	return EndTime.getTimestamp_ns();
}
uint64 PROVIDER_CmFileFTL::getStreamLength_s()
{
	return (EndTime.getTimestamp_s() - StartTime.getTimestamp_s());
}

// access files between start and end time
bool PROVIDER_CmFileFTL::getStartFiles(CmLString& _StartFiles)
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
bool PROVIDER_CmFileFTL::getNextFileFTL(CmStringFTL& _StringFTL)
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

	return true;
}

bool PROVIDER_CmFileFTL::openTimePeriodFile(time_t _SampleTime, bool _fCreate, int _FileSeqNum)
{
	CmString mNum;
	CmString mMeta;
	CmString mParams;
	CmString mParamsFile;

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
	if (-1 == chdir(FTLightDir.getTextDir().getText())){
		if (true == _fCreate){
			if (-1 == MKDIR(FTLightDir.getTextDir().getText(), S_MASK)){
				return false;
			}
			if (-1 == chdir(FTLightDir.getTextDir().getText())){
				return false;
			}
		}
		else{
			return false;
		}
	}
	// Enter (create) subsequent directories in data storage
	try{
		enterSubDirectory(TimePeriodHierarchy, TimeVal, Prefix, _fCreate);
	}
	catch (CmException e){
		// Change back to initial dir
		if (-1 == chdir(InitialDir.getTextDir().getText())){
			return false;
		}
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
					return false;
				}
			}
		}
	else{
		// Open data file for reading
#ifdef MSVS
		_sopen_s(&data_fd, Prefix.getText(), O_RDONLY, _SH_DENYWR, 0);
		if (-1 == data_fd){
			return false;
		}
#else
		if (-1 == (data_fd = OPEN(Prefix.getText(), O_RDONLY, 0))){
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

bool PROVIDER_CmFileFTL::enterSubDirectory(struct stTimePeriodHierarchy* _pTimePeriodHierarchy, unsigned int* _pTimeVal, CmString& _Prefix, bool _fCreate)
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
				return false;
			}
			if (-1==chdir(dir)){
				return false;
			}
		}else{
			return false;
		}
	}
	// Check if another directory level is needed
	if (FilePeriod<_pTimePeriodHierarchy->period){
		return enterSubDirectory(_pTimePeriodHierarchy+1,_pTimeVal+1,_Prefix,_fCreate);
	}
	return true;
}
CmString* PROVIDER_CmFileFTL::getDataVector(uint64 _u64SampleTime, CmString* _pDataVector, FTLight_DATA_TYPE_IDENTIFIER _eFTLightIdentifier)
{
	uint32    uFileTimestamp;
	CmString  mTimeStamp;
	CmString  mFile;
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

//------CONFIGURATION-MANAGEMENT-functions------------------------PROVIDER----
CmFileFTLProfile& PROVIDER_CmFileFTL::pro()
{
	return newConfig<CmFileFTLProfile>(&Pro, pro_);
}
bool PROVIDER_CmFileFTL::updateProfile(CmStringFTL& _ProFTL)
{
	return pro().UURI.updateInfoFTL(_ProFTL, getReturn());
}
bool PROVIDER_CmFileFTL::writeProfile(CmString _ConfigPath)
{
	CmStringFTL ProFTL;
	NULL != pro().UURI.getStringINI() ? ProFTL.processStringFTL(*pro().UURI.getStringINI()) : 0;
	return pro().UURI.writeInfoFTL(_ConfigPath, ProFTL, getReturn());
}
bool PROVIDER_CmFileFTL::readProfile(CmString _ConfigPath)
{
	if (false == pro().UURI.readInfoFTL(_ConfigPath, getReturn())) return false;
	return true;
}
bool PROVIDER_CmFileFTL::setDefaultProfile()
{
	return CmValueINI::setDefaultInfoFTL(pro().UURI, getReturn());
}

// Dynamic
CmFileFTLDynamic& PROVIDER_CmFileFTL::dyn()
{
	return newConfig<CmFileFTLDynamic>(&Dyn, dyn_);
}
bool PROVIDER_CmFileFTL::updateDynamic(CmStringFTL& _DynFTL)
{
	return dyn().UURI.updateInfoFTL(_DynFTL, getReturn());
}
bool PROVIDER_CmFileFTL::writeDynamic(CmString _ConfigPath)
{
	CmStringFTL DynFTL;
	NULL != dyn().UURI.getStringINI() ? DynFTL.processStringFTL(*dyn().UURI.getStringINI()) : 0;
	return dyn().UURI.writeInfoFTL(_ConfigPath, DynFTL, getReturn());
}

// Control
CmFileFTLControl& PROVIDER_CmFileFTL::ctr()
{
	return newConfig<CmFileFTLControl>(&Ctr, ctr_);
}
bool PROVIDER_CmFileFTL::updateControl(CmStringFTL& _CtrFTL)
{
	return ctr().UURI.updateInfoFTL(_CtrFTL, getReturn());
}
bool PROVIDER_CmFileFTL::writeControl(CmString _ConfigPath)
{
	CmStringFTL CtrFTL;
	NULL != ctr().UURI.getStringINI() ? CtrFTL.processStringFTL(*ctr().UURI.getStringINI()) : 0;
	return ctr().UURI.writeInfoFTL(_ConfigPath, CtrFTL, getReturn());
}

// Message
CmFileFTLMessage& PROVIDER_CmFileFTL::msg()
{
	return newConfig<CmFileFTLMessage>(&Msg, msg_);
}
bool PROVIDER_CmFileFTL::updateMessage(CmStringFTL& _MsgFTL)
{
	return msg().UURI.updateInfoFTL(_MsgFTL, getReturn());
}
bool PROVIDER_CmFileFTL::writeMessage(CmString _ConfigPath)
{
	CmStringFTL MsgFTL;
	NULL != msg().UURI.getStringINI() ? MsgFTL.processStringFTL(*msg().UURI.getStringINI()) : 0;
	return msg().UURI.writeInfoFTL(_ConfigPath, MsgFTL, getReturn());
}

// Polling
CmFileFTLPolling& PROVIDER_CmFileFTL::pln()
{
	return newConfig<CmFileFTLPolling>(&Pln, pln_);
}
bool PROVIDER_CmFileFTL::updatePolling(CmStringFTL& _PlnFTL)
{
	return pln().UURI.updateInfoFTL(_PlnFTL, getReturn());
}
bool PROVIDER_CmFileFTL::writePolling(CmString _ConfigPath)
{
	CmStringFTL PlnFTL;
	NULL != pln().UURI.getStringINI() ? PlnFTL.processStringFTL(*pln().UURI.getStringINI()) : 0;
	return pln().UURI.writeInfoFTL(_ConfigPath, PlnFTL, getReturn());
}

// Reading
CmFileFTLReading& PROVIDER_CmFileFTL::rdn()
{
	return newConfig<CmFileFTLReading>(&Rdn, rdn_);
}
bool PROVIDER_CmFileFTL::updateReading(CmStringFTL& _RdnFTL)
{
	return rdn().UURI.updateInfoFTL(_RdnFTL, getReturn());
}
bool PROVIDER_CmFileFTL::writeReading(CmString _ConfigPath)
{
	CmStringFTL RdnFTL;
	NULL != rdn().UURI.getStringINI() ? RdnFTL.processStringFTL(*rdn().UURI.getStringINI()) : 0;
	return rdn().UURI.writeInfoFTL(_ConfigPath, RdnFTL, getReturn());
}

// Diagnostics
bool PROVIDER_CmFileFTL::clearLogLevel()
{
	// clear log/message/context
	dyn().LogLevel = CMLOG_None;
	CmString(dyn().Message) = "";
	CmString(dyn().Context) = "";

	return true;
}
CmValueINI& PROVIDER_CmFileFTL::getReturn()
{
	// return first item of a LogLevel/Message/Context config sequence
	return dyn().LogLevel;
}
// UURI
CmUURI& PROVIDER_CmFileFTL::getUURI()
{
	// Actual PROVIDER implementation of the 'getUURI()' SERVICE function
	return ServiceUURI;
}


//----------------------------------------------------------------------------
// BOT_CmFileFTL
//----------------------------------------------------------------------------
//
BOT_CmFileFTL::BOT_CmFileFTL(const int8 *_Init)
	:CmValueINI(_Init)
{
	// assign bot name
	CmString BotName = getText();
	setBotName(BotName);

}
BOT_CmFileFTL::~BOT_CmFileFTL()
{

}

//----------------------------------------------------------------------------
// SERVICE_CmFileFTL
//----------------------------------------------------------------------------
//
SERVICE_CmFileFTL::SERVICE_CmFileFTL()
// Initialize SERVICE UURI (=interface) for a 'CmFileFTL' root UURI
: CmPlugNode(UURI_SERVICE_CmFileFTL, UURI_CmFileFTL)
{
	// Initialize
	LocalProvider = NULL;
}

SERVICE_CmFileFTL::~SERVICE_CmFileFTL()
{
	// Delete a local SERVICE implementation (PROVIDER) if it exists
	if (NULL != LocalProvider)
		delete LocalProvider;
}

//------Bot-test--------------------------------------------------------------
bool SERVICE_CmFileFTL::testBot()
{
	return Provider().testBot();
}
bool SERVICE_CmFileFTL::testFileFTL()
{
	return Provider().testFileFTL();
}
bool SERVICE_CmFileFTL::testPingData(CmMatrix& _M, int32 _Index, bool _isHeader)
{
	return Provider().testPingData(_M, _Index, _isHeader);
}

//------Remote-service-access-------------------------------------------------
bool SERVICE_CmFileFTL::processInformation(CmString& _Information)
{
	return Provider().processInformation(_Information);
}

//------Bot-functionality-----------------------------------------------------

// putFTLightData.
bool SERVICE_CmFileFTL::putFTLightData(CmValueINI& _UURI, CmValueINI& _Return)
{
	return Provider().putFTLightData(_UURI, _Return);
}

// FTLight identifier (UURI)
bool SERVICE_CmFileFTL::setIdentifier(const int8 *_Identifier)
{
	return Provider().setIdentifier(_Identifier);
}

// FTLight folders
bool SERVICE_CmFileFTL::setFTLightRoot(const int8 *_FTLightroot)
{
	return Provider().setFTLightRoot(_FTLightroot);
}
bool SERVICE_CmFileFTL::initFTLightDir(const CmString& _Location)
{
	return Provider().initFTLightDir(_Location);
}
bool SERVICE_CmFileFTL::setFileName(const int8 *_FileName)
{
	return Provider().setFileName(_FileName);
}
bool SERVICE_CmFileFTL::setFilePeriod(uint32 _FilePeriod)
{
	return Provider().setFilePeriod(_FilePeriod);
}
const char * SERVICE_CmFileFTL::getFTLightroot()
{
	return Provider().getFTLightroot();
}
const char * SERVICE_CmFileFTL::getIdentifier()
{
	return Provider().getIdentifier();
}
const char * SERVICE_CmFileFTL::getFileName()
{
	return Provider().getFileName();
}
uint32 SERVICE_CmFileFTL::getFilePeriod()
{
	return Provider().getFilePeriod();
}

// setup FTLight information
bool SERVICE_CmFileFTL::setupUURI(CmValueFTL& _InfoUURI)
{
	return Provider().setupUURI(_InfoUURI);
}
bool SERVICE_CmFileFTL::setupHeader(CmValueFTL& _Data, CmStringFTL& _InfoStringFTL)
{
	return Provider().setupHeader(_Data, _InfoStringFTL);
}

// write/readData.
bool SERVICE_CmFileFTL::writeData(CmValueFTL& _Data)
{
	return Provider().writeData(_Data);
}
bool SERVICE_CmFileFTL::readData(CmValueFTL& _Data, CmStringFTL& _StringFTL, CmString _AddrData, int32& _IndexCount)
{
	return Provider().readData(_Data, _StringFTL, _AddrData, _IndexCount);
}
bool SERVICE_CmFileFTL::writeData(time_t _SampleTime, CmString& _Data, int _FileSeqNum)
{
	return Provider().writeData(_SampleTime, _Data, _FileSeqNum);
}
bool SERVICE_CmFileFTL::writeData(uint64 _SampleTime, CmString& _Data, int _FileSeqNum)
{
	return Provider().writeData(_SampleTime, _Data, _FileSeqNum);
}
bool SERVICE_CmFileFTL::writeData(CmTimestamp _SampleTime, const CmString& _Data, int _FileSeqNum)
{
	return Provider().writeData(_SampleTime, _Data, _FileSeqNum);
}
bool SERVICE_CmFileFTL::writeData(time_t _SampleTime, char* _Data, int _DataLen, int _FileSeqNum)
{
	return Provider().writeData(_SampleTime, _Data, _DataLen, _FileSeqNum);
}
bool SERVICE_CmFileFTL::writeData(uint64 _SampleTime, char* _Data, int _DataLen, int _FileSeqNum)
{
	return Provider().writeData(_SampleTime, _Data, _DataLen, _FileSeqNum);
}
bool SERVICE_CmFileFTL::readData(time_t _SampleTime, CmString* _pData)
{
	return Provider().readData(_SampleTime, _pData);
}
bool SERVICE_CmFileFTL::closeFTLightFile()
{
	return Provider().closeFTLightFile();
}

// set FTLight stream parameters
uint64 SERVICE_CmFileFTL::setStartTime(CmTimestamp _StartTime, bool _fAdjust)
{
	return Provider().setStartTime(_StartTime, _fAdjust);
}
uint64 SERVICE_CmFileFTL::setEndTime(CmTimestamp _EndTime, bool _fAdjust)
{
	return Provider().setEndTime(_EndTime, _fAdjust);
}
uint64 SERVICE_CmFileFTL::setStreamLength_s(uint64 _StreamLength)
{
	return Provider().setStreamLength_s(_StreamLength);
}
uint64 SERVICE_CmFileFTL::getStartTime()
{
	return Provider().getStartTime();
}
uint64 SERVICE_CmFileFTL::getEndTime()
{
	return Provider().getEndTime();
}
uint64 SERVICE_CmFileFTL::getStreamLength_s()
{
	return Provider().getStreamLength_s();
}

// access files between start time and end time
bool SERVICE_CmFileFTL::getStartFiles(CmLString& _StartFiles)
{
	return Provider().getStartFiles(_StartFiles);
}
bool SERVICE_CmFileFTL::getNextFileFTL(CmStringFTL& _StringFTL)
{
	return Provider().getNextFileFTL(_StringFTL);
}

//------CONFIGURATION-MANAGEMENT-functions------------------------------------

// profile
CmFileFTLProfile& SERVICE_CmFileFTL::pro()
{
	return Provider().pro();
}
bool SERVICE_CmFileFTL::updateProfile(CmStringFTL& _ProFTL)
{
	return Provider().updateProfile(_ProFTL);
}
bool SERVICE_CmFileFTL::writeProfile(CmString _ConfigPath)
{
	return Provider().writeProfile(_ConfigPath);
}
bool SERVICE_CmFileFTL::readProfile(CmString _ConfigPath)
{
	return Provider().readProfile(_ConfigPath);
}
bool SERVICE_CmFileFTL::setDefaultProfile()
{
	return Provider().setDefaultProfile();
}
// dynamic
CmFileFTLDynamic& SERVICE_CmFileFTL::dyn()
{
	return Provider().dyn();
}
bool SERVICE_CmFileFTL::updateDynamic(CmStringFTL& _dyn)
{
	return Provider().updateDynamic(_dyn);
}
bool SERVICE_CmFileFTL::writeDynamic(CmString _ConfigPath)
{
	return Provider().writeDynamic(_ConfigPath);
}
// control
CmFileFTLControl& SERVICE_CmFileFTL::ctr()
{
	return Provider().ctr();
}
bool SERVICE_CmFileFTL::updateControl(CmStringFTL& _ctr)
{
	return Provider().updateControl(_ctr);
}
bool SERVICE_CmFileFTL::writeControl(CmString _ConfigPath)
{
	return Provider().writeControl(_ConfigPath);
}
// message
CmFileFTLMessage& SERVICE_CmFileFTL::msg()
{
	return Provider().msg();
}
bool SERVICE_CmFileFTL::updateMessage(CmStringFTL& _msg)
{
	return Provider().updateMessage(_msg);
}
bool SERVICE_CmFileFTL::writeMessage(CmString _ConfigPath)
{
	return Provider().writeMessage(_ConfigPath);
}
// polling
CmFileFTLPolling& SERVICE_CmFileFTL::pln()
{
	return Provider().pln();
}
bool SERVICE_CmFileFTL::updatePolling(CmStringFTL& _pln)
{
	return Provider().updatePolling(_pln);
}
bool SERVICE_CmFileFTL::writePolling(CmString _ConfigPath)
{
	return Provider().writePolling(_ConfigPath);
}
// reading
CmFileFTLReading& SERVICE_CmFileFTL::rdn()
{
	return Provider().rdn();
}
bool SERVICE_CmFileFTL::updateReading(CmStringFTL& _rdn)
{
	return Provider().updateReading(_rdn);
}
bool SERVICE_CmFileFTL::writeReading(CmString _ConfigPath)
{
	return Provider().writeReading(_ConfigPath);
}
// synchronization / diagnostics 
bool SERVICE_CmFileFTL::enterSerialize(int32 _LockID)
{
	// lock provider and service
	if (false == Provider().enterSerialize(_LockID)) return false;
	return CmParallelFTL::enterSerialize(_LockID);
}
bool SERVICE_CmFileFTL::leaveSerialize()
{
	// unlock provider and service
	if (false == Provider().leaveSerialize()) return false;
	return CmParallelFTL::leaveSerialize();
}
bool SERVICE_CmFileFTL::clearLogLevel()
{
	return Provider().clearLogLevel();
}
CmString& SERVICE_CmFileFTL::setBotName(CmString _BotName)
{
	// store service UURI and bot name in the SERVICE plugnode
	CmPlugNode::ServiceUURI = Provider().getUURI().getText();
	CmPlugNode::setBotName(_BotName);

	return CmPlugNode::getBotUURI();
}
CmString& SERVICE_CmFileFTL::getBotUURI()
{
	return CmPlugNode::getBotUURI();
}
CmUURI& SERVICE_CmFileFTL::getUURI()
{
	return Provider().getUURI();
}

//------Service-access-by-PROVIDER--------------------------------------------

PROVIDER_CmFileFTL& SERVICE_CmFileFTL::Provider()
{
	// Instantiate a PROVIDER class if it does not exist yet
	if (NULL == ServiceProvider)
	{
		LocalProvider = new PROVIDER_CmFileFTL();
		ServiceProvider = LocalProvider;
	}

	// Throw exception in case of memory problems
	if (NULL == ServiceProvider)
		throw(0);

	return *(PROVIDER_CmFileFTL *)ServiceProvider;
}

