//////////////////////////////////////////////////////////////////////////////
//
// CmStringFTL.cpp: Implementation of the CmStringFTL class
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

#define CM_TYPE_OLD_STYLE

#include "FTLight/CmStringFTL.h"
#include "Shlwapi.h"

/*------CmStringFTL--------------------ToDo-----------------------------------
*
*  - process IP as text, e.g. 192.168.42.30
*  - configuration of "0.02" failed
*
*/

/** BinX conversion */
uint8* BINX = NULL;
uint8* SYMBOL = NULL;
const int32 BinXconvertSize = 256;		    // number of entries in the BinX conversion table

/** BinMCL conversion */
uint16* BinMCLconvert = NULL;							// BinMCL conversion table
const int32 BinMCLconvertSize = 32768;		// number of entries in the BinMCL conversion table

/** CmStringFTL bookkeeping */
int32 CountStringFTL = 0;

//----------------------------------------------------------------------------
// CmParallelFTL class
//----------------------------------------------------------------------------
//

// static CmStringFTL class for logging purposes
static CmStringFTL *Logger = NULL;
static CmLogLevel MaxLogLevel = CMLOG_Error;
static CmParallelFTL LogAccess;

CmParallelFTL::CmParallelFTL()
{
	// initialize workspace
	ThreadHandle = 0;
	ThreadID = 0;
	isThreadShutdown = false;
	InitializeCriticalSection(&CriticalSection);

}
CmParallelFTL::~CmParallelFTL()
{
	// stop thread function should it still be running
	stopParallel();

	// release critical section resources
	DeleteCriticalSection(&CriticalSection);
}
void CmParallelFTL::startParallel(int64 _LoopCount)
{
	// store loop count
	LoopCount = _LoopCount;

	// start worker thread
	LockID = 0;
	isThreadShutdown = false;
	LPSECURITY_ATTRIBUTES ThreadAtributes = NULL;
	SIZE_T StackSize = 0;
	LPVOID Context = this;
	DWORD CreationFlag = 0;
	ThreadHandle = CreateThread(ThreadAtributes, StackSize, StaticThreadStart, Context, CreationFlag, &ThreadID);
	LOG3("ID=", CmString::UInt2Hex((uint64)ThreadHandle), " ThreadHandle opened", Msg1, CMLOG_Resource)
}
DWORD WINAPI CmParallelFTL::StaticThreadStart(void* _Param)
{
	// restore object pointer
	CmParallelFTL* This = (CmParallelFTL*)_Param;

	// run background function continuously or for a specified number of times
	This->isThreadFinished = false;
	This->isThreadPending = false;
	// exit with 0 if succeeded, otherwise with 1
	if (This->LoopCount <= 0){
		for (;;){
			if (This->isThreadShutdown) break;
			if (false == This->isRunningParallel()) break;
			if (false == This->runParallel()){
				This->isThreadPending = true;
				return 1;
			}
		}
	}
	else{
		for (int i = 0; i < This->LoopCount; i++){
			if (This->isThreadShutdown) break;
			if (false == This->isRunningParallel())	break;
			if (false == This->runParallel()){
				This->isThreadPending = true;
				return 1;
			}
		}
	}

	// thread has finished successfully
	This->isThreadFinished = true;
	return 0;
}
bool CmParallelFTL::runParallel()
{
	Sleep(40);	// ~25fps
	return true;
}
bool CmParallelFTL::isRunningParallel()
{
	// thread is not running if its thread handle is 0
	if (0 == ThreadHandle) return false;

	// thread is not running if the OS does not report 'STILL_ACTIVE'
	DWORD ExitCode = 0;
	GetExitCodeThread(ThreadHandle, &ExitCode);
	if (ExitCode != STILL_ACTIVE)	return false;

	// deactivated for runtime reasons, activate only if needed
	//LOG3("ID=", CmString::UInt2Hex(uint64(ThreadHandle)), " ThreadHandle still running", Msg1, CMLOG_Resource)

	// thread is not running if its thread function has been left
	if (isThreadFinished || isThreadPending)
		return false;

	return true;
}
bool CmParallelFTL::stopParallel(int32 _Timeout_ms, int32 _WaitStep)
{
	bool hasShutdownGracefully = true;

	// check if the thread is down
	if (0 == ThreadHandle) return true;

	// wait until thread went down if it is not pending already
	isThreadShutdown = true;
	if (false == isThreadPending){
		for (int32 t = 0; t < _Timeout_ms; t += _WaitStep){
			// ToDo: fix a block that occurs sporadically
			Sleep(_WaitStep);
			if (isThreadFinished)
				break;
		}
	}

	// check if thread is still running
	if (isRunningParallel()){
		hasShutdownGracefully = false;
		LOG3("ID=", CmString::UInt2Hex((uint64)ThreadHandle), " Thread did not go down", Msg1, CMLOG_Error)
	}
	else{
		// close thread handle
		CloseHandle(ThreadHandle);
		LOG3("ID=", CmString::UInt2Hex((uint64)ThreadHandle), " ThreadHandle closed", Msg1, CMLOG_Resource)

			//clear thread resources
			ThreadHandle = 0;
		ThreadID = 0;
	}

	return hasShutdownGracefully;
}
bool CmParallelFTL::isThreadDown()
{
	return isThreadShutdown;
}
bool CmParallelFTL::enterSerialize(int32 _LockID)
{
	// wait for access to a critical section
	EnterCriticalSection(&CriticalSection);

	// remember active lock
	LockID = _LockID;

	return true;
}
bool CmParallelFTL::leaveSerialize()
{
	// clear lock
	LockID = 0;

	// leave a critical section
	LeaveCriticalSection(&CriticalSection);

	return true;
}
bool CmParallelFTL::trySerialize(int32 _LockID)
{
	// wait for access to a critical section
	if (false == TryEnterCriticalSection(&CriticalSection)) return false;

	// remember active lock
	LockID = _LockID;

	return true;
}
bool CmParallelFTL::isLocked(){
	return LockID == 0 ? false : true;
}
void CmParallelFTL::setLogLevel(CmLogLevel _LogLevel)
{
	MaxLogLevel = _LogLevel;
}

bool CmParallelFTL::log(const CmString& _Message, CmLogLevel _LogLevel, const CmString* _Context)
{
	// reject non-relevant logging
	if (_LogLevel > MaxLogLevel) return false;

	// create a logger if it does not exist yet
	CmStringFTL *Location = NULL;
	CmStringFTL *UURI = NULL;
	CmStringFTL *Created = NULL;
	CmStringFTL *LogLevel = NULL;
	CmString TimestampCreated;
	if (NULL == Logger){
		LogAccess.enterSerialize();

		// create a new logger
		Logger = CmString::allocateMemory<CmStringFTL>(1, isCmStringFTL);
		NULL != Logger ? Logger->addChild(&Location) : 0;
		NULL != Location ? Location->addChild(&UURI) : 0;
		NULL != UURI ? UURI->addChild(&Created) : 0;
		NULL != Created ? Created->addChild(&LogLevel) : 0;
		// timestamp created
		TimestampCreated = CmDateTime::getTimeUTC(0, -1, true); // true= timestamp without colon
		NULL != Created ? Created->setText(TimestampCreated) : 0;
		// log level
		CmString MsgLogLevel;
		switch (MaxLogLevel){
		case CMLOG_None: MsgLogLevel = "CMLOG_None"; break;
		case CMLOG_Error: MsgLogLevel = "CMLOG_Error"; break;
		case CMLOG_Info: MsgLogLevel = "CMLOG_Info"; break;
		case CMLOG_Control: MsgLogLevel = "CMLOG_Control"; break;
		case CMLOG_Process: MsgLogLevel = "CMLOG_Process"; break;
		case CMLOG_Network: MsgLogLevel = "CMLOG_Network"; break;
		case CMLOG_Connection: MsgLogLevel = "CMLOG_Connection"; break;
		case CMLOG_PlugNodes: MsgLogLevel = "CMLOG_PlugNodes"; break;
		case CMLOG_Resource: MsgLogLevel = "CMLOG_Resource"; break;
		default: MsgLogLevel = "CMLOG?"; break;
		}
		NULL != LogLevel ? LogLevel->setText(MsgLogLevel) : 0;

		LogAccess.leaveSerialize();
	}
	NULL != Logger ? Location = Logger->getChild() : 0;
	NULL != Location ? UURI = Location->getChild() : 0;
	NULL != UURI ? Created = UURI->getChild() : 0;
	NULL != Created ? LogLevel = UURI->getChild() : 0;
	if (NULL == LogLevel) return false;

	// timestamp
	int64 SysClockNanoSec = CmDateTime::getSysClockNanoSec(false); // get absolute time
	CmString MsgTimestamp = CmDateTime::getTimestamp(SysClockNanoSec, 0, true, true);

	LogAccess.enterSerialize();

	// insert new message into logger
	CmStringFTL *Timestamp;
	CmStringFTL *Message;
	//CmStringFTL *LogLevel;
	CmStringFTL *Context;
	if (false == UURI->addChild(&Timestamp)) return false;
	if (false == Timestamp->addChild(&Message)) return false;
	//if (false == Message->addChild(&LogLevel)) return false;
	if (false == Message->addChild(&Context)) return false;

	// assign values
	Timestamp->setText(MsgTimestamp);
	Message->setText(_Message);
	//LogLevel->setText(MsgLogLevel);
	NULL != _Context ? Context->setText(*_Context) : 0;

	LogAccess.leaveSerialize();

	return true;
}
bool CmParallelFTL::writeLog(CmString _LogFile, CmString _UURI, CmString _FilePath)
{
	// check Logger availability
	CmStringFTL *Location = NULL;
	CmStringFTL *UURI = NULL;
	NULL != Logger ? Location = Logger->getChild() : 0;
	NULL != Location ? UURI = Location->getChild() : 0;
	if (NULL == UURI) return false;

	// check file path
	if (false == PathFileExistsA(_FilePath.getText())) return false;

	// assign a UURI
	UURI->setText(_UURI);

	// convert Logger to a string
	CmStringFTL LogFTL;
	Logger->serializeStringFTL(LogFTL);
	// decode excaped characters
	LogFTL.decodeTextX();

	// release logger
	CmString::releaseMemory<CmStringFTL>(Logger, 1, isCmStringFTL);
	Logger = NULL;

	// add a timestamp prefix to the logfile name
	// ToDo: insert the logfile into a FTLight repository
	CmString Timestamp = CmDateTime::getTimeUTC(0, -1, true); // true= timestamp for filename
	Timestamp += "_";
	Timestamp += _LogFile;
	_LogFile = _FilePath;
	_LogFile[((int32)_LogFile.getLength()) - 1] != '\\' ? _LogFile += "\\" : 0;
	_LogFile += Timestamp;

	// write logfile to disk
	return LogFTL.writeBinary(_LogFile.getText());
}

//----------------------------------------------------------------------------
// CmStringFTL class.
//----------------------------------------------------------------------------
CmStringFTL::CmStringFTL(CmStringFTL* _Parent)
{
	// initialize workspace
	Parent = _Parent;
	Sibling = this;
	Child = NULL;
	TextX = NULL;
	Navigation = NULL;
	LineNumber = 0;
	LengthX = 0;
	DataFormat = DATAFORMAT_NONE;

	// without a parent this will be a top operator node
	if (NULL == Parent)
		TypeX = TYPEFTL_OPERATOR;
	else
		TypeX = TYPEFTL_TEXT;

	// counter for BinMCLconvert bookkeeping
	CountStringFTL++;
}

CmStringFTL::CmStringFTL(const int8* _Text, CmStringFTL* _Parent)
	:CmString(_Text)
{
	// initialize workspace
	Parent = _Parent;
	Sibling = this;
	Child = NULL;
	TextX = NULL;
	Navigation = NULL;
	LengthX = 0;
	DataFormat = DATAFORMAT_NONE;

	// convert input string to a FTLight compliant string
	encodeTextX(_Text);

	// counter for BinMCLconvert bookkeeping
	CountStringFTL++;
}

CmStringFTL::~CmStringFTL()
{
	clearFTLightHierarchy();

	// check if last CmStringFTL
	CountStringFTL--;
	if (0 == CountStringFTL){ 

		clearConversionTable();
	}
}

// unit test for CmStringFTL
bool CmStringFTL::testCmStringFTL()
{
	// define test parameters
#define BIN_LEN		1421507
#define BIN_LEN_MCL (16 * BIN_LEN / 15)
#define STR_LEN		30
	CmString Memory;
	uint8* pField = Memory.allocateMemory<uint8>(BIN_LEN + 1, isUint8);
	uint8* pField1 = Memory.allocateMemory<uint8>(BIN_LEN + 1, isUint8);
	uint16* pBinMCL = Memory.allocateMemory<uint16>(BIN_LEN_MCL/2, isUint16);
	CmString mBinX;
	CmString mBinX1(STR_LEN);
	CmString mBinX2(STR_LEN);
	CmVector<int64> ValDIF;
	CmVector<int64> ValDIF1;
	CmVector<int64> ValDIF16;
	CmVector<uint8> BinDIF;

	// generate test input 
	for (int i = 0; i<BIN_LEN; i++){
		pField[i] = (uint8)(i * 43);
		ValDIF16[i] = int64(32767 * sin(double(i / (BIN_LEN / (2 * PI)))));
		ValDIF[i] = int64(1234567890123456789 * sin(double(i / (BIN_LEN / (2 * PI)))));
	}

	//------test-BinX-------------------------

	// Convert binary to a BinX string
	bin2BinX(&mBinX, pField, BIN_LEN);

	// Convert a BinX string to binary
	BinX2bin(pField1, BIN_LEN, mBinX);

	// Compare
	if (0 != memcmp(pField, pField1, BIN_LEN)){
		// show first and last positions of the BinX field
		MEMCPY(mBinX1.getBuffer(), mBinX1.getLength(), mBinX.getBuffer(), STR_LEN);
		MEMCPY(mBinX2.getBuffer(), mBinX2.getLength(), mBinX.getBuffer() + mBinX.getLength() - STR_LEN, STR_LEN);
		printf("\n BinX (%3d): \"%s.....%s\"", (int32)mBinX.getLength(), mBinX1.getBuffer(), mBinX2.getBuffer());
		printf("\n BinX conversion ---FAILED---\n");
		return false;
	}

	//------test-BinMCL-----------------------

	// Convert binary to a BinMCL string
	bin2BinMCL(BIN_LEN/30,(uint16*)pField,pBinMCL);
	
	// Convert a BinMCL string to binary
	BinMCL2bin(BIN_LEN/30,pBinMCL,(uint16*)pField1);

	// Compare
	if (0 != memcmp(pField, pField1, BIN_LEN - BIN_LEN % 2)){
		// show first and last positions of the BinMCL field
		MEMCPY(mBinX1.getBuffer(), mBinX1.getLength(), (char*)pBinMCL, STR_LEN);
		MEMCPY(mBinX2.getBuffer(), mBinX2.getLength(), (char*)pBinMCL + BIN_LEN_MCL - STR_LEN, STR_LEN);
		printf("\n BinMCL (%3d): \"%s.....%s\"", (int32)mBinX.getLength(), mBinX1.getBuffer(), mBinX2.getBuffer());
		printf("\n BinMCL conversion ---FAILED---");
		return false;
	}

	//------test-BinX2num/num2BinX--------------------

	// Convert numeric value
	uint64 u64Num = 85;
	uint64 u64Num1;
	uint64 u64Factor = 1;
	srand((unsigned)time(NULL));
	for (int i = 0; i<9; i++){
		if (i<8){
			u64Num = u64Factor + (rand()*rand()) % (u64Factor*RADIX_216);
		}
		else{
			u64Num += 0xC000000000000000;
		}
		num2BinX(&mBinX, &u64Num);
		u64Num1 = BinX2num(mBinX);
		if (i == 7){
			// Extended search for failure
			for (int k = 0; k<100000 && u64Num == u64Num1; k++){
				u64Num = u64Factor + (rand()*rand()) % (u64Factor*RADIX_216);
				num2BinX(&mBinX, &u64Num);
				u64Num1 = BinX2num(mBinX);
			}
		}
		u64Num != u64Num1 ? printf("Numeric conversion (%20I64u): %9s len=%u (%20I64u) %s\n", u64Num, mBinX.getBuffer(), (int32)mBinX.getLength(), u64Num1, "failed") : 0;
		u64Factor *= RADIX_216;
	}

	//------test-BinDIF-----------------------

	// Convert values to BinDIF format
	val2BinDIF(BinDIF, ValDIF);

	// Convert from BinDIF format to values
	BinDIF2val(BinDIF, ValDIF1);

	if (ValDIF != ValDIF1){
		printf("\n BinDIF conversion ---FAILED---");
		return false;
	}

	//-------conversion-benchmarks-------------------------

	MBenchmark   mBenchmark(1);
	// benchmark BinX
	try{
		while (mBenchmark.run()){
			bin2BinX(&mBinX, pField, BIN_LEN);
		}
		printf("\n bin2BinX   / Byte: %s", mBenchmark.info(850, BIN_LEN).getText());
	}
	catch (CmException E){
		return false;
	}
	// benchmark BinMCL
	try{
		while (mBenchmark.run()){
			bin2BinMCL(BIN_LEN / 30, (uint16*)pField, pBinMCL);
		}
		printf("\n bin2BinMCL / Byte: %s", mBenchmark.info(850, BIN_LEN).getText());
	}
	catch (CmException E){
		return false;
	}
	// benchmark BinDIF(16)
	try{
		while (mBenchmark.run()){
			val2BinDIF(BinDIF, ValDIF16);
		}
		printf("\n bin2BinDIF(16) / Byte: %s", mBenchmark.info(850, BIN_LEN*sizeof(int16)).getText());
	}
	catch (CmException E){
		return false;
	}
	// benchmark BinDIF(64)
	try{
		while (mBenchmark.run()){
			val2BinDIF(BinDIF, ValDIF);
		}
		printf("\n bin2BinDIF(64) / Byte: %s", mBenchmark.info(850, BIN_LEN*sizeof(int64)).getText());
	}
	catch (CmException E){
		return false;
	}

	// cleanup resources
	Memory.releaseMemory<uint8>(pField, BIN_LEN + 1, isUint8);
	Memory.releaseMemory<uint8>(pField1, BIN_LEN + 1, isUint8);
	Memory.releaseMemory<uint16>(pBinMCL, BIN_LEN_MCL / 2, isUint16);

	//------test-TextX------------------------

	// test strings containing all special characters
	//const int8* SpecialCharacters = "Test\n\r,-:;=@`\x7F\\end";
	//const int8* SpecialCharactersEscaped = "Test\\\n\\\r\\,\\-\\:\\;\\=\\@\\`\\\x7F\\end";
	// ToDo: escape '-' only in case of an address
	const int8* SpecialCharacters = "Test\n\r,:;=@`\x7F\\end";
	const int8* SpecialCharactersEscaped = "Test\\\n\\\r\\,\\:\\;\\=\\@\\`\\\x7F\\end";
	// binary field containing all byte values 255..0
	uint8 BinField[256];
	for (int i = sizeof(BinField) - 1; i >= 0; i--){
		BinField[i] = (uint8)(255 - i);
	}

	// set/getTextX
	setTextX(SpecialCharacters);
	if (0 != strncmp((char*)TextX, SpecialCharacters, strlen(SpecialCharacters))) return false;

	// test encodeTestX()
	encodeTextX(SpecialCharacters);
	if (0 != strncmp((char*)TextX, SpecialCharactersEscaped, strlen(SpecialCharactersEscaped))) return false;

	// test decodeTestX() 
	decodeTextX();
	if (*this != SpecialCharacters) return false;
	decodeTextX(SpecialCharactersEscaped, strlen(SpecialCharactersEscaped));

	if (*this != SpecialCharacters) return false;

	// in-place encode/decode
	CmStringFTL Special(SpecialCharacters);
	CmString Escaped = (int8*)Special.encodeTextX().getTextX();
	if (Escaped != SpecialCharactersEscaped) return false;

	// test encode/decodeBinX()
	encodeBinX(BinField, sizeof(BinField));
	decodeBinX(TextX, LengthX);
	for (int i = 0; i < sizeof(BinField); i++){
		if (BinField[i] != (uint8)*(pText + i)) return false;
	}

	// test encode/decodeNumX()
	uint64 TestVal64 = 0xFEDCBA9876543210;
	uint64 uint64Val, uint64Ret;
	int64 int64Val, int64Ret;
	double doubleVal, doubleRet;
	uint32 uint32Val, uint32Ret;
	int32 int32Val, int32Ret;
	float floatVal, floatRet;
	uint16 uint16Val, uint16Ret;
	int16 int16Val, int16Ret;
	uint8 uint8Val, uint8Ret;
	int8 int8Val, int8Ret;
	bool boolVal, boolRet;
	// uint64
	encodeNumX(TestVal64);
	setData(TextX, LengthX);
	uint64Ret = decodeNumX2uint64(&uint64Val);
	if ((uint64Ret != TestVal64) || (uint64Val != TestVal64)) return false;
	// int64
	encodeNumX((int64)TestVal64);
	setData(TextX, LengthX);
	int64Ret = decodeNumX2int64(&int64Val);
	if ((int64Ret != (int64)TestVal64) || (int64Val != (int64)TestVal64)) return false;
	// double
	encodeNumX(*((double*)&TestVal64));
	setData(TextX, LengthX);
	doubleRet = decodeNumX2double(&doubleVal);
	if ((doubleRet != *((double*)&TestVal64)) || (doubleVal != *((double*)&TestVal64))) return false;
	// uint32
	encodeNumX((uint32)TestVal64);
	setData(TextX, LengthX);
	uint32Ret = decodeNumX2uint32(&uint32Val);
	if ((uint32Ret != (uint32)TestVal64) || (uint32Val != (uint32)TestVal64)) return false;
	// int32
	encodeNumX((int32)TestVal64);
	setData(TextX, LengthX);
	int32Ret = decodeNumX2int32(&int32Val);
	if ((int32Ret != (int32)TestVal64) || (int32Val != (int32)TestVal64)) return false;
	// float
	encodeNumX(*((float*)&TestVal64));
	setData(TextX, LengthX);
	floatRet = decodeNumX2float(&floatVal);
	if ((floatRet != *((float*)&TestVal64)) || (floatVal != *((float*)&TestVal64))) return false;
	// uint16
	encodeNumX((uint16)TestVal64);
	setData(TextX, LengthX);
	uint16Ret = decodeNumX2uint16(&uint16Val);
	if ((uint16Ret != (uint16)TestVal64) || (uint16Val != (uint16)TestVal64)) return false;
	// int16
	encodeNumX((int16)TestVal64);
	setData(TextX, LengthX);
	int16Ret = decodeNumX2int16(&int16Val);
	if ((int16Ret != (int16)TestVal64) || (int16Val != (int16)TestVal64)) return false;
	// uint8
	encodeNumX((uint8)TestVal64);
	setData(TextX, LengthX);
	uint8Ret = decodeNumX2uint8(&uint8Val);
	if ((uint8Ret != (uint8)TestVal64) || (uint8Val != (uint8)TestVal64)) return false;
	// int8
	encodeNumX((int8)TestVal64);
	setData(TextX, LengthX);
	int8Ret = decodeNumX2int8(&int8Val);
	if ((int8Ret != (int8)TestVal64) || (int8Val != (int8)TestVal64)) return false;
	// bool
	encodeNumX(TestVal64 > 0);
	setData(TextX, LengthX);
	boolRet = decodeNumX2bool(&boolVal);
	if ((boolRet != (TestVal64 > 0)) || (boolVal != (TestVal64 > 0))) return false;

	//
	// test FTLight structure functionality
	//
	setText("EKD@JN58nc_Türkenfeld.CmStringFTL");		// identification
	// add some childs
	CmStringFTL *Child = NULL;
	CmStringFTL *Child1 = NULL;
	const int NumChilds = 8;
	const int NumChilds3 = 3;
	const int NumChilds32 = 1;
	const int NumChilds320 = 5;
	const int NumChilds6 = 2;
	for (int i = 0; i < NumChilds; i++){
		if (true == addChild(&Child)){
			CmString Content = "Child-";
			Content += i;
			Child->setText(Content);
		}
	}
	// find a  child
	CmString Content3("Child-3");
	CmString Content32("Child-3-2");
	if (true == findChild(&Child1, Content3)){
		// add some childs to selected child
		for (int i = 0; i < NumChilds3; i++){
			if (true == Child1->addChild(&Child)){
				CmString Content = "Child-3-";
				Content += i;
				Child->setText(Content);
			}
		}
		// find a child
		if (true == Child1->findChild(&Child1, Content32)){
			// add some childs to selected child
			for (int i = 0; i < NumChilds32; i++){
				if (true == Child1->addChild(&Child)){
					CmString Content = "Child-3-2-";
					Content += i;
					Child->setText(Content);
				}
			}
			// add some childs to last child
			Child1 = Child;
			if (NULL != Child1){
				for (int i = 0; i < NumChilds320; i++){
					if (true == Child1->addChild(&Child)){
						CmString Content = "Child-3-2-0-";
						Content += i;
						Child->setText(Content);
					}
				}
			}
		}
	}
	// find a  child
	CmString Content6("Child-6");
	if (true == findChild(&Child1, Content6)){
		// add some childs to selected child
		for (int i = 0; i < NumChilds6; i++){
			if (true == Child1->addChild(&Child)){
				CmString Content = "Child-6-";
				Content += i;
				Child->setText(Content);
			}
		}
	}

	// test serializing a FTL hierarchy
#define TEST_SERIALIZE_STRINGFTL_HIERARCHY "e.kantz@JN58nc_Türkenfeld.Test_serialize_CmStringFTL:1522000000:v1.0\n\
,Group1:::comment1\n\
,,Item1:Value1\n\
,,Item2:Value2\n\
,Group2:::comment2\n\
,,Item3:Value3\n\
,,Item4:Value4\n\
"
	
	CmString SourceFTL(TEST_SERIALIZE_STRINGFTL_HIERARCHY);
	CmString StringFTL;
	CmStringFTL ProFTL;
	// NOTE: process/serializeStringFTL has to run from own CmStringFTL (not from *this)
	ProFTL.processStringFTL(SourceFTL);
	ProFTL.serializeStringFTL(StringFTL);
	// update timestamp
	int32 Start = SourceFTL.findPosition(':');
	int32 Length = SourceFTL.findPosition(':', Start+1) - Start + 1;
	memcpy_s(SourceFTL.getBuffer() + Start, Length, StringFTL.getBuffer() + Start, Length);
	// compare
	//int8* Input = SourceFTL.getBuffer();
	//int8* Output = StringFTL.getBuffer();
	if (StringFTL != SourceFTL) return false;


	// test synchronous reading
#define TEST_SYNCHRONOUS_WRITING "e.kantz@JN58nc_Türkenfeld.Test_synchronous_reading:1524000000:v1.0\n\
,Data:Timestamp,Item1,Item2\n\
ñ÷÷÷`!`¨$;ò÷÷÷`*;ò÷÷÷`$;@\n\
8·b¡.Hn\\; ý¼©\\; Ð„zŒ\n\
heÊ°.Hnf; ý¼©–; Ù&YŒ\n\
àpoÀ.Hnf; ý¼©–; m{ #Œ\n\
"
	CmString DataIn = TEST_SYNCHRONOUS_WRITING;
	CmString DataOut;
	CmString DataOut2;
	CmStringFTL DataFTL;
	// convert data forth and back
	DataFTL.processStringFTL(DataIn);
	DataFTL.serializeStringFTL(DataOut);
	// compare
	//int8* Input = DataIn.getBuffer();
	//int8* Output = DataOut.getBuffer();
	// convert data forth and back
	DataFTL.clearFTLightHierarchy();
	DataFTL.processStringFTL(DataOut);
	DataFTL.serializeStringFTL(DataOut2);
	// TEST
	//DataOut.writeBinary("DataOut.txt");
	//DataOut2.writeBinary("DataOut2.txt");
	if (DataOut != DataOut2) return false;

	// remove conversion tables
	clearConversionTable();

	return true;
}

bool CmStringFTL::isConversionTable()
{
	// check if conversion tables have been created before
	if (NULL == BinMCLconvert){
		
		CmString Memory;
		// get memory for BINX/SYMBOL
		if (NULL == (BINX = Memory.allocateMemory<uint8>(BinXconvertSize, isUint8))) return false;
		if (NULL == (SYMBOL = Memory.allocateMemory<uint8>(BinXconvertSize, isUint8))) return false;
		// fill conversion tables
		for (int32 i = 0; i < BinXconvertSize; i++){
			*(BINX + i) = Symbol2BinX(uint8(i));
			*(SYMBOL + i) = BinX2Symbol(uint8(i));
		}
		// get memory for BinMCL
		if (NULL == (BinMCLconvert = Memory.allocateMemory<uint16>(BinMCLconvertSize, isUint16))) return false;
		// fill conversion table
		for (int32 i = 0; i < BinMCLconvertSize; i++){
			*(BinMCLconvert + i) = 256 * Symbol2BinX((uint8)(i / 216)) + Symbol2BinX((uint8)(i % 216));
		}
	}
	return true;
}
bool CmStringFTL::clearConversionTable()
{
	// remove BinX conversion tables
	releaseMemory<uint8>(BINX, BinXconvertSize, isUint8);
	releaseMemory<uint8>(SYMBOL, BinXconvertSize, isUint8);
	// remove BinMCL conversion table
	releaseMemory<uint16>(BinMCLconvert, BinMCLconvertSize, isUint16);

	return true;
}

bool CmStringFTL::clearFTLightHierarchy()
{
	// delete all childs (the list of siblings)
	CmStringFTL* Sibling = Child;
	CmStringFTL* Delete = Sibling;
	for (;NULL != Child; Delete = Sibling){
		// next sibling
		Sibling = Sibling->Sibling;
		// delete child
		releaseMemory<CmStringFTL>(Delete, 1, isCmStringFTLChild);
		if (Sibling == Child) break;
	}

	// delete navigation
	releaseMemory<NaviX>(Navigation, 1, isNaviX);

	// delete TextX
	releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);

	// re-initialize workspace
	Child = NULL;
	LineNumber = 0;
	LengthX = 0;
	DataFormat = DATAFORMAT_NONE;

	return true;
}


CmStringFTL::NaviX * CmStringFTL::X()
{
	// create navigation if it does not exist yet
	if (NULL == Navigation){
		Navigation = allocateMemory<NaviX>(1, isNaviX);
		memset(Navigation, 0, sizeof(NaviX));
	}

	return Navigation;
}


bool CmStringFTL::setTextX(const int8* _Text)
{
	DataFormat = DATAFORMAT_NONE;

	// delete previous TextX
	if (NULL != TextX) {
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}

	// check for new content
	if (NULL == _Text) return true;

	// allocate TextX memory
	LengthX = strlen(_Text);
	TextX = allocateMemory<uint8>((int32)LengthX + 1, isUint8);
	if (NULL == TextX) return true;
	// save text in TextX
	strncpy_s((int8*)TextX, LengthX+1, _Text, strlen(_Text));

	return true;
}

bool CmStringFTL::setTextX(CmString& _Text)
{
	return setTextX(_Text.getBuffer());
}

//
// TextX access functions
//
const uint8 * CmStringFTL::getTextX()
{
	return TextX;
}

int64 CmStringFTL::getLengthX()
{
	return LengthX;
}

//
// convert to and from TextX format
//
CmStringFTL& CmStringFTL::encodeTextX(const int8* _Text)
{
	// store new text or take existing text
	_Text != NULL ? CmString::setText(_Text) : 0;
	DataFormat = DATAFORMAT_NONE;

	// delete previous TextX
	if (NULL != TextX){
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}

	// check for valid content
	if (NULL == pText) return *this;

	// look for special characters and determine target string length
	const int8* pSrc = pText;
	while (0 != *pSrc){
		// special FTLight characters
		switch (*pSrc){
		case 10: LengthX++; break;	// line feed
		case 13: LengthX++; break;	// carriage return
		case 44: LengthX++; break;	// comma
		//case 45: LengthX++; break;	// minus
		case 58: LengthX++; break;	// colon
		case 59: LengthX++; break;	// semicolon
		case 61: LengthX++; break;	// equal sign
		case 64: LengthX++; break;	// "at" sign
		case 96: LengthX++; break;	// backtick
		case 127: LengthX++; break;	// delete
		default: break;
		}
		LengthX++;
		pSrc++;
	}

	// allocate TextX memory
	TextX = allocateMemory<uint8>((int32)LengthX + 1, isUint8);
	if (NULL == TextX) return *this;

	// generate TextX compliant to FTLight specification
	pSrc = pText;
	uint8* pDst = TextX;
	while (0 != *pSrc){
		// escape special FTLight characters by a preceding backslash
		switch (*pSrc){
		case 10: *pDst++ = '\\'; break;	// line feed
		case 13: *pDst++ = '\\'; break;	// carriage return
		case 44: *pDst++ = '\\'; break;	// comma
		//case 45: *pDst++ = '\\'; break;	// minus
		case 58: *pDst++ = '\\'; break;	// colon
		case 59: *pDst++ = '\\'; break;	// semicolon
		case 61: *pDst++ = '\\'; break;	// equal sign
		case 64: *pDst++ = '\\'; break;	// "at" sign
		case 96: *pDst++ = '\\'; break;	// backtick
		case 127: *pDst++ = '\\'; break;	// delete
		default: break;
		}
		*pDst++ = *pSrc++;
	}
	// add a string terminating zero
	*pDst = 0;

	return *this;
}

CmStringFTL& CmStringFTL::encodeTextX(CmString& _Text)
{
	encodeTextX(_Text.getText());
	return *this;
}

void CmStringFTL::decodeTextX(const int8* _TextX, size_t _LengthX)
{
	// delete previous TextX
	if (NULL != TextX){
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}

	// transfer intern Data to TextX if no _TextX was provided
	if (NULL == _TextX || _LengthX == 0){
		_TextX = pText;
		_LengthX = Length;
	}

	// store new FTLight coded string
	TextX = allocateMemory<uint8>((int32)_LengthX + 1, isUint8);
	if (NULL == TextX) return;
	// string terminating zero
	*(TextX + _LengthX) = 0;
	LengthX = _LengthX;
	memcpy(TextX, _TextX, LengthX);

	// count escaped characters and determine resulting decoded length
	size_t Length_l;
	Length_l = LengthX;
	const uint8* pSrc = TextX + 1;  // first character can not be escaped
	bool isSpecial;
	while (1 < _LengthX--){
		// special FTLight characters
		isSpecial = false;
		switch (*pSrc){
		case 10: isSpecial = true; break;	// line feed
		case 13: isSpecial = true; break;	// carriage return
		case 44: isSpecial = true; break;	// comma
		//case 45: isSpecial = true; break;	// minus
		case 58: isSpecial = true; break;	// colon
		case 59: isSpecial = true; break;	// semicolon
		case 61: isSpecial = true; break;	// equal sign
		case 64: isSpecial = true; break;	// "at" sign
		case 96: isSpecial = true; break;	// backtick
		case 127: isSpecial = true; break;	// delete
		default: break;
		}
		// check if a special character was escaped 
		if (isSpecial){
			if (*(pSrc - 1) == '\\')
				Length_l--;
		}
		pSrc++;
	}
	
	// set resultig text length
	setLength(Length_l);

	// decode FTLight coded string by removing backslash from 
	// all escaped special characters
	int8 TestCh;
	pSrc = TextX;
	int8* pDst = pText;
	*pDst++ = *pSrc++;		// first character can not be escaped
	_LengthX = LengthX;
	while (1 < _LengthX--){
		// special FTLight characters
		isSpecial = false;
		TestCh = *pSrc;
		switch (*pSrc){
		case 10: isSpecial = true; break;	// line feed
		case 13: isSpecial = true; break;	// carriage return
		case 44: isSpecial = true; break;	// comma
		//case 45: isSpecial = true; break;	// minus
		case 58: isSpecial = true; break;	// colon
		case 59: isSpecial = true; break;	// semicolon
		case 61: isSpecial = true; break;	// equal sign
		case 64: isSpecial = true; break;	// "at" sign
		case 96: isSpecial = true; break;	// backtick
		case 127: isSpecial = true; break;	// delete
		default: break;
		}
		// check if a special character was escaped 
		if (isSpecial){
			if (*(pSrc - 1) == '\\'){
				*--pDst = *pSrc++;
				pDst++;
			}
			else{
				*pDst++ = *pSrc++;
			}
		}
		else{
			*pDst++ = *pSrc++;
		}
	}
	DataFormat = DATAFORMAT_STRING;
}

void CmStringFTL::decodeTextX(CmString& _Text)
{
	decodeTextX(_Text.getText(), _Text.getLength());
}

//
// convert to and from BinX format
//
void CmStringFTL::encodeBinX(const uint8* _Data, size_t _Length)
{
	// delete previous TextX
	if (NULL != TextX){
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}

	// use TextX as intermediate memory for input data
	if (NULL != _Data && 0 != _Length){
		TextX = (uint8*)_Data;
		LengthX = _Length;
	}
	else{
		TextX = (uint8*)pText;
		LengthX = Length;
		pText = NULL;
		Length = 0;
	}

	// convert to BinX
	bin2BinX(this, TextX, (int32)LengthX);

	// put BinX to TextX 
	uint8* TextX1 = TextX;
	TextX = (uint8*)pText;
	pText = NULL;
	size_t LengthX1 = LengthX;
	LengthX = Length;
	Length = 0;

	// save the binary field to pText (CmString base class)
	if (NULL != _Data && 0 != _Length){
		setData(_Data, (int32)_Length);
	}
	else{
		// return binary field from intermediate place to pText (CmString base class)
		pText = (int8*)TextX1;
		Length = LengthX1;
	}

	DataFormat = DATAFORMAT_BINARY;
}

void CmStringFTL::encodeBinX(CmString& _Data)
{
	encodeBinX(_Data.getBinary(), _Data.getLength());
}

void CmStringFTL::decodeBinX(const uint8* _Data, size_t _Length)
{
	// free TextX
	if (NULL != TextX && _Data != TextX){
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}

	// Store BinX data in CmString
	if (NULL != _Data && 0 != _Length){
		setData(_Data, (int32)_Length);
	}

	// release TextX
	releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
	// Determine resulting binary length
	LengthX = getLengthBinX2bin(int32(Length));
	TextX = allocateMemory<uint8>(int32(LengthX + 1), isUint8);
	memset(TextX, 0, LengthX+1);

	// Run conversion from BinX to a binary field
	BinX2bin(TextX, (int32)LengthX, *this);

	// put BinX to TextX 
	uint8* TextX1 = TextX;
	TextX = (uint8*)pText;
	pText = NULL;
	size_t LengthX1 = LengthX;
	LengthX = Length;
	Length = 0;

	// return binary field from intermediate place to pText (CmString base class)
	pText = (int8*)TextX1;
	Length = LengthX1;
	DataFormat = DATAFORMAT_BINARY;
}

void CmStringFTL::decodeBinX(CmString& _Data)
{
	decodeBinX(_Data.getBinary(), _Data.getLength());
}

//
// convert to and from NumX format
//
void CmStringFTL::encodeNumX(uint64 _Value)
{
	// convert 64-bit field to a BinX string
	num2BinX(this, _Value);

	// put BinX string to TextX
	if (NULL!=TextX)
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
	LengthX = Length;
	TextX = allocateMemory<uint8>((int32)LengthX + 1, isUint8);
	*(TextX + LengthX) = 0;
	memcpy(TextX, pText, Length);

	// put 64-bit value to CmString
	setData((uint8*)&_Value, 8);
	DataFormat = DATAFORMAT_UINT64;
}
void CmStringFTL::encodeNumX(int64 _Value)
{
	uint64 Value = 0;
	Value = (uint64)_Value;
	encodeNumX(Value);
	DataFormat = DATAFORMAT_INT64;
}
void CmStringFTL::encodeNumX(double _Value)
{
	uint64 Value = 0;
	Value = *((uint64*)(&_Value));
	encodeNumX(Value);
	DataFormat = DATAFORMAT_DOUBLE;
}
void CmStringFTL::encodeNumX(uint32 _Value)
{
	uint64 Value = 0;
	Value = (uint64)_Value;
	encodeNumX(Value);
	DataFormat = DATAFORMAT_UINT32;
}
void CmStringFTL::encodeNumX(int32 _Value)
{
	uint64 Value = 0;
	Value = (uint64)_Value;
	encodeNumX(Value);
	DataFormat = DATAFORMAT_INT32;
}
void CmStringFTL::encodeNumX(float _Value)
{
	uint64 Value = 0;
	Value = *((uint32*)(&_Value));
	encodeNumX(Value);
	DataFormat = DATAFORMAT_FLOAT;
}
void CmStringFTL::encodeNumX(uint16 _Value)
{
	uint64 Value = 0;
	Value = (uint64)_Value;
	encodeNumX(Value);
	DataFormat = DATAFORMAT_UINT16;
}
void CmStringFTL::encodeNumX(int16 _Value)
{
	uint64 Value = 0;
	Value = (uint64)_Value;
	encodeNumX(Value);
	DataFormat = DATAFORMAT_INT16;
}
void CmStringFTL::encodeNumX(uint8 _Value)
{
	uint64 Value = 0;
	Value = (uint64)_Value;
	encodeNumX(Value);
	DataFormat = DATAFORMAT_UINT8;
}
void CmStringFTL::encodeNumX(int8 _Value)
{
	uint64 Value = 0;
	Value = (uint64)_Value;
	encodeNumX(Value);
	DataFormat = DATAFORMAT_INT8;
}
void CmStringFTL::encodeNumX(bool _Value)
{
	uint64 Value = 0;
	Value = (uint64)(_Value ? 1 : 0);
	encodeNumX(Value);
	DataFormat = DATAFORMAT_BOOL;
}
//
// decode
//
uint64 CmStringFTL::decodeNumX2uint64(uint64* _Value)
{
	// convert BinX to uint64
	uint64 Value = BinX2num(*this);

	// put BinX to TextX
	if (NULL!=TextX)
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
	LengthX = Length;
	TextX = allocateMemory<uint8>((int32)LengthX + 1, isUint8);
	*(TextX + LengthX) = 0;
	memcpy(TextX, pText, Length);

	// put 64-bit value to CmString
	setData((uint8*)&Value, 8);

	// return result
	if (NULL != _Value)
		*_Value = Value;
	DataFormat = DATAFORMAT_UINT64;

	return Value;
}
int64 CmStringFTL::decodeNumX2int64(int64* _Value)
{
	// convert BinX to uint64
	decodeNumX2uint64();

	// return result
	if (NULL != _Value)
		*_Value = (int64)*((uint64*)pText);
	DataFormat = DATAFORMAT_INT64;

	return (int64)*((uint64*)pText);
}
double CmStringFTL::decodeNumX2double(double* _Value)
{
	// convert BinX to uint64
	decodeNumX2uint64();

	// return result
	if (NULL != _Value)
		*_Value = *((double*)pText);
	DataFormat = DATAFORMAT_DOUBLE;

	return *((double*)pText);
}
uint32 CmStringFTL::decodeNumX2uint32(uint32* _Value)
{
	// convert BinX to uint64
	decodeNumX2uint64();

	// return result
	if (NULL != _Value)
		*_Value = (uint32)*((uint64*)pText);
	DataFormat = DATAFORMAT_UINT32;

	return (uint32)*((uint64*)pText);
}
int32 CmStringFTL::decodeNumX2int32(int32* _Value)
{
	// convert BinX to uint64
	decodeNumX2uint64();

	// return result
	if (NULL != _Value)
		*_Value = (int32)*((uint64*)pText);
	DataFormat = DATAFORMAT_INT32;

	return (int32)*((uint64*)pText);
}
float CmStringFTL::decodeNumX2float(float* _Value)
{
	// convert BinX to uint64
	decodeNumX2uint64();

	// return result
	if (NULL != _Value)
		*_Value = *((float*)pText);
	DataFormat = DATAFORMAT_FLOAT;

	return *((float*)pText);
}
uint16 CmStringFTL::decodeNumX2uint16(uint16* _Value)
{
	// convert BinX to uint64
	decodeNumX2uint64();

	// return result
	if (NULL != _Value)
		*_Value = (uint16)*((uint64*)pText);
	DataFormat = DATAFORMAT_UINT16;

	return (uint16)*((uint64*)pText);
}
int16 CmStringFTL::decodeNumX2int16(int16* _Value)
{
	// convert BinX to uint64
	decodeNumX2uint64();

	// return result
	if (NULL != _Value)
		*_Value = (int16)*((uint64*)pText);
	DataFormat = DATAFORMAT_INT16;

	return (int16)*((uint64*)pText);
}
uint8 CmStringFTL::decodeNumX2uint8(uint8* _Value)
{
	// convert BinX to uint64
	decodeNumX2uint64();

	// return result
	if (NULL != _Value)
		*_Value = (uint8)*((uint64*)pText);
	DataFormat = DATAFORMAT_UINT8;

	return (uint8)*((uint64*)pText);
}
int8 CmStringFTL::decodeNumX2int8(int8* _Value)
{
	// convert BinX to uint64
	decodeNumX2uint64();

	// return result
	if (NULL != _Value)
		*_Value = (int8)*((uint64*)pText);
	DataFormat = DATAFORMAT_INT8;

	return (int8)*((uint64*)pText);
}
bool CmStringFTL::decodeNumX2bool(bool* _Value)
{
	// convert BinX to uint64
	decodeNumX2uint64();

	// return result
	if (NULL != _Value)
		*_Value = *((uint64*)pText) > 0 ? true : false;
	DataFormat = DATAFORMAT_BOOL;

	return *((uint64*)pText) > 0 ? true : false;
}

bool CmStringFTL::decodeNum(CmDataFormat _DataFormat)
{
	// check for FTLight Num format
	if (TypeX != TYPEFTL_NUMBER && TypeX != TYPEFTL_NUMBER_BINX) return false;

	// try appropriate conversion
	uint64 uVal = 0;
	double fVal = 0;
	if (DATAFORMAT_UINT64 == _DataFormat || DATAFORMAT_BOOL == _DataFormat){
		// for uint64 format, a separate conversion is used in order to get full length
		uVal = getNumAsUint64();
		if (isConversionError){
			// if UINT64 does not work then try double
			fVal = (double)getNumAsDouble();
			if (isConversionError){
				// failed, no chance
				return false;
			}
			uVal = (uint64)fVal;
		}
	}
	else{
		// all other formats will first be converted into double
		fVal = getNumAsDouble();
		// if double conversion does not work then try UINT64
		if (isConversionError){
			fVal = (double)getNumAsUint64();
			if (isConversionError){
				// failed, no chance
				return false;
			}
		}
	}
	// transfer Num string to TextX
	if (getLength() == 0) return false;
	if (NULL != TextX){
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}
	// ToDo: make memory deallocation thread-safe
	//LengthX = getLength();
	//TextX = allocateMemory<uint8>((int32)LengthX+1, isUint8);
	//*(TextX + LengthX) = 0;
	//memcpy(TextX, getBuffer(), LengthX);

	// write appropriate data format into CmString base class, 
	CmFlex Val;
	setLength(sizeof(Val));
	DataFormat = _DataFormat;
	switch (DataFormat)
	{
	case DATAFORMAT_UINT64: Val.uint64Value = uVal; break;
	case DATAFORMAT_DOUBLE: Val.doubleValue = fVal; break;
	case DATAFORMAT_INT64: Val.int64Value = (int64)fVal; break;
	case DATAFORMAT_UINT32: Val.uint32Value = (uint32)fVal; break;
	case DATAFORMAT_INT32: Val.int32Value = (int32)fVal; break;
	case DATAFORMAT_FLOAT: Val.floatValue = (float)fVal; break;
	case DATAFORMAT_UINT16: Val.uint16Value = (uint16)fVal; break;
	case DATAFORMAT_INT16: Val.int16Value = (int16)fVal; break;
	case DATAFORMAT_UINT8: Val.uint8Value = (uint8)fVal; break;
	case DATAFORMAT_INT8: Val.int8Value = (int8)fVal; break;
	case DATAFORMAT_BOOL: Val.boolValue = uVal > 0 ? true : false; break;
	default: return false;
	}
	memcpy(getBuffer(), &Val.uint8Value, sizeof(Val));

	return true;
}
bool CmStringFTL::isInteger()
{
	// evaluated string content for being an integer (+-0..9)
	bool isInteger = true;
	for (int32 i = 0; i < int32(Length); i++){
		uint8 Char = *(pText+i);
		if((Char < '0' || Char > '9') && (Char != '+') && (Char != '-')){
			isInteger = false;
			break;
		}
	}
	
	return isInteger;
}

//
// CmStringFTL node access
//
void CmStringFTL::setParent(CmStringFTL* _Parent)
{
	Parent = _Parent;
}
CmStringFTL * CmStringFTL::getParent()
{
	return Parent;
}
CmStringFTL * CmStringFTL::getSibling()
{
	return Sibling;
}
CmStringFTL * CmStringFTL::getChild()
{
	return Child;
}
CmTypeFTL CmStringFTL::getTypeX()
{
	return TypeX;
}
bool CmStringFTL::isBinary(bool _isParentBinary)
{
	bool isBinary = false;
	switch (TypeX){
	case TYPEFTL_BINX:
	case TYPEFTL_CONTROLX:
	case TYPEFTL_FTLightOPEN:
	case TYPEFTL_FTLightWRAP:
	case TYPEFTL_BINMCL:
	case TYPEFTL_BINXBINARY:
	case TYPEFTL_BINXSTRING:
	case TYPEFTL_BINXVALUE:
	case TYPEFTL_BINXTIME:
	case TYPEFTL_CMXTOKEN:
	case TYPEFTL_CMXLINK:
	case TYPEFTL_BINDIF:
		isBinary = true;
		break;
	case TYPEFTL_TEXT_BINX:
		// will be binary when parent was binary
		isBinary = _isParentBinary ? true : false;
		break;
	default: break;
	}

	return isBinary;
}
//
// FTLight structure maintenance
//
bool CmStringFTL::addChild(CmStringFTL** _Child)
{
	// create a new child
	CmStringFTL* NewChild = allocateMemory<CmStringFTL>(1, isCmStringFTLChild);
	if (NULL == NewChild) return false;

	// check if a child existed already
	if (NULL == Child){
		Child = NewChild;
	}else{
		// insert new child into chain of siblings
		CmStringFTL* LastChild;
		if (false == Child->getLastChild(&LastChild)){
			releaseMemory<CmStringFTL>(NewChild, 1, isCmStringFTLChild);
			return false;
		}
		LastChild->Sibling = NewChild;
		NewChild->Sibling = Child;
	}

	// establish a child-parent relationship
	NewChild->Parent = this;

	// return new child
	if (NULL == _Child) return false;
	*_Child = NewChild;

	return true;
}

bool CmStringFTL::findChild(CmStringFTL** _Child, CmString& _Content, CmTypeFTL _TypeX)
{
	// initialize return value
	if (NULL != _Child)
		*_Child = NULL;

	// check parameter and if any child exists
	if ((NULL == _Child) || (NULL == Child)) return false;

	// step through list of siblings
	*_Child = Child;
	do{
		// compare content
		if ((*_Child)->TypeX == _TypeX && (*_Child)->getLength() == _Content.getLength()){
			if (0 == memcmp((*_Child)->getText(), _Content.getText(), _Content.getLength()))
				return true;
		}
		*_Child = (*_Child)->Sibling;
	} while (*_Child != Child);

	// no child matches
	*_Child = NULL;

	return false;
}

//
// receiveLineX() and local functions
// 
bool CmStringFTL::createRoot(CmString& _Identifier)
{
	// extract operator and location from identifier
	CmString Operator;
	CmString Location;
	CmUURI UURI("", _Identifier.getText());
	Operator = UURI.getOperator();
	Location = UURI.getLocation();

	// add an operator node if this node was different
	X()->PathX = this;
	if ((TYPEFTL_OPERATOR != X()->PathX->TypeX) || (0 != X()->PathX->getLength() && *X()->PathX != Operator)){
		X()->PathX->addChild(&X()->PosX);
		if (NULL == X()->PosX) return false;
		X()->PathX = X()->PosX;
		X()->PathX->TypeX = TYPEFTL_OPERATOR;
		X()->PathX->DataFormat = DATAFORMAT_FTLight;
	}
	if (0 == X()->PathX->getLength()){
		X()->PathX->setText(Operator);
		X()->PathX->DataFormat = DATAFORMAT_FTLight;
	}

	// search for a location node
	X()->PathX->findChild(&X()->PosX, Location);
	if (NULL != X()->PosX){
		X()->PathX = X()->PosX;
	}
	else{
		// add a location node since it does not exist yet
		X()->PathX->addChild(&X()->PosX);
		if (NULL == X()->PosX) return false;
		X()->PathX = X()->PosX;
		X()->PathX->TypeX = TYPEFTL_LOCATION;
		X()->PathX->setText(Location);
		X()->PathX->DataFormat = DATAFORMAT_FTLight;
	}

	// search for an identifier node
	X()->PathX->findChild(&X()->PosX, _Identifier);
	if (NULL != X()->PosX){
		X()->PathX = X()->PosX;
	}
	else{
		// add an identifier node since it does not exist yet
		X()->PathX->addChild(&X()->PosX);
		if (NULL == X()->PosX) return false;
		X()->PathX = X()->PosX;
		X()->PathX->TypeX = TYPEFTL_IDENTIFIER;
		X()->PathX->setText(_Identifier);
		X()->PathX->DataFormat = DATAFORMAT_FTLight;
	}

	// set Root to found respectively created identifier node
	if (NULL != X()->Root && *X()->Root != _Identifier) return false;
	X()->Root = X()->PathX;
	X()->PosX = X()->Root;

	return true;
}

bool CmStringFTL::createRoot()
{
	// make this a root node above "0" in a CmStringFTL hierarchy
	Parent = NULL;
	Sibling = this;
	Child = NULL;
	X()->Root = this;
	X()->PathX = this;
	X()->PosX = this;


	return true;
}

bool CmStringFTL::getChecksum(CmString& _LineX, CmString& _Checksum)
{
	// get line and highlight information
	int32 LineNumber_l;
	uint32 LineStart_l;
	uint32 HighlightStart_l;
	uint32 HighlightLength_l;
	if (false == _LineX.getLineHighlight(LineNumber_l, LineStart_l, HighlightStart_l, HighlightLength_l)) return false;

	// determine range (dependent on checksum length)
	int64 Range;
	switch (HighlightLength_l){
	case 1: Range = CHECKSUM_DIGIT_1; break;
	case 2: Range = CHECKSUM_DIGIT_2; break;
	case 3: Range = CHECKSUM_DIGIT_3; break;
	case 4: Range = CHECKSUM_DIGIT_4; break;
	case 5: Range = CHECKSUM_DIGIT_5; break;
	case 6: Range = CHECKSUM_DIGIT_6; break;
	case 7: Range = CHECKSUM_DIGIT_7; break;
	default: Range = CHECKSUM_DIGIT_1; break;
	}

	// calculate checksum without line number
	uint64 Checksum = 0;
	for (uint32 n = 0; n < HighlightStart_l - LineStart_l; n++){
		Checksum *= 256;
		Checksum += _LineX[LineStart_l + n];
		Checksum %= Range;
	}

	// include line number
	CmString Num;
	Num += 10;
	for (uint32 n = 0; n < Num.getLength(); n++){
		Checksum *= 256;
		Checksum += Num[n];
		Checksum %= Range;
	}

	// convert checksum to BinX format
	num2BinX(&_Checksum, Checksum);

	return true;
}

bool CmStringFTL::verifyChecksum(CmString& _LineX, CmString& _Checksum)
{
	// calculate checksum
	CmString Checksum;
	if (false == getChecksum(_LineX, Checksum)) return false;

	// verify received checksum against calculated checksum
	if (Checksum != _Checksum) return false;

	return true;
}

bool CmStringFTL::getChild(int32 _ChildPosition)
{
	// goto first child
	if (NULL != X()->PosX/* && NULL != X()->PosX*/->Child){
		X()->PosX = X()->PosX->Child;
	}
	else{
		CmStringFTL *FirstChild = NULL;
		if (false == X()->PosX->addChild(&FirstChild)) return false;
		X()->PosX = FirstChild;
		*X()->PosX += "-";
	}

	// find/create appropriate sibling
	while (_ChildPosition-- > 0){
		// check if a next sibling exists (it must not be the first one)
		if (NULL != X()->PosX->Sibling && X()->PosX->Sibling != X()->PosX->Sibling->Parent->Child){
			X()->PosX = X()->PosX->Sibling;
		}
		else{
			// create a new sibling since PosX points already to the last one
			CmStringFTL *NewChild = allocateMemory<CmStringFTL>(1, isCmStringFTLChild);
			if (NULL == NewChild) return false;
			NewChild->setParent(X()->PosX->Parent);
			NewChild->Sibling = X()->PosX->Sibling;
			X()->PosX->Sibling = NewChild;
			X()->PosX = NewChild;
			*X()->PosX += "-";
		}
	}

	return true;
}

bool CmStringFTL::isChild(int32 _ChildPosition)
{
	// goto first child
	if ((NULL == X()->PosX) || (NULL == X()->PosX->Child)) return false;
	X()->PosX = X()->PosX->Child;

	// find appropriate sibling
	while (_ChildPosition-- > 0){
		// check if a next sibling exists (its not the first one)
		if (X()->PosX->Sibling == X()->PosX->Sibling->Parent->Child) return false;
		X()->PosX = X()->PosX->Sibling;
	}

	return true;
}

bool CmStringFTL::getPosition(const CmString& _Address)
{
	// adjust PosX to Root position's parent
	if (NULL != X()->Root && NULL != X()->Root->Parent){
		X()->PosX = X()->Root->Parent;
	}

	// find/create childs according to address string's components
	int32 ChildPosition = 0;
	for (uint32 n = 0; n < _Address.getLength(); n++)
	{
		if ('-' == _Address[n]){
			// find/create a child at ChildPosition
			if (false == getChild(ChildPosition)) return false;
			ChildPosition = 0;
			continue;
		}
		ChildPosition *= 10;
		ChildPosition += _Address[n] - '0';
	}

	// find/create a child according to last address component
	if (false == getChild(ChildPosition)) return false;

	return true;
}

bool CmStringFTL::findPosition(const CmString& _Address, CmString _StartAddr)
{
	// adjust PosX to Root position's parent
	if (NULL != X()->Root && NULL != X()->Root->Parent){
		X()->PosX = X()->Root->Parent;
	}

	// find childs according to address string's components
	int32 ChildPosition = 0;
	int32 PositionStartAddr = -1;
	for (uint32 n = 0; n < _Address.getLength(); n++)
	{
		if ('-' == _Address[n]){
			// replace with matching address component if it exists
			int32 StartPosition = 0;
			while (++PositionStartAddr < (int32)_StartAddr.getLength()){
				if ((_StartAddr[PositionStartAddr] < '0') || (_StartAddr[PositionStartAddr] > '9')) break;
				StartPosition *= 10;
				StartPosition += _StartAddr[PositionStartAddr] - '0';
				ChildPosition = StartPosition;
			}
			// find a child at ChildPosition
			if (false == isChild(ChildPosition)) return false;
			ChildPosition = 0;
			continue;
		}
		ChildPosition *= 10;
		ChildPosition += _Address[n] - '0';
	}

	// find/create a child according to last address component
	if (false == isChild(ChildPosition)) return false;

	return true;
}

bool CmStringFTL::getLocal(const CmString& Address, CmStringFTL*& LocalX)
{
	CmStringFTL *PosX1 = X()->PosX;

	// check if Locald item exists
	if (true == findPosition(Address)){
		// preserve Local information
		LocalX = X()->PosX;
	}
	else{
		// Locald item does not exist
		LocalX = NULL;
		TypeX = TYPEFTL_Local_BINX;
	}

	// Local: recover position
	X()->PosX = PosX1;

	return true;
}

bool CmStringFTL::initLineX(CmString& _LineX, uint32& _LineStart, uint32& _ActiveStart, uint32& _ActiveLength, uint32& _HighlightStart, uint32& _HighlightLength)
{
	// 'highlight' the 'active' range to indicate start of a new line
	_LineX.getActive(_ActiveStart, _ActiveLength, LineNumber);
	_LineStart = _ActiveStart;
	_HighlightStart = _ActiveStart;
	_HighlightLength = _ActiveLength;
	_LineX.setHighlight(_HighlightStart, _HighlightLength);

	// PosX will be adjusted step-by-step to root position
	X()->PosX = X()->Root;
	if (NULL != X()->Root && NULL != X()->Root->Parent){
		X()->PosX = X()->Root->Parent;
	}

	X()->isPath = true;
	X()->isOption = false;
	X()->isFirstItem = true;
	X()->isRepeatedPath = true;
	X()->isStartOfCollection = false;
	X()->isWaitForBufferStart = false;
	X()->isWaitForBufferLength = false;
	X()->isSingleAddressBeforeAt = false;
	X()->isRestoreSynchronousWriting = false;
	
	return true;
}

bool CmStringFTL::checkSyncronousWriting(CmTypeFTL _TypeX)
{
	// check for valid navigation
	if ((NULL == X()->PosX) || (NULL == X()->PathX)) return false;

	if (TYPEFTL_IDENTIFIER == _TypeX){
		// switch synchronous writing off
		X()->sync.isSynchronousWriting = false;
		X()->sync.isFixedParentCollection = false;
	}
	else{
		// check for start of synchronous writing 
		if (false == X()->sync.isSynchronousWriting){
			// initialize parent/level/child positions for synchronous writing
			X()->sync.isSynchronousWriting = true;
			X()->sync.FirstParentPos = -1;
			X()->sync.Levels = 0;
			X()->sync.SequenceNumber = 0;
			X()->sync.BufferLength = 0;
			X()->sync.BufferPos = -1;

			// adjust state
			X()->isRepeatedPath = false;
			X()->isPath = false;

			// check for synchronous writing on a new vs. existing position
			if ((NULL != X()->PosX->Parent) && (X()->isRestoreSynchronousWriting || (NULL == X()->PosX->Child))){
				// existing position: adjust PathX to current position's parent 
				X()->PathX = X()->PosX->Parent;
				X()->sync.FirstParentPos = -1;
				// search for addressed first item (on parent level)
				CmStringFTL *ParentPos = X()->PathX->Child;
				while (ParentPos != NULL && ParentPos != X()->PosX && ParentPos->Sibling != ParentPos->Sibling->Parent->Child){
					X()->sync.FirstParentPos++;
					ParentPos = ParentPos->Sibling;
				}
			}
			else{
				// preserve existing items (on parent level) when starting a new synchronous writing 
				X()->PosX = X()->PathX;
				if (NULL != X()->PosX && NULL != X()->PosX->Child){
					X()->sync.Levels = 1;
				}
			}
			X()->sync.ParentPos = X()->sync.FirstParentPos;
		}
		else{
			// initialize parent position
			X()->sync.ParentPos = X()->sync.FirstParentPos;
			// increment level and buffer position
			if (false == X()->sync.isFixedParentCollection){
				X()->sync.Levels++;
			}
			else{
				X()->sync.SequenceNumber++;
				X()->sync.BufferPos++;
				if (X()->sync.BufferLength > 0){
					X()->sync.BufferPos %= X()->sync.BufferLength;
				}
			}
		}
	}
	X()->isFirstItem = false;
	X()->isRestoreSynchronousWriting = false;

	return true;
}

bool CmStringFTL::runSynchronousWriting(CmString& _ItemX, CmTypeFTL _TypeX)
{
	// next parent position
	X()->sync.ParentPos++;

	// detect fixed parent collections and buffer length
	if (TYPEFTL_FIXED_PARENTS == _TypeX){
		X()->sync.isFixedParentCollection = true;
		X()->isWaitForBufferLength = true;
		X()->isWaitForBufferStart = false;
		return true;
	}
	else{
		if (X()->isWaitForBufferLength){
			X()->sync.BufferLength = _ItemX.getNumAsUint64();
			X()->isWaitForBufferLength = false;
			X()->isWaitForBufferStart = true;
			return true;
		}
		else{
			if (X()->isWaitForBufferStart){
				X()->sync.BufferPos = _ItemX.getNumAsUint64() - 1;
				X()->isWaitForBufferStart = false;
				return true;
			}
		}
	}

	// generate a relative address 
	CmString SyncAddr;
	SyncAddr += X()->sync.ParentPos;
	int64 Levels = X()->sync.Levels;
	while (Levels-- > 0){
		SyncAddr += "-0";
	}
	if (X()->sync.isFixedParentCollection){
		SyncAddr += "-";
		SyncAddr += X()->sync.BufferPos;
	}

	// adjust PosX to addressed position and write _ItemX content
	X()->PathX->X()->Root = NULL; // relative start instead of start at root position
	X()->PathX->X()->PosX = X()->PathX; // PathX is the relative start position
	if (false == X()->PathX->getPosition(SyncAddr)) return false;
	X()->PathX->X()->PosX->TypeX = _TypeX;
	X()->PathX->X()->PosX->setText(_ItemX);
	X()->PathX->X()->PosX->DataFormat = DATAFORMAT_FTLight;

	return true;
}

bool CmStringFTL::evaluateRepeatedPath()
{
	if (false == X()->isStartOfCollection){
		if (X()->isRepeatedPath){
			// start at current path
			CmStringFTL *NewPathX = X()->PathX;
			// check if path was exhausted
			if (X()->PosX == X()->PathX){
				// involve child in path
				NULL != NewPathX->Child ? NewPathX = NewPathX->Child : 0;
			}
			// involve one more of previous path items
			while (NULL != NewPathX->Parent && NewPathX->Parent != X()->PosX && NewPathX != X()->PosX){
				NewPathX = NewPathX->Parent;
			}
			X()->PosX = NewPathX;
			return true;
		}
	}
	else{
		X()->isRepeatedPath = false;
	}

	return false;
}

bool CmStringFTL::putChildOnPath(CmString& _ItemX, CmTypeFTL& _TypeX, CmStringFTL *_LocalX)
{
	// recover effective path from PosX
	X()->PathX = X()->PosX;

	// determine parent position in order to maintain childs or siblings
	if (false == X()->isPath && NULL != X()->PathX->Parent && false == X()->sync.isSynchronousWriting && false == X()->isStartOfCollection){
		X()->PathX = X()->PathX->Parent;
	}

	// evaluation for option
	if (TYPEFTL_OPTION == _TypeX){
		X()->isOption = true;
	}
	else if (X()->isStartOfCollection){
		X()->isOption = false;
	}
	if (X()->isOption){
		_TypeX = TYPEFTL_OPTION;
	}

	// add next item
	X()->PathX->addChild(&X()->PosX);
	if (NULL == X()->PosX) return false;
	X()->PosX->TypeX = _TypeX;
	X()->PosX->DataFormat = DATAFORMAT_FTLight;
	// set _ItemX content respectively a Local (address)
	if (TYPEFTL_Local == _TypeX){
		X()->PosX->setLength(sizeof(_LocalX));
		memcpy(X()->PosX->getBuffer(), &_LocalX, sizeof(_LocalX));
	}
	else{
		X()->PosX->setText(_ItemX);
	}
	X()->PosX->DataFormat = DATAFORMAT_FTLight;

	return true;
}

bool CmStringFTL::receiveLineX(CmString& _LineX)
{
	int32 LineNumber_l;
	uint32 LineStart_l;
	uint32 ActiveStart_l;
	uint32 ActiveLength_l;
	uint32 HighlightStart_l;
	uint32 HighlightLength_l;
	CmTypeFTL TypeX_l;
	CmStringFTL ItemX;
	CmStringFTL *LocalX = NULL;

	// initialize LineX reader for reading next line
	if (false == initLineX(_LineX, LineStart_l, ActiveStart_l, ActiveLength_l, HighlightStart_l, HighlightLength_l)) return false;

	// obtain all ItemX along with their types
	while (getNextItemX(_LineX, ItemX, TypeX_l, X()->isStartOfCollection, X()->isParentBinary)){

		// determine active and highlighted text fragments
		_LineX.getActive(ActiveStart_l, ActiveLength_l, LineNumber_l);
		_LineX.getHighlight(HighlightStart_l, HighlightLength_l);

		// process item dependent on TypeX_l
		switch (TypeX_l){
		case TYPEFTL_IDENTIFIER:
			X()->isSingleAddressBeforeAt = false;
			// at line 1, goto or create operator, location and identifier nodes
			if (1 == LineNumber_l && 0 == HighlightStart_l){
				if (false == createRoot(ItemX)) return false;
				X()->isRepeatedPath = false;
				X()->isFirstItem = false;
				continue;
			} 
			break;
		case TYPEFTL_Local_BINX:
			X()->isSingleAddressBeforeAt = false;
			// adjust PosX to addressed/Locald position
			if (false == getPosition(ItemX)) return false;
			X()->PathX = X()->PosX;
			X()->isFirstItem = false;
			// mark address if it is the first item in a line
			if (LineStart_l == HighlightStart_l){
				X()->isSingleAddressBeforeAt = true;
			}
			continue;
		case TYPEFTL_Local:
			X()->isSingleAddressBeforeAt = false;
			// handle Local (for further items beyond line start)
			if (false == getLocal(ItemX, LocalX)) return false;
			break;
		case TYPEFTL_CHECKSUM:
			X()->isSingleAddressBeforeAt = false;
			// verify checksum
			if (false == verifyChecksum(_LineX, ItemX)) return false;
			continue;
		case TYPEFTL_TEXT_BINX:
		case TYPEFTL_NUMBER_BINX:
			X()->isSingleAddressBeforeAt = false;
			if (X()->isFirstItem && false == X()->sync.isSynchronousWriting){
				// remove escape ('\') character at first position if NOT synchronous writing
				ItemX.decodeTextX();
				ItemX.TypeX = TypeX_l; // restore type
			}
			break;
		case TYPEFTL_TEXT:
		case TYPEFTL_NUMBER:
			X()->isSingleAddressBeforeAt = false;
			// remove escape ('\') character for type TextX
			ItemX.decodeTextX();
			ItemX.TypeX = TypeX_l; // restore type
			break;
		case TYPEFTL_FIXED_PARENTS:
			// restore synchronous writing if only a single address occured before this '@' item
			if (X()->isSingleAddressBeforeAt){
				X()->isRestoreSynchronousWriting = true;
				X()->isSingleAddressBeforeAt = false;
			}
			break;
		case TYPEFTL_BINXTIME:
			X()->isSingleAddressBeforeAt = false;
			if (X()->isFirstItem){
				X()->isParentBinary = true;
			}
			break;
		default:
			X()->isSingleAddressBeforeAt = false;
			break;
		}

		// analyse first item for synchronous writing 
		if (X()->isFirstItem || X()->isRestoreSynchronousWriting){
			if (false == checkSyncronousWriting(TypeX_l)) return false;
		}

		// run synchronous writing
		if (X()->sync.isSynchronousWriting){
			if (false == runSynchronousWriting(ItemX, TypeX_l)) return false;
			continue;
		}

		// evaluate repeated (empty) path items
		if (0 == HighlightLength_l){
			if (true == evaluateRepeatedPath()) continue;
		}

		// generate child
		putChildOnPath(ItemX, TypeX_l, LocalX);

		// check whether path turns into a collection
		if (X()->isStartOfCollection){
			X()->isPath = false;
		}

		// switch path to new child
		if (X()->isPath){
			X()->PathX = X()->PosX;
		}
	}

	return true;
}

bool CmStringFTL::processStringFTL(const int8* _StringX)
{
	CmString StringX(_StringX);

	return processStringFTL(StringX);
}

bool CmStringFTL::processStringFTL(CmString& _StringX)
{
	// split string into lines
	uint32 StartPos = 0;
	uint32 FoundPos = 0;
	X()->isParentBinary = false;
	while (StartPos < _StringX.getLength()){
		// find end of line
		FoundPos = _StringX.searchPattern("\x0A", StartPos);
		// convert line content and insert it into CmStringFTL hierarchy
		_StringX.setActive(StartPos, FoundPos - StartPos + 1, ++LineNumber);
		receiveLineX(_StringX);
		// next line
		StartPos = FoundPos + 1;
	}

	return true;
}

bool CmStringFTL::readFileX(const int8* _FileX)
{
	//read file from disk
	CmString FileX;
	try{
		FileX.readBinary(_FileX);
	}
	catch (...){
		return false;
	}

	return processStringFTL(FileX);
}

bool CmStringFTL::serializeStringFTL(CmString& _StringFTL)
{
	// ToDo: complete all variants

	// add timestamp
	CmString Timestamp;
	bool isOption = false;
	bool isComment = false;
	bool isParentBinary = false;
	const bool isPerformanceCounter = false;
	uint64 uTimestamp = CmDateTime::getSysClockNanoSec(isPerformanceCounter) / DATETIME_NANOSECONDS;
	Timestamp.double2String((double)uTimestamp);
	setValueX("0-0", Timestamp);

	// convert CmStringFTL hierarchy to a string
	CmString LineFTL;
	CmStringFTL *NodeChild = this;
	CmStringFTL *NodeSibling = this;
	CmStringFTL *NodeLocal = NULL;
	int LevelChild = 0;
	// travel StringFTL hierarchy
	while (NULL != NodeSibling){
		// first travel all childs 
		if (NULL != NodeSibling->getChild()){
			LevelChild > 1 ? LineFTL += (NodeSibling->getChild()->isBinary(isParentBinary) ? "=" : ":") : 0;
			LevelChild++;
			NodeChild = NodeSibling->getChild();
			NodeSibling = NodeChild;
			isOption = false;
			isComment = false;
		}
		else{
			// second travel siblings
			if (false==isOption && false==isComment && NodeSibling->getSibling() != NodeChild){
				// ToDo: remove 'LevelChild < 6' limitation
				if (LevelChild >= 3 && LevelChild < 6){
					LineFTL += "\n";
					for (int i = 2; i < LevelChild; i++){
						LineFTL += NodeSibling->isBinary(isParentBinary) ? ";" : ",";
					}
				}
				else{
					LineFTL += NodeSibling->isBinary(isParentBinary) ? ";" : ",";
				}
				NodeSibling = NodeSibling->getSibling();
			}
			else{
				// no more siblings, travel levels backwards
				while (NULL != NodeSibling && NodeSibling->getSibling() == NodeChild){
					LevelChild--;
					NodeSibling = NodeSibling->getParent();
					NodeChild = NodeSibling;
					if (NULL != NodeChild && NULL != NodeChild->getParent()){
						NodeChild = NodeChild->getParent()->getChild();
					}
				}
				if (NULL != NodeSibling){
					LineFTL += "\n";
					for (int i = LevelChild; i > 2; i--) LineFTL += NodeSibling->isBinary(isParentBinary) ? ";" : ",";
					NodeSibling = NodeSibling->getSibling();
				}
			}
		}
		// add Node value
		if (NULL != NodeSibling){
			switch (NodeSibling->getTypeX()){
			case TYPEFTL_OPTION:
				isOption = true;
				LineFTL += ":";
				LineFTL += (int8*)NodeSibling->encodeTextX().getTextX();
				break;
			case TYPEFTL_COMMENT:
				isComment = true;
				LineFTL += "::";
				LineFTL += (int8*)NodeSibling->encodeTextX().getTextX();
				break;
			case TYPEFTL_Local:
				// recover Locald node's address
				if (sizeof(NodeLocal) != NodeSibling->getLength()){
					LineFTL += "--address length mismatch";
				}
				else{
					memcpy(&NodeLocal, NodeSibling->getBuffer(), sizeof(NodeLocal));
					LineFTL += "--";
					if (NULL != NodeLocal){
						LineFTL += NodeLocal->getText();
					}
					else{
						LineFTL += "invalid Local";
					}
				}
				break;
			case TYPEFTL_Local_BINX:
				LineFTL += (int8*)NodeSibling->encodeTextX().getTextX();
				LineFTL += " (invalid Local)";
				break;
			default:
				LineFTL += LevelChild > 2 && false==NodeSibling->isBinary(isParentBinary) ? (int8*)NodeSibling->encodeTextX().getTextX() : NodeSibling->getText();
			}
			LevelChild > 1 ? _StringFTL += LineFTL : 0;
			LineFTL = "";
			isParentBinary = NodeSibling->isBinary(isParentBinary) ? true : false;
		}
		else{
			_StringFTL += "\n";
		}

	}

	return true;
}

bool CmStringFTL::convertFTL2XML(const CmString& FileFTL, const CmString& FileXML)
{
	return convertFTL2XML(FileFTL.getText(), FileXML.getText());
}

bool CmStringFTL::convertFTL2XML(const int8* FileFTL, const int8* FileXML)
{
	// read and analyze FTLight encoded file
	if (false == readFileX(FileFTL)) return false;

	// convert FTLight to XML
	CmString XML;
	CmString Element;
	CmString Value;
	CmStringFTL *NodeChild = this;
	CmStringFTL *NodeSibling = this;
	CmStringFTL *NodeLocal = NULL;
	int LevelChild = 0;
	int OrderSibling = 0;
	int NodeIndex = 0;
	XML = "<FTLight>";
	// travel DataX hierarchy
	while (NULL != NodeSibling){
		// first travel all childs 
		if (NULL != NodeSibling->getChild()){
			LevelChild++;
			NodeChild = NodeSibling->getChild();
			NodeSibling = NodeChild;
			Element = "\n<level";
			Element += LevelChild;
			Element += ">";
			XML += Element;
		}
		else{
			// second travel siblings
			if (NodeSibling->getSibling() != NodeChild){
				OrderSibling++;
				NodeSibling = NodeSibling->getSibling();
			}
			else{
				// no more siblings, travel levels backwards
				while (NULL != NodeSibling && NodeSibling->getSibling() == NodeChild){
					if (LevelChild > 0){
						Element = "\n</level";
						Element += LevelChild;
						Element += ">";
						XML += Element;
					}
					LevelChild--;
					OrderSibling = 0;
					NodeSibling = NodeSibling->getParent();
					NodeChild = NodeSibling;
					if (NULL != NodeChild && NULL != NodeChild->getParent()){
						NodeChild = NodeChild->getParent()->getChild();
					}
				}
				if (NULL != NodeSibling){
					NodeSibling = NodeSibling->getSibling();
				}
			}
		}
		// add Node
		if (NULL != NodeSibling){
			if (NodeSibling->getLength() == 0){
				continue;
			}
			if (CmString("-") == NodeSibling->getText()){
				continue;
			}
			Element = "\n<node";
			Element += LevelChild;
			Element += "_";
			Element += ++NodeIndex;
			Element += ">";
			switch (NodeSibling->getTypeX()){
			case	TYPEFTL_OPERATOR:
				Element += "OPERATOR: ";
				Element += NodeSibling->getText();
				break;
			case	TYPEFTL_LOCATION:
				Element += "LOCATION: ";
				Element += NodeSibling->getText();
				break;
			case	TYPEFTL_IDENTIFIER:
				Element += "IDENTIFIER: ";
				Element += NodeSibling->getText();
				break;
			case	TYPEFTL_QUERY:
				Element += "TYPEFTL_QUERY";
				break;
			case	TYPEFTL_TEXT:
				Element += NodeSibling->getText();
				break;
			case	TYPEFTL_TEXT_BINX:
				Element += NodeSibling->decodeNumX2uint64();
				break;
			case	TYPEFTL_NUMBER:
				Element += NodeSibling->getText();
				break;
			case	TYPEFTL_NUMBER_BINX:
				try{
					Element += NodeSibling->decodeNumX2uint64();
				}
				catch (...){
					Element += 0;
				}
				break;
				break;
			case TYPEFTL_Local:
				 // recover Locald node's address
				if (sizeof(NodeLocal) != NodeSibling->getLength()){
					Element += "--address length mismatch";
				}
				else{
					memcpy(&NodeLocal, NodeSibling->getBuffer(), sizeof(NodeLocal));
					Element += "--";
					if (NULL != NodeLocal){
						Element += NodeLocal->getText();
					}
					else{
						Element += "invalid Local";
					}
				}
				break;
			case TYPEFTL_Local_BINX:
				Element += NodeSibling->getText();
				Element += " (invalid Local)";
				break;
			case	TYPEFTL_FIXED_PARENTS:
				Element += "TYPEFTL_FIXED_PARENTS";
				break;
			case	TYPEFTL_CHECKSUM:
				Element += "TYPEFTL_CHECKSUM";
				break;
			case TYPEFTL_OPTION:
				Element += "::";
				Element += NodeSibling->getText();
				break;
			case	TYPEFTL_DEFAULT:
				Element += "TYPEFTL_DEFAULT";
				break;
			case TYPEFTL_COMMENT:
				Element += ":::";
				Element += NodeSibling->getText();
				break;
			case	TYPEFTL_BINX:
				try{
					Element += NodeSibling->decodeNumX2uint16();
				}
				catch (...){
					Element += 0;
				}
				break;
			case	TYPEFTL_CONTROLX:
				Element += "TYPEFTL_CONTROLX";
				break;
			case	TYPEFTL_FTLightOPEN:
				Element += "TYPEFTL_FTLightOPEN";
				break;
			case	TYPEFTL_FTLightWRAP:
				Element += "TYPEFTL_FTLightWRAP";
				break;
			case	TYPEFTL_BINMCL:
				Element += "TYPEFTL_BINMCL";
				break;
			case	TYPEFTL_BINXBINARY:
				Element += "TYPEFTL_BINXBINARY";
				break;
			case	TYPEFTL_BINXSTRING:
				Element += "TYPEFTL_BINXSTRING";
				break;
			case	TYPEFTL_BINXVALUE:
				Element += "TYPEFTL_BINXVALUE";
				break;
			case	TYPEFTL_BINXTIME:
				Element += "TYPEFTL_BINXTIME";
				break;
			case	TYPEFTL_CMXTOKEN:
				Element += "TYPEFTL_CMXTOKEN";
				break;
			case	TYPEFTL_CMXLINK:
				Element += "TYPEFTL_CMXLINK";
				break;
			case TYPEFTL_BINDIF:
				Element += "TYPEFTL_BINDIF";
				break;
			default:
				Element += "TYPEFTL_NONE";
			}
			Element += "</node";
			Element += LevelChild;
			Element += "_";
			Element += NodeIndex;
			Element += ">";
			XML += Element;
		}
	}
	XML += "\n</FTLight>";

	XML.writeBinary(FileXML);

	return true;
}

const CmString& CmStringFTL::getNode(const CmString& _Address)
{
	// get a node at specified position (create one if it does not existed yet)
	if (false == getPosition(_Address)) return _Address;

	// PosX points to specified node
	return *Navigation->PosX;
}

bool CmStringFTL::addItemX(const CmString& _Val)
{
	// get a BinX coded item
	CmString BinX;
	bin2BinX(&BinX, _Val.getBinary(), (int32)_Val.getLength());

	// increase string length and add a separator and the Item
	size_t PrevLength = Length;
	this->adjustLength(Length + 1 + BinX.getLength());
	setAt((int32)PrevLength, (uint8)';');
	memcpy(pText + PrevLength + 1, BinX.getBuffer(), BinX.getLength());
	DataFormat = DATAFORMAT_FTLight;

	return true;
}
bool CmStringFTL::addItemX(void* _Val)
{
	CmString Item(sizeof(void*));
	Item.setAt(0, _Val);
	return addItemX(Item);
}
bool CmStringFTL::addItemX(uint64 _Val)
{
	CmString Item(sizeof(uint64));
	Item.setAt(0, _Val);
	return addItemX(Item);
}
bool CmStringFTL::addItemX(int64 _Val)
{
	CmString Item(sizeof(int64));
	Item.setAt(0, _Val);
	return addItemX(Item);
}
bool CmStringFTL::addItemX(double _Val)
{
	CmString Item(sizeof(double));
	Item.setAt(0, _Val);
	return addItemX(Item);
}
bool CmStringFTL::addItemX(uint32 _Val)
{
	CmString Item(sizeof(uint32));
	Item.setAt(0, _Val);
	return addItemX(Item);
}
bool CmStringFTL::addItemX(int32 _Val)
{
	CmString Item(sizeof(int32));
	Item.setAt(0, _Val);
	return addItemX(Item);
}
bool CmStringFTL::addItemX(float _Val)
{
	CmString Item(sizeof(float));
	Item.setAt(0, _Val);
	return addItemX(Item);
}
bool CmStringFTL::addItemX(uint16 _Val)
{
	CmString Item(sizeof(uint16));
	Item.setAt(0, _Val);
	return addItemX(Item);
}
bool CmStringFTL::addItemX(int16 _Val)
{
	CmString Item(sizeof(int16));
	Item.setAt(0, _Val);
	return addItemX(Item);
}
bool CmStringFTL::addItemX(uint8 _Val)
{
	CmString Item(sizeof(uint8));
	Item.setAt(0, _Val);
	return addItemX(Item);
}
bool CmStringFTL::addItemX(int8 _Val)
{
	CmString Item(sizeof(int8));
	Item.setAt(0, _Val);
	return addItemX(Item);
}
bool CmStringFTL::addItemX(bool _Val)
{
	CmString Item(sizeof(bool));
	Item.setAt(0, _Val);
	return addItemX(Item);
}

CmDataFormat CmStringFTL::getDataFormat()
{
	return DataFormat;
}
CmDataFormat CmStringFTL::getDataFormat(CmString& _DataFormat)
{
	switch (DataFormat){
	case DATAFORMAT_NONE: _DataFormat = "DATAFORMAT_NONE"; break;
	case DATAFORMAT_FTLight: _DataFormat = "DATAFORMAT_FTLight"; break;
	case DATAFORMAT_STRING: _DataFormat = "DATAFORMAT_STRING"; break;
	case DATAFORMAT_BINARY: _DataFormat = "DATAFORMAT_BINARY"; break;
	case DATAFORMAT_UINT64: _DataFormat = "DATAFORMAT_UINT64"; break;
	case DATAFORMAT_INT64: _DataFormat = "DATAFORMAT_INT64"; break;
	case DATAFORMAT_DOUBLE: _DataFormat = "DATAFORMAT_DOUBLE"; break;
	case DATAFORMAT_UINT32: _DataFormat = "DATAFORMAT_UINT32"; break;
	case DATAFORMAT_INT32: _DataFormat = "DATAFORMAT_INT32"; break;
	case DATAFORMAT_FLOAT: _DataFormat = "DATAFORMAT_FLOAT"; break;
	case DATAFORMAT_UINT16: _DataFormat = "DATAFORMAT_UINT16"; break;
	case DATAFORMAT_INT16: _DataFormat = "DATAFORMAT_INT16"; break;
	case DATAFORMAT_UINT8: _DataFormat = "DATAFORMAT_UINT8"; break;
	case DATAFORMAT_INT8: _DataFormat = "DATAFORMAT_INT8"; break;
	case DATAFORMAT_BOOL: _DataFormat = "DATAFORMAT_BOOL"; break;
	}

	return DataFormat;
}

bool CmStringFTL::insertValueX(const CmString& _Address, const CmString& _ValueX)
{
	// obtain existing/create new node
	CmString ValueX = getNode(_Address);

	// set value
	ValueX.setText(_ValueX);

	return true;
}
bool CmStringFTL::setValueX(const CmString& _Address, const CmString& _ValueX)
{
	// adjust addressed FTLight node in PosX
	if (false == findPosition(_Address)) return false;

	// set new value
	X()->PosX->setText(_ValueX);
	X()->PosX->DataFormat = DATAFORMAT_FTLight;

	return true;
}
bool CmStringFTL::getValueX(const CmString& _Address, CmStringFTL& _ValueX, CmString _StartAddr)
{
	// adjust addressed FTLight node in PosX
	if (false == findPosition(_Address, _StartAddr)) return false;

	// convert from BinX to binary data if necessary
	if (DATAFORMAT_FTLight == X()->PosX->DataFormat){
		switch (X()->PosX->TypeX){
		case TYPEFTL_BINX: X()->PosX->decodeBinX(); break;
		default: break;
		}
	}

	// copy data and data format information to ValueX
	_ValueX.setLength(X()->PosX->getLength());
	memcpy(_ValueX.getBuffer(), X()->PosX->getBinary(), X()->PosX->getLength());
	_ValueX.DataFormat = X()->PosX->DataFormat;
	_ValueX.TypeX = X()->PosX->TypeX;

	return true;
}

bool CmStringFTL::getValue(const char *_Address, CmDataFormat _DataFormat, CmStringFTL **_Value)
{
	// adjust addressed FTLight node in PosX
	CmString Address_l(_Address);
	if (false == findPosition(Address_l)) return false;

	// convert from FTLight to appropriate data format if necessary
	if (DATAFORMAT_FTLight == X()->PosX->DataFormat){
		switch (X()->PosX->TypeX){
		case TYPEFTL_BINX: X()->PosX->decodeBinX(); break;
		case TYPEFTL_NUMBER:	X()->PosX->decodeNum(_DataFormat); break;
		default: break;
		}
	}

	// return found value
	if (NULL != _Value){
		*_Value = X()->PosX;
	}

	return true;
}
bool CmStringFTL::getValue(const char *_Address, CmString& _Val)
{
	CmStringFTL *Value;
	if (false == getValue(_Address, DATAFORMAT_STRING, &Value)) return false;
	_Val = *Value;

	return true;
}
bool CmStringFTL::getValue(const char *_Address, void*& _Val)
{
	CmStringFTL *Value;
	if (false == getValue(_Address, DATAFORMAT_BINARY, &Value)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const char *_Address, uint64& _Val)
{
	CmStringFTL *Value;
	if (false == getValue(_Address, DATAFORMAT_UINT64, &Value)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const char *_Address, int64& _Val)
{
	CmStringFTL *Value;
	if (false == getValue(_Address, DATAFORMAT_INT64, &Value)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const char *_Address, double& _Val)
{
	CmStringFTL *Value;
	if (false == getValue(_Address, DATAFORMAT_DOUBLE, &Value)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const char *_Address, uint32& _Val)
{
	CmStringFTL *Value;
	if (false == getValue(_Address, DATAFORMAT_UINT32, &Value)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const char *_Address, int32& _Val)
{
	CmStringFTL *Value;
	if (false == getValue(_Address, DATAFORMAT_INT32, &Value)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const char *_Address, float& _Val)
{
	CmStringFTL *Value;
	if (false == getValue(_Address, DATAFORMAT_FLOAT, &Value)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const char *_Address, uint16& _Val)
{
	CmStringFTL *Value;
	if (false == getValue(_Address, DATAFORMAT_UINT16, &Value)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const char *_Address, int16& _Val)
{
	CmStringFTL *Value;
	if (false == getValue(_Address, DATAFORMAT_INT16, &Value)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const char *_Address, uint8& _Val)
{
	CmStringFTL *Value;
	if (false == getValue(_Address, DATAFORMAT_UINT8, &Value)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const char *_Address, int8& _Val)
{
	CmStringFTL *Value;
	if (false == getValue(_Address, DATAFORMAT_INT8, &Value)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const char *_Address, bool& _Val)
{
	CmStringFTL *Value;
	if (false == getValue(_Address, DATAFORMAT_BOOL, &Value)) return false;
	Value->getAt(0, _Val);

	return true;
}
// CmString Addr
bool CmStringFTL::getValue(const CmString& _Addr, CmDataFormat _DataFormat, CmStringFTL **_Val, CmString _StartAddr)
{
	// adjust addressed FTLight node in PosX
	if (false == findPosition(_Addr, _StartAddr)) return false;

	// convert from FTLight to appropriate data format if necessary
	if (DATAFORMAT_FTLight == X()->PosX->DataFormat && _DataFormat != DATAFORMAT_STRING){
		switch (X()->PosX->TypeX){
		case TYPEFTL_BINX: X()->PosX->decodeBinX(); break;
		case TYPEFTL_NUMBER:	X()->PosX->decodeNum(_DataFormat); break;
		default: break;
		}
	}

	// return found value
	if (NULL != _Val){
		*_Val = X()->PosX;
	}

	return true;
}
bool CmStringFTL::getValue(const CmString& _Addr, CmString& _Val, CmString _StartAddr)
{
	CmStringFTL *Value;
	if (false == getValue(_Addr, DATAFORMAT_STRING, &Value, _StartAddr)) return false;
	_Val = *Value;

	return true;
}
bool CmStringFTL::getValue(const CmString& _Addr, void*& _Val, CmString _StartAddr)
{
	CmStringFTL *Value;
	if (false == getValue(_Addr, DATAFORMAT_BINARY, &Value, _StartAddr)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const CmString& _Addr, uint64& _Val, CmString _StartAddr)
{
	CmStringFTL *Value;
	if (false == getValue(_Addr, DATAFORMAT_UINT64, &Value, _StartAddr)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const CmString& _Addr, int64& _Val, CmString _StartAddr)
{
	CmStringFTL *Value;
	if (false == getValue(_Addr, DATAFORMAT_INT64, &Value, _StartAddr)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const CmString& _Addr, double& _Val, CmString _StartAddr)
{
	CmStringFTL *Value;
	if (false == getValue(_Addr, DATAFORMAT_DOUBLE, &Value, _StartAddr)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const CmString& _Addr, uint32& _Val, CmString _StartAddr)
{
	CmStringFTL *Value;
	if (false == getValue(_Addr, DATAFORMAT_UINT32, &Value, _StartAddr)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const CmString& _Addr, int32& _Val, CmString _StartAddr)
{
	CmStringFTL *Value;
	if (false == getValue(_Addr, DATAFORMAT_INT32, &Value, _StartAddr)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const CmString& _Addr, float& _Val, CmString _StartAddr)
{
	CmStringFTL *Value;
	if (false == getValue(_Addr, DATAFORMAT_FLOAT, &Value, _StartAddr)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const CmString& _Addr, uint16& _Val, CmString _StartAddr)
{
	CmStringFTL *Value;
	if (false == getValue(_Addr, DATAFORMAT_UINT16, &Value, _StartAddr)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const CmString& _Addr, int16& _Val, CmString _StartAddr)
{
	CmStringFTL *Value;
	if (false == getValue(_Addr, DATAFORMAT_INT16, &Value, _StartAddr)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const CmString& _Addr, uint8& _Val, CmString _StartAddr)
{
	CmStringFTL *Value;
	if (false == getValue(_Addr, DATAFORMAT_UINT8, &Value, _StartAddr)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const CmString& _Addr, int8& _Val, CmString _StartAddr)
{
	CmStringFTL *Value;
	if (false == getValue(_Addr, DATAFORMAT_INT8, &Value, _StartAddr)) return false;
	Value->getAt(0, _Val);

	return true;
}
bool CmStringFTL::getValue(const CmString& _Addr, bool& _Val, CmString _StartAddr)
{
	CmStringFTL *Value;
	if (false == getValue(_Addr, DATAFORMAT_BOOL, &Value, _StartAddr)) return false;
	Value->getAt(0, _Val);

	return true;
}


int64 CmStringFTL::incrementSequenceNumber()
{
	return ++X()->sync.SequenceNumber;
}
bool CmStringFTL::getConfigPath(CmString _AppData, CmString _AppCompany, CmString _AppProgram, CmString _AppVersion)
{
	// check if AppData path is available
	setText(_AppData);
	if (false == PathFileExistsA(getText())) return false;

	// get company path
	*this += "\\";
	*this += _AppCompany;
	if (false == PathFileExistsA(getText())){
		// create company path
		if (-1 == _mkdir(getText())) return false;
	}

	// get program path
	*this += "\\";
	*this += _AppProgram;
	if (false == PathFileExistsA(getText())){
		// create program path
		if (-1 == _mkdir(getText())) return false;
	}

	// get profile_version path
	if (_AppProgram.getLength() > 0 && _AppVersion.getLength() > 0){
		*this += "\\";
		*this += _AppProgram;
		*this += "_";
		*this += _AppVersion;
		if (false == PathFileExistsA(getText())){
			// create program path
			if (-1 == _mkdir(getText())) return false;
		}
	}

	// trailing backslash
	*this += "\\";

	return true;
}

bool CmStringFTL::getIdentifier(CmString& _Info, CmUURI& _UURI)
{
	// find @ before delimiters ,:;=
	bool isAt = false;
	int32 i;
	for (i = 0; i < int32(_Info.getLength()); i++){
		_Info[i] == '@' ? i > 0 ? _Info[i - 1] != '\\' ? isAt = true : 0 : 0 : 0; 
		if (_Info[i] == ',' || _Info[i] == ':' || _Info[i] == ';' || _Info[i] == '='){
			break; // delimiter found
		}
	}
	// without @ it is not a UURI
	if (false == isAt) return false;

	// return found UURI
	_UURI.setUURI(_Info, i-1);

	return true;
}

bool CmStringFTL::encodeValueFormat(CmString& _ValueFormat, CmPhysicalUnit _Unit, uint64 _Factor, uint64 _Divisor, uint64 _Base, int64 _Exponent)
{
	CmString Num;
	// format identifier
	setFTLightType(_ValueFormat, BINX_VALUE);
	// physical unit
	if ((_Unit != 0) || (_Factor != 1) || (_Divisor != 1) || (_Base != 1) || (_Exponent != 0)){
		_ValueFormat += "`";
		num2BinX(&Num, _Unit);
		_ValueFormat += Num;
	}
	// factor
	if ((_Factor!=1) || (_Divisor != 1) || (_Base != 1) || (_Exponent != 0)){
		_ValueFormat += "`";
		num2BinX(&Num, _Factor);
		_ValueFormat += Num;
	}
	// devisor
	if ((_Divisor != 1) || (_Base != 1) || (_Exponent != 0)){
		_ValueFormat += "`";
		num2BinX(&Num, _Divisor);
		_ValueFormat += Num;
	}
	// base
	if ((_Base != 1) || (_Exponent != 0)){
		_ValueFormat += "`";
		num2BinX(&Num, _Base);
		_ValueFormat += Num;
	}
	// exponent
	if (_Exponent!=0){
		_ValueFormat += "`";
		_Exponent < 0 ? _ValueFormat += "`" : 0;
		num2BinX(&Num, _Base);
		_ValueFormat += Num;
	}

	return true;
}
bool CmStringFTL::decodeValueFormat(const CmString& /*_ValueFormat*/, CmPhysicalUnit& /*_Unit*/, double /*_Value*/)
{

	return true;
}
bool CmStringFTL::encodeTimeFormat(CmString& _TimeFormat, uint64 _Factor, uint64 _Divisor, uint64 _Base, int64 _Exponent)
{
	CmString Num;
	// validate conversion table
	if (false == isConversionTable()) return false;
	// format identifier
	setFTLightType(_TimeFormat, BINX_TIME);
	// factor
	if ((_Factor != 1) || (_Divisor != 1) || (_Base != 1) || (_Exponent != 0)){
		_TimeFormat += "`";
		num2BinX(&Num, _Factor);
		_TimeFormat += Num;
	}
	// devisor
	if ((_Divisor != 1) || (_Base != 1) || (_Exponent != 0)){
		_TimeFormat += "`";
		num2BinX(&Num, _Divisor);
		_TimeFormat += Num;
	}
	// base
	if ((_Base != 1) || (_Exponent != 0)){
		_TimeFormat += "`";
		num2BinX(&Num, _Base);
		_TimeFormat += Num;
	}
	// exponent
	if (_Exponent != 0){
		_TimeFormat += "`";
		_Exponent < 0 ? _TimeFormat += "`" : 0;
		num2BinX(&Num, _Base);
		_TimeFormat += Num;
	}

	return true;
}
bool CmStringFTL::decodeTimeFormat(const CmString& /*_TimeFormat*/, double /*_Time*/)
{

	return true;
}

int32 CmStringFTL::BinX2bin(uint8* _pBuf, int32 _nBufSize, CmString& _mBinX, bool _fTail)
{
	size_t nBinXLen = _mBinX.getLength();
	size_t nBinLen = getLengthBinX2bin(nBinXLen, _fTail);
	int32 nPos;
	uint8 uchMerge;
	int64 n64Used = 0;
	int32 nShift;
	int32 nFactor;
	uint8 uchCode;
	bool  fMerge;
	uint64 uQuadrupel;

	// Check if buffer size is sufficient
	memset(_pBuf, 0, _nBufSize);
	if (_nBufSize<(int32)nBinLen){
		//throw CmException("Unsufficient binary buffer size", (uint32)_nBufSize);
		return 0;
	}
	for (nPos = 0; nPos<(int32)nBinXLen; nPos += 4){
		//Decode BinX characters
		uQuadrupel = 0;
		nFactor = 1;
		for (int i = nPos; i<nPos + 4 && i<(int32)nBinXLen; i++){
			uchCode = *(_mBinX.getBuffer() + i);
			switch (uchCode){
			case 248: uchCode = 12; break;		//avoid 44 - comma
			case 249: uchCode = 13; break;		//avoid 45 - minus sign
			case 250: uchCode = 26; break;		//avoid 58 - colon
			case 251: uchCode = 27; break;		//avoid 59 - semicolon
			case 252: uchCode = 29; break;		//avoid 61 - equal sign
			case 253: uchCode = 32; break;		//avoid	64 - @ sign
			case 254: uchCode = 64; break;		//avoid 96 - back apostroph
			case 255: uchCode = 95; break;		//avoid 127- del
			default:  uchCode -= 32;
			}
			uQuadrupel += uchCode * nFactor;
			nFactor *= RADIX_216;
		}
		// Generate bit sequence
		nShift = (int32)(n64Used % 8);
		fMerge = true;
		if (0 == nShift){
			fMerge = false;
		}
		else{
			uQuadrupel <<= nShift;
		}
		if (true == fMerge){
			uchMerge = *(_pBuf + (int32)(n64Used / 8));
			uQuadrupel += uchMerge;
		}
		// Fill buffer
		if (n64Used / 8 + 4 < (int32)nBinLen){
			*(int32*)(_pBuf + (int32)(n64Used / 8)) = (int32)uQuadrupel;
			if (nShift > 1){
				*(_pBuf + (int32)(n64Used / 8) + 4) = (int8)(uQuadrupel >> 32);
			}
		}
		else{
			for (int32 i = (int32)(n64Used / 8); i<(int32)nBinLen; i++){
				*(_pBuf + i) = (int8)(uQuadrupel);
				uQuadrupel >>= 8;
			}
		}
		n64Used += 31;
	}

	return (int32)nBinLen;
}
bool CmStringFTL::isDataTypeIdentifier(CmString& _FTLight, FTLight_DATA_TYPE_IDENTIFIER _eDataType)
{
	uint32 uNum = 0;
	// Evaluate first 4 bytes
	if (_FTLight.getLength() >= 4){
		for (int i = 3; i >= 0; i--){
			uNum *= 216;
			uNum += *((uint8*)(_FTLight.getBuffer() + i)) - 32;
		}
		if ((uint32)_eDataType == uNum){
			return true;
		}
	}
	return false;
}
CmString* CmStringFTL::num2BinX(CmString* _pBinX, uint64 _u64Num)
{
	return num2BinX(_pBinX, &_u64Num);
}
CmString* CmStringFTL::num2BinX(CmString* _pBinX, uint64* _p64Num)
{
	uint64 u64Num = *_p64Num;
	int32  nDigits;
#ifdef gcc
	if (0 == (u64Num & 0xFFFFFFFF80000000ull)){
		for (nDigits = 1; u64Num >= RADIX_216; nDigits++){
			u64Num /= RADIX_216;
		}
	}
	else if (0 == (u64Num & 0xC000000000000000ull)){
#else
	if (0 == (u64Num & 0xFFFFFFFF80000000)){
		for (nDigits = 1; u64Num >= RADIX_216; nDigits++){
			u64Num /= RADIX_216;
		}
	}
	else if (0 == (u64Num & 0xC000000000000000)){
#endif
		u64Num >>= 31;
		for (nDigits = 5; u64Num >= RADIX_216; nDigits++){
			u64Num /= RADIX_216;
		}
	}
	else{
		nDigits = 9;
	}
	return bin2BinX(_pBinX, (uint8*)_p64Num, 8, nDigits);
	}
uint64 CmStringFTL::BinX2num(CmString& _mBinX)
{
	uint64 u64Num;
	BinX2bin((uint8*)&u64Num, 8, _mBinX, true);
	return u64Num;
}

//------ private --------------------------------------------------------------------------

// FTLight bin code, byt<=215
uint8 CmStringFTL::Symbol2BinX(uint8 _Symbol)
{
	uint8 b;
	switch (_Symbol)
	{
	case 12: b = 248; break;      // avoid ,
	case 13: b = 249; break;      // avoid -
	case 26: b = 250; break;      // avoid :
	case 27: b = 251; break;      // avoid ;
	case 29: b = 252; break;      // avoid =
	case 32: b = 253; break;      // avoid @
	case 64: b = 254; break;      // avoid `
	case 95: b = 255; break;      // avoid del
	default: b = _Symbol + 32;
	}
	return b;
}

// FTLight bin decode
uint8 CmStringFTL::BinX2Symbol(uint8 _BinX)
{
	uint8 b;
	switch (_BinX)
	{
	case 248: b = 12; break;      // avoid ,
	case 249: b = 13; break;      // avoid -
	case 250: b = 26; break;      // avoid :
	case 251: b = 27; break;      // avoid ;
	case 252: b = 29; break;      // avoid =
	case 253: b = 32; break;      // avoid @
	case 254: b = 64; break;      // avoid `
	case 255: b = 95; break;      // avoid del
	default: b = _BinX - 32;
	}
	return b;
}

CmString* CmStringFTL::bin2BinX(CmString* _pBinX, const uint8* _pBuf, int32 _nBinLen, int32 _nDigits)
{
	int64 n64Bits  = 8*_nBinLen;
	int32 nBinXLen = _nDigits>0?_nDigits:(int32)(4*((n64Bits)/31)+((n64Bits)%31)/8+((n64Bits)%31>0?1:0));
	int64 n64Used;
	int32 nShift;
	int32 nPos = 0;
	int64 n64Factor;
	uint8 uchCode;
	uint32 uQuadrupel;
	uint64 u64Register;

	// check conversion table
	if (false == isConversionTable()) return false;

	// Determine length of the resulting BinX string
	_pBinX->setLength(nBinXLen);

	// Split input bit field into 31-bit chunks
	for (n64Used=0;n64Used<n64Bits;n64Used+=31){
		//Load register
		if (n64Used/8+8<=_nBinLen){
			u64Register = *((uint64*)(_pBuf+(int32)(n64Used/8)));
		}else{
			u64Register = 0;
			n64Factor   = 1;
			for (int i=(int32)(n64Used/8);i<_nBinLen;i++){
				u64Register += *(_pBuf+i) * n64Factor;
				n64Factor   *= 256;
			}
		}
		// Bit shift
		nShift = (int32)(n64Used%8);
		if (0==nShift){
			uQuadrupel = (uint32)u64Register;
		}else{
			u64Register >>= nShift;
			uQuadrupel = (uint32)u64Register;
		}
		uQuadrupel &= 0x7FFFFFFF;
		// Code generation
		// ToDo: accellerate by using 2 quadrupels in one conversion
		for (int i=0;i<4 && nPos<nBinXLen;i++){
			uchCode = uQuadrupel%RADIX_216;
			uQuadrupel /= RADIX_216;
			uchCode = *(uint8*)(BinMCLconvert+uchCode);
			_pBinX->setAt(nPos++,uchCode);
		}
	}
	return _pBinX;
}

size_t CmStringFTL::getLengthBinX2bin(size_t _LengthX, bool _fTail)
{
	int32 nBinXbits = 31 * ((int32)_LengthX / 4) + (((int32)_LengthX % 4) == 0 ? 0 : 8 * ((int32)_LengthX % 4) - 1);
	size_t nBinLen = nBinXbits / 8 + ((_fTail == true) && (nBinXbits<64) && (nBinXbits % 8>0) ? 1 : 0);

	return nBinLen;
}

bool CmStringFTL::bin2BinMCL(int _n, uint16 *_inp, uint16 *_out)
{
	int i;
	union shifter {
	uint16 wrd[2];			//[0]=lsw [1]=msw
	unsigned long lwr;
	} shf;

	// check conversion table
	if (false == isConversionTable()) return false;

	// convert n groups of 15 words into 32n FTLight legal chars
	for (i = 0; i<_n; i++){									//30270 = 32768*15/16
		shf.lwr = 0;

		shf.wrd[0]=*_inp++;									//get input word
		shf.lwr=shf.lwr<<1;									//save msb
		*_out++=*(uint16*)(BinMCLconvert+(shf.wrd[0]>>1));	//table lookup and store two chars

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinMCLconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinMCLconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinMCLconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinMCLconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinMCLconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinMCLconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinMCLconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinMCLconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinMCLconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinMCLconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinMCLconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinMCLconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinMCLconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinMCLconvert+(shf.wrd[0]>>1));

		*_out++=*(uint16*)(BinMCLconvert+shf.wrd[1]);                   //store the word made of msb's
	}
	return true;
}

bool CmStringFTL::BinMCL2bin(int _n, uint16 *_inp, uint16 *_out)
{
	//unpacks n 32 byte groups into n 15 word groups
	uint16 w, wb[15];
	for (int i=0;i<_n;i++)	{
		for (int j=0;j<15;j++){
			w=*_inp++;
			wb[j] = 216 * SYMBOL[(uint8)(w / 256)] + SYMBOL[(uint8)(w % 256)];
		}
		w=*_inp++;
		w = 216 * SYMBOL[(uint8)(w / 256)] + SYMBOL[(uint8)(w % 256)];
		w=w<<1;
		for (int j=0;j<15;j++)
		{
			*_out++=(wb[j])+(w&32768);
			w=w<<1;
		}
	}
	return true;
}

bool CmStringFTL::val2BinDIF(CmVector<uint8>& _BinDIF, CmVector<int64>& _Values, int32 _insertAbsoluteValue)
{
	CmVector<uint8>& D = _BinDIF;
	CmVector<int64>& V = _Values;

	// D : [i] = BinDIF
	// V : [i] = Value

	// check conversion tables
	if (false == isConversionTable()) return false;

	// convert all values to BinDIF symbols
	int32 I = 0;
	int64 Value, PrevVal = V[0];
	uint8 Repetition = 210;
	uint8 PrevBinX = BINX[210];
	uint8 PrevPrevBinX = PrevBinX;
	int32 ValDIFcount = _insertAbsoluteValue;
	D.clear();
	for (int i = 0; i < V.getLength(); i++){
		// get dif
		int64 ValDIF = (Value = V[i]) - PrevVal;
		PrevVal = Value;
		// check for absolute value
		if (abs(ValDIF) > 100 || ValDIFcount >= _insertAbsoluteValue){
			// finalize eventually still running repetition
			if (Repetition > 210){
				Repetition = 210;
				I++;
			}
			// insert absolute value
			int64 Digits216 = getDigits216(Value);	// negative Value will be transformed to positive range
			D[I++] = BINX[uint8(200 + Digits216)];
			for (int d = 0; d < Digits216; d++){
				PrevPrevBinX = PrevBinX;
				D[I++] = PrevBinX = BINX[Value % 216];
				Value = Value / 216;
			}
			ValDIFcount = 0;
		}
		else{
			// insert DIF value
			uint8 BinX = BINX[uint8(ValDIF + 100)];
			// insert BinX and take care for repeated BinX values
			if (0 == ValDIFcount){
				// no repeated BinX after absolute value (potential risk of misleading symbols)
				PrevPrevBinX = PrevBinX;
				D[I++] = PrevBinX = BinX;
				ValDIFcount++;
			}
			else if (Repetition > 210){
				// previous BinX repetition
				if ((BinX == PrevPrevBinX) && (Repetition != 215)){
					// repetion further continues
					D[I] = BINX[++Repetition];
				}
				else{
					// max repetitions exceeded, insert new DIF
					Repetition = 210;
					PrevPrevBinX = PrevBinX;
					D[++I] = PrevBinX = BinX;
					ValDIFcount++;
					I++;
				}
			}
			else if (BinX != PrevBinX){
				// no repeated BinX
				PrevPrevBinX = PrevBinX;
				D[I++] = PrevBinX = BinX;
				ValDIFcount++;
			}
			else if ((BinX == PrevBinX) && (BinX == PrevPrevBinX) && (ValDIFcount > 1)){
				// start new repetition
				D[--I] = PrevBinX = BINX[Repetition = 211];
			}
			else{
				// no repeated BinX
				PrevPrevBinX = PrevBinX;
				D[I++] = PrevBinX = BinX;
				ValDIFcount++;
			}
		}
	}

	return true;
}

bool CmStringFTL::BinDIF2val(CmVector<uint8>& _BinDIF, CmVector<int64>& _Values)
{
	CmVector<uint8>& D = _BinDIF;
	CmVector<int64>& V = _Values;

	// D : [i] = BinX
	// V : [i] = Value

	// check conversion tables
	if (false == isConversionTable()) return false;

	// convert a BinDIF data stream to int64 values
	int32 I = 0;
	int32 Repetitions = 210;
	int64 Value = 0;
	uint8 PrevSymbol = 210;
	uint8 AbsoluteSymbols = 200;
	int64 AbsoluteRange = 1;
	V.clear();
	for (int32 i = 0; i < D.getLength(); i++){
		uint8 Symbol = SYMBOL[D[i]];
		if (PrevSymbol > 200 && PrevSymbol < 210){
			if (AbsoluteSymbols++ < PrevSymbol){
				// assemble an absolute value
				Value = Value + Symbol * AbsoluteRange;
				AbsoluteRange *= 216;
			}
			if (AbsoluteSymbols >= PrevSymbol){
				// absolute value finished, adjust sign
				Value >= AbsoluteRange / 2 ? Value -= AbsoluteRange : 0;
				V[I++] = Value;
				PrevSymbol = 210;
			}
		}
		else if (Symbol > 200 && Symbol < 210){
			// start of absolute value
			PrevSymbol = Symbol;
			AbsoluteSymbols = 200;
			AbsoluteRange = 1;
			Value = 0;
		}
		else if (Symbol > 210){
			// repetition
			while (Repetitions++ <= Symbol){
				V[I++] = Value = Value + PrevSymbol - 100;
			}
			Repetitions = 210;
			PrevSymbol = Symbol;
		}
		else if (Symbol == 210){
			// empty value: insert minimal int64 value
			V[I++] = MIN_INT64;
			PrevSymbol = Symbol;
		}
		else {
			// DIF value
			V[I++] = Value = Value + Symbol - 100;
			PrevSymbol = Symbol;
		}
	}

	return true;
}

bool CmStringFTL::asVector(CmVector<int64>& _Data)
{
	// align vector lengths with string length
	CmVector<uint8> BinDIF;
	BinDIF.setLength(int32(Length));
	// copy data to vector 
	memcpy(BinDIF.getData(), pText, Length);
	// decode BinDIF data to 64bit values
	return BinDIF2val(BinDIF, _Data);
}

int64 CmStringFTL::getDigits216(int64& _Value)
{
#define RANGE_1		216
#define RANGE_2		46656
#define RANGE_3		10077696
#define RANGE_4		2176782336
#define RANGE_5		470184984576
#define RANGE_6		101559956668416
#define RANGE_7		21936950640377856
#define RANGE_8		4738381338321616896

	// determine the minimal amount of digits for representing Value base 216 in BinDIF format
	int64 Value = abs(_Value);
	if (Value < RANGE_1 / 2){
		_Value < 0 ? _Value += RANGE_1 : 0;
		return 1;
	}
	else if (Value < RANGE_2 / 2){
		_Value < 0 ? _Value += RANGE_2 : 0;
		return 2;
	}
	else if (Value < RANGE_3 / 2){
		_Value < 0 ? _Value += RANGE_3 : 0;
		return 3;
	}
	else if (Value < RANGE_4 / 2){
		_Value < 0 ? _Value += RANGE_4 : 0;
		return 4;
	}
	else if (Value < RANGE_5 / 2){
		_Value < 0 ? _Value += RANGE_5 : 0;
		return 5;
	}
	else if (Value < RANGE_6 / 2){
		_Value < 0 ? _Value += RANGE_6 : 0;
		return 6;
	}
	else if (Value < RANGE_7 / 2){
		_Value < 0 ? _Value += RANGE_7 : 0;
		return 7;
	}
	else if (Value < RANGE_8 / 2){
		_Value < 0 ? _Value += RANGE_8 : 0;
		return 8;
	}
	else {
		// NOTE: huge negative numbers where abs(_Value) >= RANGE_8 are not suppported
		return 9;
	}
}

bool CmStringFTL::setFTLightType(CmString& _BinX, FTLight_DATA_TYPE_IDENTIFIER _FTLightType)
{
	// set result length
	_BinX.setLength(4);
	// decompose identifier value
	uint32 TypeFTL = _FTLightType;
	for (int i = 0; i < 4; i++){
		_BinX.setAt(i, BINX[(uint8)(TypeFTL % 216)]);
		TypeFTL -= TypeFTL % 216;
		TypeFTL /= 216;
	}

	return true;
}

bool CmStringFTL::getFTLightType(CmString& _BinX, FTLight_DATA_TYPE_IDENTIFIER& _FTLightType)
{
	// initialize return value
	_FTLightType = FTLight_NONE;

	// check data availabílity
	uint32 ActiveStart;
	uint32 ActiveLength;
	int32 LineNumber;
	_BinX.getActive(ActiveStart, ActiveLength, LineNumber);
	if (ActiveLength < 4) return false;

	// decode field
	uint32 FTLightType = 0;
	for (int i = 3; i >= 0; i--)
	{
		FTLightType *= 216;
		FTLightType += SYMBOL[(uint8)(*(_BinX.getBuffer() + ActiveStart + i))];
	}

	// check for valid range
	if ((FTLightType < FTLight_BINX_MIN) || (FTLightType > FTLight_BINX_MAX)) return false;

	_FTLightType = (FTLight_DATA_TYPE_IDENTIFIER)FTLightType;

	return true;
}
bool CmStringFTL::getLastChild(CmStringFTL** _LastChild)
{
	// check parameter
	if (NULL == _LastChild) return false;

	// search for last child
	*_LastChild = this->Sibling;
	while ((*_LastChild)->Sibling != this){
		*_LastChild = (*_LastChild)->Sibling;
	}

	return true;
}

bool CmStringFTL::getParent(CmStringFTL** _Parent)
{
	// check parameter
	if ((NULL == _Parent) || (NULL == Parent)) return false;

	// return parent node
	*_Parent = Parent;

	return true;
}


bool CmStringFTL::getNextItemX(CmString& _LineX, CmString& ItemX, CmTypeFTL& _TypeX, bool& _isStartOfCollection, bool _isParentBinary)
{
	// initialize control variables
	bool isStartOfLine = false;
	int32 LineNumber_l;
	uint32 ActiveStart_l;
	uint32 ActiveLength_l;
	uint32 HighlightStart_l;
	uint32 HighlightLength_l;
	_LineX.getActive(ActiveStart_l, ActiveLength_l, LineNumber_l);
	_LineX.getHighlight(HighlightStart_l, HighlightLength_l);
	if (ActiveStart_l == HighlightStart_l && ActiveLength_l == HighlightLength_l){
		isStartOfLine = true;
	}
	HighlightStart_l = ActiveStart_l;
	HighlightLength_l = 0;
	_LineX.setHighlight(HighlightStart_l, HighlightLength_l);
	_isStartOfCollection = false;
	_TypeX = TYPEFTL_TEXT;

	// initialize ItemX
	ItemX.setLength(0);

	// check data availability
	if (0 == ActiveLength_l) return false;

	// flags to run a state machine
	bool isDetermined = true;
	bool isIdentifier = false;
	bool isNumber = true;
	bool isAddress = true;
	bool isAddressDelimiter = false;
	bool isChecksum = false;
	bool isText = true;
	bool isLineEnd = false;
	bool isQuery = false;
	bool isDelete = false;
	bool isEscape = false;
	bool isDelimiter = true;
	bool isNextDelimiter = false;
	bool isFramework = false;
	bool isOption = false;
	bool isComment = false;
	bool isProcessed = true;		// processing flag for option/comment

	// process first byte (usually a delimiter, however exceptions exist)
	ActiveLength_l--;
	HighlightLength_l++;
	const uint8* src = _LineX.getBinary() + ActiveStart_l++;
	switch (*src){
	case 0:  isLineEnd = true; break;	// end of string
	case 10: isLineEnd = true; break;	// line feed
	case 13: isLineEnd = true; break;	// carriage return
	case 44: break;	// comma
	case 58: _isStartOfCollection = true; isFramework = true; break;	// colon
	case 59: isText = false; isNumber = false; break;	// semicolon
	case 61: isText = false; isNumber = false; isChecksum = true; _isStartOfCollection = true; break;	// equal sign
	case 64: return false;	// "at" sign, not allowed as first data in a field
	case 96: isQuery = true; isAddress = false; break;	// backtick: query, however, it has to be a single char
	case 127: isDelete = true; break;	// delete
	case '0': isDelimiter = false; break;
	case '1': isDelimiter = false; break;
	case '2': isDelimiter = false; break;
	case '3': isDelimiter = false; break;
	case '4': isDelimiter = false; break;
	case '5': isDelimiter = false; break;
	case '6': isDelimiter = false; break;
	case '7': isDelimiter = false; break;
	case '8': isDelimiter = false; break;
	case '9': isDelimiter = false; break;
	case '-': isDelimiter = false; isAddress = false; break;
	case '+': isDelimiter = false; isAddress = false; break;
	case '.': isDelimiter = false; isAddress = false; break;
	case 'E': isDelimiter = false; isAddress = false; break;
	case 'e': isDelimiter = false; isAddress = false; break;
	case 'X': isDelimiter = false; isAddress = false; break;
	case 'x': isDelimiter = false; isAddress = false; break;
	case 'A': isDelimiter = false; isAddress = false; break;
	case 'a': isDelimiter = false; isAddress = false; break;
	case 'B': isDelimiter = false; isAddress = false; break;
	case 'b': isDelimiter = false; isAddress = false; break;
	case 'C': isDelimiter = false; isAddress = false; break;
	case 'c': isDelimiter = false; isAddress = false; break;
	case 'D': isDelimiter = false; isAddress = false; break;
	case 'd': isDelimiter = false; isAddress = false; break;
	case 'F': isDelimiter = false; isAddress = false; break;
	case 'f': isDelimiter = false; isAddress = false; break;

	default: isDelimiter = false; isNumber = false; isAddress = false; break;
	}

	// implicite (left out) identifier if a line starts with a delimiter
	if (isStartOfLine && isDelimiter){
		ActiveStart_l--;
		ActiveLength_l++;
		HighlightLength_l--;
		_LineX.setActive(ActiveStart_l, ActiveLength_l);
		_LineX.setHighlight(HighlightStart_l, HighlightLength_l);
		// a (left out) identifier at start of line is common in FTLight
		_TypeX = TYPEFTL_IDENTIFIER;
		return true;
	}

	// finish when line end has been reached
	if (isLineEnd){
		_LineX.setActive(ActiveStart_l, ActiveLength_l);
		_LineX.setHighlight(HighlightStart_l, HighlightLength_l);
		// line end at the beginning should never happen
		return false;	
	}

	// check if the data type could be uniquely determined
	if (isStartOfLine && false == isDelimiter)
		isDetermined = false;

	// skip delimiter in the beginning
	if (isDelimiter){
		HighlightStart_l++;
		HighlightLength_l--;
	}

	// find next delimiter
	while (ActiveLength_l > 0){
		src = _LineX.getBinary() + ActiveStart_l;
		switch (*src){
		case 0:  isLineEnd = true; break;	// end of string
		case 10: isLineEnd = true; break;	// line feed
		case 13: isLineEnd = true; break;	// carriage return
		case 44: // comma
			if (isEscape == false){
				isNextDelimiter = true;
				isChecksum = false;
				break;
			}
			isEscape = false;
			break;
		case 58: // colon
			if (isEscape == false){
				if (isFramework && 0 == HighlightLength_l){
					// option detected
					isFramework = false;
					isOption = true;
					isProcessed = false;
				}
				else
				if (isOption && 0 == HighlightLength_l){
					// comment detected
					isOption = false;
					isComment = true;
					isProcessed = false;
				}
				else{
					isNextDelimiter = true;
				}
				isChecksum = false;
				break;
			}
			isEscape = false;
			break;
		case 59: // semicolon
			if (isEscape == false){
				isNextDelimiter = true;
				isChecksum = false;
				break;
			}
			isEscape = false;
			break;
		case 61: // equal sign
			if (isEscape == false){
				isNextDelimiter = true;
				isChecksum = true;
				break;
			}
			isEscape = false;
			break;
		case 64: // "at" sign
			if (isEscape == false){
				isIdentifier = true;
				break;
			}
			isEscape = false;
			isAddress = false;
			break;
		case 92: // backslash
			if (isText /*text field*/ && (isDetermined || _isParentBinary == false) /*no synchronous binary parent*/){
				isEscape = true;
			}
			break;
		case 96: // backtick
			if (isEscape == false){
				isQuery = true;
				isAddress = false;
				break;
			}
			isEscape = false;
			break;
		case 127: // delete
			if (isEscape == false){
				isDelete = true;
				isAddress = false;
				break;
			}
			isEscape = false;
			break;
		case '0': isEscape = false; isDelimiter = false; break;
		case '1': isEscape = false; isDelimiter = false; break;
		case '2': isEscape = false; isDelimiter = false; break;
		case '3': isEscape = false; isDelimiter = false; break;
		case '4': isEscape = false; isDelimiter = false; break;
		case '5': isEscape = false; isDelimiter = false; break;
		case '6': isEscape = false; isDelimiter = false; break;
		case '7': isEscape = false; isDelimiter = false; break;
		case '8': isEscape = false; isDelimiter = false; break;
		case '9': isEscape = false; isDelimiter = false; break;
		case '-': isEscape = false; isDelimiter = false; if (HighlightLength_l>0) isAddressDelimiter = true; break;
		case '+': isEscape = false; isDelimiter = false; isAddress = false; break;
		case '.': isEscape = false; isDelimiter = false; isAddress = false; break;
		case 'E': isEscape = false; isDelimiter = false; isAddress = false; break;
		case 'e': isEscape = false; isDelimiter = false; isAddress = false; break;
		case 'X': isEscape = false; isDelimiter = false; isAddress = false; break;
		case 'x': isEscape = false; isDelimiter = false; isAddress = false; break;
		case 'A': isEscape = false; isDelimiter = false; isAddress = false; break;
		case 'a': isEscape = false; isDelimiter = false; isAddress = false; break;
		case 'B': isEscape = false; isDelimiter = false; isAddress = false; break;
		case 'b': isEscape = false; isDelimiter = false; isAddress = false; break;
		case 'C': isEscape = false; isDelimiter = false; isAddress = false; break;
		case 'c': isEscape = false; isDelimiter = false; isAddress = false; break;
		case 'D': isEscape = false; isDelimiter = false; isAddress = false; break;
		case 'd': isEscape = false; isDelimiter = false; isAddress = false; break;
		case 'F': isEscape = false; isDelimiter = false; isAddress = false; break;
		case 'f': isEscape = false; isDelimiter = false; isAddress = false; break;
		default: isEscape = false; isNumber = false; isAddress = false; break;
		}
		// finish when item end has been reached
		if (isNextDelimiter || isLineEnd){
			break;
		}
		// step forward
		ActiveStart_l++;
		ActiveLength_l--;
		HighlightLength_l++;

		// adjust for option/comment
		if ((isOption || isComment) && false == isProcessed){
			isProcessed = true;
			HighlightStart_l++;
			HighlightLength_l--;
		}
	}

	// check for misplaced query operator
	if (isQuery && 1 == HighlightLength_l && false == isNextDelimiter){
		_TypeX = TYPEFTL_QUERY;
		_LineX.setActive(ActiveStart_l, ActiveLength_l);
		_LineX.setHighlight(HighlightStart_l, HighlightLength_l);
		return false;		// stand-alone query operator is not allowed
	}

	// determine appropriate data type
	if (isIdentifier && 1 == HighlightLength_l){
		// the string contains a single '@' sign which indicates the start of a synchronous 
		// write operation with a fixed parent collection
		_TypeX = TYPEFTL_FIXED_PARENTS;
	}
	else
		// determine appropriate data type
	if (isIdentifier){
		// the string contains a non-escaped '@' sign which makes it a FTLight identifier
		_TypeX = TYPEFTL_IDENTIFIER;
	}
	else
	if (isChecksum && isLineEnd){
		// a binary field at the end of line preceded by an "=" sign represents the line's checksum
		_TypeX = TYPEFTL_CHECKSUM;
	}
	else
	if (isQuery && 1 == HighlightLength_l){
		// a single backtick (no other characters) represents the query operator
		_TypeX = TYPEFTL_QUERY;
	}
	else
	if (isOption){
		// two colons in the beginning make this item an option
		_TypeX = TYPEFTL_OPTION;
	}
	else
	if (isComment){
		// three colons in the begining make this item a comment
		_TypeX = TYPEFTL_COMMENT;
	}
	else
	if (false == isDetermined && isText && false == isNumber){
		// characters other than (0123456789+-.eExXaAbBcCdDfF) at start of line 
		// without a delimiter sugggest this item to be text or binary (and not a number)
		_TypeX = TYPEFTL_TEXT_BINX;

		// evaluate parent node for binary type
		


	}
	else
	if (isText && (false == isNumber || 0 == HighlightLength_l)){
		// characters other than (0123456789+-.eExXaAbBcCdDfF) after a 
		// text delimiter (,:) make this item a text (and not a number)
		_TypeX = TYPEFTL_TEXT;
	}
	else
	// 2015-04-25/EKantz: a single number will no longer be a Local, however
	//                    a Local has to have a delimiter, e.g. '0-' or 0-0'
	if (false == isDetermined && isAddress && isAddressDelimiter){
		// an address may contain only numbers (0123456789) and it has to have
		// address delimiter(s) if the length is bigger than 1, however, at the
		// start of a line without a delimiter it could also be binary.
		_TypeX = TYPEFTL_Local_BINX;
	}
	else
	if (isAddress && isAddressDelimiter){
		// an address may contain only numbers (0123456789) and it has to have
		// address delimiter(s) if the length is bigger than 1
		_TypeX = TYPEFTL_Local;
	}
	else
	if (false == isDetermined && isNumber){
		// the set of characters (0123456789+-.eExXaAbBcCdDfF) after a 
		// text delimiter (,:) suggest that this item could be a number, however, 
		// at the start of a line without a delimiter it could also be binary.
		_TypeX = TYPEFTL_NUMBER_BINX;
	}
	else
	if (isNumber){
		// the set of characters (0123456789+-.eExXaAbBcCdDfF) after a 
		// text delimiter (,:) suggest that this item could be a number
		_TypeX = TYPEFTL_NUMBER;
	}
	else{
		// after sorting out all other data types this item is TYPEFTL_BINX
		_TypeX = TYPEFTL_BINX;
	}

	// check for special binary types
	if ((TYPEFTL_BINX == _TypeX) || (TYPEFTL_TEXT_BINX == _TypeX)){
		FTLight_DATA_TYPE_IDENTIFIER FTLightType;
		getFTLightType(_LineX, FTLightType);

		if (FTLight_OPEN == FTLightType){
			_TypeX = TYPEFTL_FTLightOPEN;
		}
		else
		if (FTLight_WRAP == FTLightType){
			_TypeX = TYPEFTL_FTLightWRAP;
		}
		else
		if (BINMCL == FTLightType){
			_TypeX = TYPEFTL_BINMCL;
		}
		else
		if (BINX_BINARY == FTLightType){
			_TypeX = TYPEFTL_BINXBINARY;
		}
		else
		if (BINX_STRING == FTLightType){
			_TypeX = TYPEFTL_BINXSTRING;
		}
		else
		if (BINX_VALUE == FTLightType){
			_TypeX = TYPEFTL_BINXVALUE;
		}
		else
		if (BINX_TIME == FTLightType){
			_TypeX = TYPEFTL_BINXTIME;
		}
		else
		if (BINX_TOKEN == FTLightType){
			_TypeX = TYPEFTL_CMXTOKEN;
		}
		else
		if (BINX_LINK == FTLightType){
			_TypeX = TYPEFTL_CMXLINK;
		}
		else
		if (TYPEFTL_BINDIF == FTLightType){
			_TypeX = TYPEFTL_BINDIF;
		}
	}

	// return descriptor values
	_LineX.setActive(ActiveStart_l, ActiveLength_l);
	_LineX.setHighlight(HighlightStart_l, HighlightLength_l);

	// generate ItemX
	ItemX.assignSubString(HighlightStart_l + HighlightLength_l - 1, &_LineX, HighlightStart_l);

	return true;
}

//----------------------------------------------------------------------------
// CmStringFTLroot class.
//----------------------------------------------------------------------------
//
CmStringFTLroot::CmStringFTLroot()
{
	// make this a empty root node
	createRoot();
}
CmStringFTLroot::~CmStringFTLroot()
{

}

