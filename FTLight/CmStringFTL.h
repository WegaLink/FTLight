//////////////////////////////////////////////////////////////////////////////
//
// CmStringFTL.h: Declaration of the CmStringFTL class
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

#ifndef CmStringFTLH
#define CmStringFTLH

#include "Services/Basic/CmString.h"
#include "Services/Basic/CmTime.h"

namespace Cosmos
{

/** FTLight data type identifiers (little endian)
*	The data type identifiers have been defined as symbols
*   from the 216-radix based BinX conding. The identifiers
*   are unused symbols and start from highest 216^4-1 value.
*
*   NOTE: FTLight types will be represented little endian!
*/
#define FTLight_BINX_MAX	(uint32)0x81BF0FFF	// = 216^4-1
#define FTLight_BINX_MIN	(uint32)0x80000000	// = 2^31
enum FTLight_DATA_TYPE_IDENTIFIER {
	FTLight_OPEN	= FTLight_BINX_MAX - 0,
	FTLight_WRAP	= FTLight_BINX_MAX - 1,
	BINMCL			= FTLight_BINX_MAX - 2,
	BINX_BINARY		= FTLight_BINX_MAX - 3,
	BINX_STRING		= FTLight_BINX_MAX - 4,
	BINX_VALUE		= FTLight_BINX_MAX - 5,
	BINX_TIME		= FTLight_BINX_MAX - 6,
	BINX_TOKEN		= FTLight_BINX_MAX - 7,
	BINX_LINK		= FTLight_BINX_MAX - 8,
	BINX_DIF		= FTLight_BINX_MAX - 9,

	// unknown type
	FTLight_NONE = FTLight_BINX_MIN
};

/** CmTypeFTL.
*   The FTLight specification uses special characters, data type identifiers
*   along with rules to conclude about the type of data that comes as items
*   in a FTLight line. The following definitions refer to those data types.
*/
enum CmTypeFTL {
	TYPEFTL_NONE			= 0,
	TYPEFTL_OPERATOR		= 1,
	TYPEFTL_LOCATION		= 2,
	TYPEFTL_IDENTIFIER		= 3,
	TYPEFTL_QUERY			= 4,
	TYPEFTL_TEXT			= 5,
	TYPEFTL_TEXT_BINX		= 6,
	TYPEFTL_NUMBER			= 7,
	TYPEFTL_NUMBER_BINX		= 8,
	TYPEFTL_Local			= 9,
	TYPEFTL_Local_BINX		= 10,
	TYPEFTL_FIXED_PARENTS	= 11,
	TYPEFTL_CHECKSUM		= 12,
	TYPEFTL_OPTION			= 13,
	TYPEFTL_DEFAULT			= 14,
	TYPEFTL_COMMENT			= 15,
	TYPEFTL_BINX			= 16,
	TYPEFTL_CONTROLX		= 17,
	TYPEFTL_FTLightOPEN		= 18,
	TYPEFTL_FTLightWRAP		= 19,
	TYPEFTL_BINMCL			= 20,
	TYPEFTL_BINXBINARY		= 21,
	TYPEFTL_BINXSTRING		= 22,
	TYPEFTL_BINXVALUE		= 23,
	TYPEFTL_BINXTIME		= 24,
	TYPEFTL_CMXTOKEN		= 25,
	TYPEFTL_CMXLINK			= 26,
	TYPEFTL_BINDIF			= 27,

};

/** CmDataFormat
*   When converting to and from FTLight, the following data formats apply.
*/
enum CmDataFormat {
	DATAFORMAT_NONE		= 0,
	DATAFORMAT_FTLight	= 1,
	DATAFORMAT_STRING	= 2,
	DATAFORMAT_BINARY	= 3,
	DATAFORMAT_UINT64	= 4,
	DATAFORMAT_INT64	= 5,
	DATAFORMAT_DOUBLE	= 6,
	DATAFORMAT_UINT32	= 7,
	DATAFORMAT_INT32	= 8,
	DATAFORMAT_FLOAT	= 9,
	DATAFORMAT_UINT16	= 10,
	DATAFORMAT_INT16	= 11,
	DATAFORMAT_UINT8	= 12,
	DATAFORMAT_INT8		= 13,
	DATAFORMAT_BOOL		= 14,
	// vector elements
	DATAFORMAT_MATRIX	= 15,

};

/** CmUnit.
*  Physical units can be involved according to the following enumeration.
*/
enum CmPhysicalUnit {
	UNIT_NONE					= 0,		// without unit
	UNIT_TIME					= 1,		// second, [s]
	UNIT_LENGTH					= 2,		// meter, [m]
	UNIT_MASS					= 3,		// kilogram, [kg]
	UNIT_CURRENT				= 4,		// ampere, [A]
	UNIT_TEMPERATURE			= 5,		// kelvin, [K]
	UNIT_QUANTITY_OF_MATERIAL	= 6,		// mol, [mol]
	UNIT_LIGHT_INTENSITY		= 7,		// candela, [cd]
	UNIT_PLANE_ANGLE			= 8,		// radiant, [rad]
	UNIT_SOLID_ANGLE			= 9,		// steradiant, [sr]
	UNIT_FREQUENCY				= 10,		// hertz, [Hz]
	UNIT_FORCE					= 11,		// newton, [N]
	UNIT_PRESSURE				= 12,		// pascal, [Pa]
	UNIT_ENERGY_WORK_HEAT		= 13,		// joule, [J]
	UNIT_POWER					= 14,		// watt, [W]
	UNIT_ELECTRICAL_CHARGE		= 15,		// coulomb, [C]
	UNIT_VOLTAGE				= 16,		// volt, [V]
	UNIT_CAPACITANCE			= 17,		// farad, [F]
	UNIT_ELECTRICAL_RESISTANCE	= 18,		// ohm, [ohm]
	UNIT_SUSCEPTANCE			= 19,		// siemens, [S]
	UNIT_MAGNETIC_FLUX			= 20,		// weber, [Wb]
	UNIT_MAGNETIC_FLUX_DENSITY	= 21,		// tesla, [T]
	UNIT_INDUCTANCE				= 22,		// henry, [H]
	UNIT_TEMPERATURE_CELSIUS	= 23,		// degree celsius, [°C]
	UNIT_LUMINOUS_FLUX			= 24,		// lumen, [lm]
	UNIT_LUMINANCE_INTENSITY	= 25,		// lux, [lx]
	UNIT_RADIOACTIVITY			= 26,		// becquerel, [Bq]
	UNIT_ABSORBED_DOSE			= 27,		// gray, [Gy]
	UNIT_DOSE_EQUIVALENT		= 28,		// sievert, [Sv]
	UNIT_CATALYTIC_ACTIVITY		= 29,		// katal, [kat]

};

/** CmLogLevel.
*  The logging will be controlled by the following log levels
*/
enum CmLogLevel {
	CMLOG_None			= 0,		// logging disabled
	CMLOG_Error			= 1,		// errors (=default)
	CMLOG_TestMode		= 2,		// communication test
	CMLOG_QuickView		= 3,		// quick view
	CMLOG_Info			= 4,		// info
	CMLOG_Control		= 5,		// control/response
	CMLOG_Request		= 6,		// request/dataset
	CMLOG_Process		= 7,		// processing data
	CMLOG_Network		= 8,		// network maintenance
	CMLOG_Connection	= 9,		// connection details
	CMLOG_PlugNodes		= 10,		// PlugNodes operation
	CMLOG_Resource		= 11,		// resource management
};

/** CmLockID.
*  The following LockIDs will be used when protecting parallel program execution paths.
*/
#define LOCKID_RANGE	1000
enum CmLockID {
	// CmLockID
	CMLOCKID_NONE																						= 0,

	// CmStringFTL (0)
	CMLOCKID_StringFTL_LogAccess														= CMLOCKID_NONE,

	// CmServiceAccess (1000)
	CMLOCKID_CmServiceAccess																= CMLOCKID_StringFTL_LogAccess + LOCKID_RANGE,
	CMLOCKID_CmServiceAccess_switchServiceAccess				= 1 + CMLOCKID_CmServiceAccess,
	CMLOCKID_CmServiceAccess_shutdownConnections				= 2 + CMLOCKID_CmServiceAccess,
	CMLOCKID_CmServiceAccess_deleteIdleConnections				= 3 + CMLOCKID_CmServiceAccess,
	CMLOCKID_CmServiceAccess_deleteConnections					= 4 + CMLOCKID_CmServiceAccess,

	// SERVICE_CmGateway (2000)
	CMLOCKID_SERVICE_CmGateway															= CMLOCKID_CmServiceAccess + LOCKID_RANGE,
	CMLOCKID_SERVICE_CmGateway_Provider							= 1 + CMLOCKID_SERVICE_CmGateway,
	CMLOCKID_SERVICE_CmGateway_addToGatewayConnectionList		= 2 + CMLOCKID_SERVICE_CmGateway,
	CMLOCKID_SERVICE_CmGateway_cleanupGatewayConnections		= 3 + CMLOCKID_SERVICE_CmGateway,
	CMLOCKID_SERVICE_CmGateway_sendInfoToGateway				= 4 + CMLOCKID_SERVICE_CmGateway,

	// CmApplication (3000)
	CMLOCKID_CmApplication																	= CMLOCKID_SERVICE_CmGateway + LOCKID_RANGE,
	// CmModule (4000)
	CMLOCKID_CmModule																				= CMLOCKID_CmApplication + LOCKID_RANGE,
	CMLOCKID_CmModule2																			= CMLOCKID_CmModule + LOCKID_RANGE,
	CMLOCKID_CmModule3																			= CMLOCKID_CmModule2 + LOCKID_RANGE,
	// CmDevice (7000)
	CMLOCKID_CmDevice																				= CMLOCKID_CmModule3 + LOCKID_RANGE,
	CMLOCKID_CmDevice2																			= CMLOCKID_CmDevice + LOCKID_RANGE,
	CMLOCKID_CmDevice3																			= CMLOCKID_CmDevice2 + LOCKID_RANGE,

};

// radix for BinX conversion
#define RADIX_216 216

// forward declaration
class CmStringFTLroot;

//------------------------------------------------------------------------------
// CmParallelFTL class
//------------------------------------------------------------------------------
//

/** CmParallelFTL.
*  A function will run in parallel to other functionality in a program by
*  letting it run in a separate thread.
*
* see: https://stackoverflow.com/questions/1372967/how-do-you-use-createthread-for-functions-which-are-class-members
*/
class CmParallelFTL
{
public:
	/** CmParallelFTL.
	*  A member function will run a specified number of times in background.
	*/
	CmParallelFTL();
	virtual ~CmParallelFTL();

public:
	/** startParallel.
	*  A thread will be started which calls a static thread function.
	*/
	void startParallel(int64 _LoopCount = 0);

protected:
	/** StaticThreadStart.
	*  A static function which will be started in a thread.
	*/
	static DWORD WINAPI StaticThreadStart(void* _Param);

protected:
	/** runParallel.
	*  This function will be executed in parallel to other program functionality.
	*/
	virtual bool runParallel();

public:
	/** isRunningParallel.
	*  Returns true if a thread was started and is still running.
	*/
	bool isRunningParallel();

public:
	/** stopParallel.
	*  Stop parallel execution of this class' program functionality gracefully.
	*  After the timeout has elapsed the thread will be unconditionally removed.
	*/
	bool stopParallel(int32 _Timeout_ms = 400, int32 _WaitStep = 20);

public:
	/** isThreadGoingDown.
	*	Return true when the thread has been scheduled for shutdown
	*/
	bool isThreadDown();

public:
	/** enter/leaveSerialize/isLocked.
	*  A critical section will be entered as soon as it is not used by others,
	*  respectively it will be left and thus be enabled again for access by others.
	*/
	virtual bool enterSerialize(int32 _LockID = 0);
	virtual bool leaveSerialize();
	bool trySerialize(int32 _LockID = 0);
	bool isLocked();

public:
	/** setLogLevel.
	*  The global MaxLogLevel will be set.
	*/
	static void setLogLevel(CmLogLevel _LogLevel);

public:
	/** log.
	*   A message will be added to a Logger (CmStringFTL) if the LogLevel is less
	*   or equal to the current maximal LogLevel .
	*   An  optional Context can be used for a context-sensitive logging.
	*/
	bool log(const CmString& _Message, CmLogLevel _LogLevel = CMLOG_Error, const CmString* _Context = NULL);

#ifdef CMNOLOG
#define LOG14(a,b,c,d,e,f,g,h,i,j,k,l,m,n,msg,LogLevel)
#define LOG13(a,b,c,d,e,f,g,h,i,j,k,l,m,msg,LogLevel)
#define LOG12(a,b,c,d,e,f,g,h,i,j,k,l,msg,LogLevel)
#define LOG11(a,b,c,d,e,f,g,h,i,j,k,msg,LogLevel)
#define LOG10(a,b,c,d,e,f,g,h,i,j,msg,LogLevel)
#define LOG9(a,b,c,d,e,f,g,h,i,msg,LogLevel)
#define LOG8(a,b,c,d,e,f,g,h,msg,LogLevel)
#define LOG7(a,b,c,d,e,f,g,msg,LogLevel)
#define LOG6(a,b,c,d,e,f,msg,LogLevel)
#define LOG5(a,b,c,d,e,msg,LogLevel)
#define LOG4(a,b,c,d,msg,LogLevel)
#define LOG3(a,b,c,msg,LogLevel)
#define LOG2(a,b,msg,LogLevel)
#define LOG1(a,msg,LogLevel)
#else
#define LOG14(a,b,c,d,e,f,g,h,i,j,k,l,m,n,msg,LogLevel) 	CmString msg=a; msg+=b; msg+=c; msg+=d; msg+=e; msg+=f; msg+=g; msg+=h; msg+=i; msg+=j; msg+=k; msg+=l; msg+=m; msg+=n; log(msg, LogLevel);
#define LOG13(a,b,c,d,e,f,g,h,i,j,k,l,m,msg,LogLevel) 	CmString msg=a; msg+=b; msg+=c; msg+=d; msg+=e; msg+=f; msg+=g; msg+=h; msg+=i; msg+=j; msg+=k; msg+=l; msg+=m; log(msg, LogLevel);
#define LOG12(a,b,c,d,e,f,g,h,i,j,k,l,msg,LogLevel) 	CmString msg=a; msg+=b; msg+=c; msg+=d; msg+=e; msg+=f; msg+=g; msg+=h; msg+=i; msg+=j; msg+=k; msg+=l; log(msg, LogLevel);
#define LOG11(a,b,c,d,e,f,g,h,i,j,k,msg,LogLevel) 	CmString msg=a; msg+=b; msg+=c; msg+=d; msg+=e; msg+=f; msg+=g; msg+=h; msg+=i; msg+=j; msg+=k; log(msg, LogLevel);
#define LOG10(a,b,c,d,e,f,g,h,i,j,msg,LogLevel) 	CmString msg=a; msg+=b; msg+=c; msg+=d; msg+=e; msg+=f; msg+=g; msg+=h; msg+=i; msg+=j; log(msg, LogLevel);
#define LOG9(a,b,c,d,e,f,g,h,i,msg,LogLevel) 	CmString msg=a; msg+=b; msg+=c; msg+=d; msg+=e; msg+=f; msg+=g; msg+=h; msg+=i; log(msg, LogLevel);
#define LOG8(a,b,c,d,e,f,g,h,msg,LogLevel) 	CmString msg=a; msg+=b; msg+=c; msg+=d; msg+=e; msg+=f; msg+=g; msg+=h; log(msg, LogLevel);
#define LOG7(a,b,c,d,e,f,g,msg,LogLevel) 	CmString msg=a; msg+=b; msg+=c; msg+=d; msg+=e; msg+=f; msg+=g; log(msg, LogLevel);
#define LOG6(a,b,c,d,e,f,msg,LogLevel) 	CmString msg=a; msg+=b; msg+=c; msg+=d; msg+=e; msg+=f; log(msg, LogLevel);
#define LOG5(a,b,c,d,e,msg,LogLevel) 	CmString msg=a; msg+=b; msg+=c; msg+=d; msg+=e; log(msg, LogLevel);
#define LOG4(a,b,c,d,msg,LogLevel) 	CmString msg=a; msg+=b; msg+=c; msg+=d; log(msg, LogLevel);
#define LOG3(a,b,c,msg,LogLevel) 	CmString msg=a; msg+=b; msg+=c; log(msg, LogLevel);
#define LOG2(a,b,msg,LogLevel) 	CmString msg=a; msg+=b; log(msg, LogLevel);
#define LOG1(a,msg,LogLevel) 	CmString msg=a; log(msg, LogLevel);
#endif

public:
	/** writeLog.
	*  The content of the Logger (CmStringFTL) will be written to specified file.
	*  The UURI is assigned as a top level to the FTLight-style log file.
	*/
	bool writeLog(CmString _LogFile, CmString _UURI, CmString _FilePath = "log\\");

	//--------workspace-----------------------------------------------------------

protected:
	// Thread
	DWORD ThreadID;
	int64 LoopCount;
	bool isThreadShutdown;
	bool isThreadFinished;
	bool isThreadPending;
public:
	HANDLE ThreadHandle;

private:
	// serialize
	CRITICAL_SECTION	CriticalSection;
	int32 LockID;
	int32 LastLockID;
	CmTimestamp Timestamp;
	int32 LastLockTime_ms;
};

//------------------------------------------------------------------------------
// CmStringFTL class
//------------------------------------------------------------------------------
//
/** CmStringFTL.
*   The class CmStringFTL represents a single node in a FTLight hierarchy. On the
*   one hand it provides for converting all data types to and from FTLight data
*   formats. On the other hand, it forms the structure of a FTLight hierarchy 
*   by connecting 'child' and 'sibling' nodes.
*/
class CmStringFTL : public CmString, public CmParallelFTL
{
public:
	CmStringFTL(CmStringFTL* _Parent = NULL);
	CmStringFTL(const int8* _Text, CmStringFTL* _Parent = NULL);
	~CmStringFTL();

public:
	/** unit test for CmStringFTL */
	bool testCmStringFTL();

public:
	/** build/clear conversion table */
	bool isConversionTable();
	bool clearConversionTable();

public:
	/** cleanup FTLight resources */
	static void shutDownFTLight();

public:
	/** clear FTLight hierarchy */
	bool clearFTLightHierarchy();

public:
	/** setTextX.
	*  A string will be stored in TextX and the data format will be adjusted to DATAFORMAT_NONE
	*/
	bool setTextX(const int8* _Text);
	bool setTextX(CmString& _Text);

public:
	/** getTextX/LengthX
	*   A FTLight encoded string / its length will be returned.
	*/
	const uint8 *getTextX();
	int64 getLengthX();

public:
	/** encodeTextX.
	*   A text string will be converted into a FTLight compliant string.
	*   Basically, some defined special characters are escaped by '\'.
	*/
	CmStringFTL& encodeTextX(const int8* _Text =NULL);
	CmStringFTL& encodeTextX(CmString& _Text);

public:
	/** decodeTextX.
	*   A FTLight encoded string will be returned to original string.
	*   Basically, escaping of some defined special characters by a
	*   backslash will be removed.
	*   If Text==NULL then the content of the CmString will be decoded.
	*/
	void decodeTextX(const int8* _Text =NULL, size_t _Length =0);
	void decodeTextX(CmString& _Text);

public:
	/** encodeBinX.
	*   A binary field will be converted into a FTLight compliant string.
	*   Basically, it will be encoded by symbols of radix 216.
	*   If Data==NULL then the content of the CmString will be encoded.
	*   The binary input data will be preserved in the base CmString class.
	*   The encoded BinX string will be available in TextX.
	*/
	void encodeBinX(const uint8* _Data = NULL, size_t _Length = 0);
	void encodeBinX(CmString& _Data);

public:
	/** decodeBinX.
	*   A FTLight encoded string will be returned to original binary field.
	*   Basically, it will be restored from symbols of radix 216.
	*   If Data==NULL then the content of the CmString will be decoded.
	*   The BinX input string will be preserved in TextX.
	*   The decoded binary data will be in the base CmString class.
	*/
	void decodeBinX(const uint8* _Data = NULL, size_t _Length = 0);
	void decodeBinX(CmString& _Data);

public:
	/** encodeNumX.
	*		A 64-bit binary field will be encoded into a BinX string. Usually,
	*   any data type from a single bit logical value or a byte upto 64-bit 
	*   data types will first be casted/copied into an unsigned 64-bit field.
	*   Unused bits should be filled with 0. The resulting BinX string will 
	*   encompass the minimal byte number that is needed to represent the
	*   valid bits without leading zeros.
	*/
	void encodeNumX(uint64 _Value); 
	void encodeNumX(int64 _Value);
	void encodeNumX(double _Value);
	void encodeNumX(uint32 _Value);
	void encodeNumX(int32 _Value);
	void encodeNumX(float _Value);
	void encodeNumX(uint16 _Value);
	void encodeNumX(int16 _Value);
	void encodeNumX(uint8 _Value);
	void encodeNumX(int8 _Value);
	void encodeNumX(bool _Value);

public:
	/** decodeNumX.
	*   A BinX string will be converted back into a 64-bit binary field. 
	*   Dependend on the meaning of the value, the 64-bit field will usually
	*   be casted/converted into the target data type which may contain less
	*   then 64 bits.
	*/
	uint64 decodeNumX2uint64(uint64* _Value = NULL);
	int64 decodeNumX2int64(int64* _Value = NULL);
	double decodeNumX2double(double* _Value = NULL);
	uint32 decodeNumX2uint32(uint32* _Value = NULL);
	int32 decodeNumX2int32(int32* _Value = NULL);
	float decodeNumX2float(float* _Value = NULL);
	uint16 decodeNumX2uint16(uint16* _Value = NULL);
	int16 decodeNumX2int16(int16* _Value = NULL);
	uint8 decodeNumX2uint8(uint8* _Value = NULL);
	int8 decodeNumX2int8(int8* _Value = NULL);
	bool decodeNumX2bool(bool* _Value = NULL);

public:
	/** decodeNum.
	*   A Num (number) string will be converted into a specified data format.
	*/
	bool decodeNum(CmDataFormat _DataFormat);

public:
	/** isInteger. A string will be evaluated for being an integer (+-0..9) */
	bool isInteger();

public:
	/** setParent. */
	void setParent(CmStringFTL* _Parent);

public:
	/** getParent/Sibling/Child/TypeX
	*		A pointer to requested node/type will be returned.
	*/
	CmStringFTL * getParent();
	CmStringFTL * getSibling();
	CmStringFTL * getChild();
	CmTypeFTL getTypeX();
	bool isBinary(bool _isParentBinary = false);

public:
	/** addChild.
	*   A new CmStringFTL will be appended to child position. If the child position
	*   is already occupied then a new child will be inserted into a sibling list.
	*/
	bool addChild(CmStringFTL** _Child);

public:
	/** findChild.
	*   A child with a specified content will be returned if one could be found.
	*/
	bool findChild(CmStringFTL** _Child, CmString& _Content, CmTypeFTL _TypeX = TYPEFTL_TEXT);

public:
	/** receiveLineX.
	*   A subsequent FTLight line will be inserted into current hierarchy of CmStringFTL nodes.
	*/
	bool receiveLineX(CmString& _FTLightline);

public:
	/** processStringFTL.
	*   A string with multiple LineX items will be inserted into hierarchy of CmStringFTL nodes.
	*/
	bool processStringFTL(const int8* _StringX);
	bool processStringFTL(CmString& _StringX);

public:
	/** readFileX.
	*   A file will be read and converted into a hierarchy of CmStringFTL nodes
	*/
	bool readFileX(const int8* _FileX);

public:
	/** serializeStringFTL
	*  A hierarchy of CmStringFTL nodes will be converted to a string.
	*/
	bool serializeStringFTL(CmString& _StringFTL);

public:
	/** convertFTLight2XML.
	*   A FTLight encoded file will be analyzed and converted to a XML file.
	*/
	bool convertFTL2XML(const CmString& _FileFTL, const CmString& _FileXML);
	bool convertFTL2XML(const int8* _FileFTL, const int8* _FileXML);

public:
	/** getNode.
	*   The node at a specified position will be returned. If there is no node at that position
	*   then one will be created. In that case there is no content available (empty).
	*/
	const CmString& getNode(const CmString& _Address);

public:
	/** addItemX.
	*   A next binary item will be added to a FTLight line collection
	*/
	bool addItemX(const CmString& _Val);
	bool addItemX(void *_Val);
	bool addItemX(uint64 _Val);
	bool addItemX(int64 _Val);
	bool addItemX(double _Val);
	bool addItemX(uint32 _Val);
	bool addItemX(int32 _Val);
	bool addItemX(float _Val);
	bool addItemX(uint16 _Val);
	bool addItemX(int16 _Val);
	bool addItemX(uint8 _Val);
	bool addItemX(int8 _Val);
	bool addItemX(bool _Val);

public:
	/** getDataFormat.
	*   Current format of the data in the base CmString buffer will be returned
	*/
	CmDataFormat getDataFormat();
	CmDataFormat getDataFormat(CmString& _DataFormat);

public:
	/** insert/set/getValueX.
	*   A CmStringFTL will be inserted/copied to/from specified address in a FTLight hierarchy.
	*/
	bool insertValueX(const CmString& _Address, const CmString& _ValueX);
	bool setValueX(const CmString& _Address, const CmString& _ValueX);
	bool getValueX(const CmString& _Address, CmStringFTL& _ValueX, CmString _StartAddr = CmString());

public:
	/** getValue.
	*   A value will be restored from ItemX at specified address in a FTLight hierarchy.
	*/
	bool getValue(const char *_Address, CmDataFormat _DataFormat, CmStringFTL **_Val);
	bool getValue(const char *_Address, CmString& _Val);
	bool getValue(const char *_Address, void*& _Val);
	bool getValue(const char *_Address, uint64& _Val);
	bool getValue(const char *_Address, int64& _Val);
	bool getValue(const char *_Address, double& _Val);
	bool getValue(const char *_Address, uint32& _Val);
	bool getValue(const char *_Address, int32& _Val);
	bool getValue(const char *_Address, float& _Val);
	bool getValue(const char *_Address, uint16& _Val);
	bool getValue(const char *_Address, int16& _Val);
	bool getValue(const char *_Address, uint8& _Val);
	bool getValue(const char *_Address, int8& _Val);
	bool getValue(const char *_Address, bool& _Val);

	bool getValue(const CmString& _Addr, CmDataFormat _DataFormat, CmStringFTL **_Val, CmString _StartAddr = CmString());
	bool getValue(const CmString& _Addr, CmString& _Val, CmString _StartAddr = CmString());
	bool getValue(const CmString& _Addr, void*& _Val, CmString _StartAddr = CmString());
	bool getValue(const CmString& _Addr, uint64& _Val, CmString _StartAddr = CmString());
	bool getValue(const CmString& _Addr, int64& _Val, CmString _StartAddr = CmString());
	bool getValue(const CmString& _Addr, double& _Val, CmString _StartAddr = CmString());
	bool getValue(const CmString& _Addr, uint32& _Val, CmString _StartAddr = CmString());
	bool getValue(const CmString& _Addr, int32& _Val, CmString _StartAddr = CmString());
	bool getValue(const CmString& _Addr, float& _Val, CmString _StartAddr = CmString());
	bool getValue(const CmString& _Addr, uint16& _Val, CmString _StartAddr = CmString());
	bool getValue(const CmString& _Addr, int16& _Val, CmString _StartAddr = CmString());
	bool getValue(const CmString& _Addr, uint8& _Val, CmString _StartAddr = CmString());
	bool getValue(const CmString& _Addr, int8& _Val, CmString _StartAddr = CmString());
	bool getValue(const CmString& _Addr, bool& _Val, CmString _StartAddr = CmString());

public:
	/** incrementSequenceNumber()
	*		The sequence number for volatile synchronous writing will be incremented and returned.
	*/
	int64 incrementSequenceNumber();

public:
	/** getConfigPath.
	*   A configuration path will be determined and created if it does not exist yet.
	*/
	bool getConfigPath(CmString _AppData, CmString _AppCompany, CmString _AppProgram, CmString _AppVersion);

public:
	/** getIdentifier.
	*  First ItemX will be read from an Info StringFTL and its type will
	*  be checked against the identifier type (=UURI).
	*/
	static bool getIdentifier(CmString& _Info, CmUURI& _UURI);

public:
	/** encode/decodeValueFormat.
	*  A FTLIght format string for a physical unit will be encoded.
	*/
	bool encodeValueFormat(CmString& _ValueFormat, CmPhysicalUnit _Unit, uint64 _Factor = 1, uint64 _Divisor = 1, uint64 _Base = 1, int64 _Exponent = 0);
	bool decodeValueFormat(const CmString& _ValueFormat, CmPhysicalUnit& _Unit, double _Value);

public:
	/** encode/decodeTimeFormat.
	* A FTLight format string for a time unit will be encoded.
	*/
	bool encodeTimeFormat(CmString& _TimeFormat, uint64 _Factor = 1, uint64 _Divisor = 1, uint64 _Base = 1, int64 _Exponent = 0);
	bool decodeTimeFormat(const CmString& _TimeFormat, double _Time);

public:
	/** BinX2bin.
	*   A BinX string will be converted back into original binary field. If fTail==true
	*   then incomplete 4-byte quadrupel will be converted as well.
	*/
	static int32 BinX2bin(uint8* _pBuf, int32 _nBufSize, CmString& _mBinX, bool _fTail = false);

public:
	/** for CmFileFTL usage */
	static bool isDataTypeIdentifier(CmString& _FTLight, FTLight_DATA_TYPE_IDENTIFIER _eDataType);

public:
	// Convert numbers to and from a BinX string
	CmString* num2BinX(CmString* _pBinX, uint64 _u64Num);
	CmString* num2BinX(CmString* _pBinX, uint64* _p64Num);
	static uint64 BinX2num(CmString& _mBinX);

public:
	/** Convert binary data to and from BinDIF format */
	bool val2BinDIF(CmVector<uint8>& _BinDIF, CmVector<int64>& _Values, int32 _insertAbsoluteValue = 62);
	bool BinDIF2val(CmVector<uint8>& _BinDIF, CmVector<int64>& _Values);
	bool asVector(CmVector<int64>& _Data);
	int64 getDigits216(int64& _Value);

//------ private --------------------------------------------------------------------------

private:
	// Convert symbols 0..215 into legal BinX characters 
	uint8 Symbol2BinX(uint8 _Symbol);
	uint8 BinX2Symbol(uint8 _BinX);

private:
	/** convert type identifiers to and from BinX format */
	bool setFTLightType(CmString& _BinX, FTLight_DATA_TYPE_IDENTIFIER _FTLightType);
	bool getFTLightType(CmString& _BinX, FTLight_DATA_TYPE_IDENTIFIER& _FTLightType);

private:
	/** getLengthBinX2bin.
	*   Resulting binary length from a BinX2bin conversion will be reported. If 
	*   fTail==true then incomplete 4-byte quadrupel will be counted as well.
	*/
	static size_t getLengthBinX2bin(size_t _LengthX, bool _fTail = false);

private:
	/** bin2BinX.
	*   A binary field will be converted into a BinX string. If nDigits==0 then the
	*   BinX length will be determined from the length of the binary field.
	*/
	CmString* bin2BinX(CmString* _pBinX, const uint8* _pBuf, int32 _nBinLen, int32 _nDigits = 0);

private:
	/** Convert binary data to and from a BinMCL string */
	bool bin2BinMCL(int _n, uint16 *_inp, uint16 *_out);
	bool BinMCL2bin(int _n, uint16 *_inp, uint16 *_out);

private:
	/** getLastChild.
	*   The child that was most recently inserted will be returned.
	*/
	bool getLastChild(CmStringFTL** _LastChild);

private:
	/** getParent.
	*   The parent node will be returned
	*/
	bool getParent(CmStringFTL** _Parent);

private:
	/** getNextItemX.
	*		The next item in a FTLight line as well as its type will be determined.
	*   The active range in LineX will be shortened by found ItemX and TypeX 
	*   information. The ItemX position and length are returned as 'highlight'.
	*   When starting a new line, 'highlight' needs to be set to 'active' range.
	*/
	bool getNextItemX(CmString& _LineX, CmString& _ItemX, CmTypeFTL& _TypeX, bool& _isStartOfCollection, bool _isParentBinary);

private:
	/** createRoot.
	*   A standard node hierarchy consisting of an operator node, a location node
	*   and an identifier node will be generated if it does not exist yet. The
	*   identifier node will become the root of further items to be inserted.
	*   Alternatively, an empty root node can be created.
	*/
	bool createRoot(CmString& _Identifier);

public:
	bool createRoot();

private:
	/** getChecksum.
	*   Current line's checksum will be evaluated.
	*/
	bool getChecksum(CmString& _Line, CmString& _Checksum);

private:
	/** verifyChecksum.
	*   Current line's checksum will be evaluated and be checked against the
	*   checksum that was attached as last item at the end of line. The range
	*   depends on checksum's length
	*/
	bool verifyChecksum(CmString& _Line, CmString& _Checksum);
#define CHECKSUM_DIGIT_1  ((int64)216)
#define CHECKSUM_DIGIT_2  ((int64)216*216)
#define CHECKSUM_DIGIT_3  ((int64)216*216*216)
#define CHECKSUM_DIGIT_4  ((int64)216*216*216*216)
#define CHECKSUM_DIGIT_5  ((int64)216*216*216*216*216)
#define CHECKSUM_DIGIT_6  ((int64)216*216*216*216*216*216)
#define CHECKSUM_DIGIT_7  ((int64)216*216*216*216*216*216*216)

private:
	/** getChild.
	*   Find or create a child node at specified position
	*/
	bool getChild(int32 _ChildPosition);

private:
	/** isChild.
	*   Find a child node at specified position
	*/
	bool isChild(int32 _ChildPosition);

private:
	/** getPosition.
	*   A position according to specified address will be adjusted with PosX.
	*/
	bool getPosition(const CmString& _Address);

private:
	/** findPosition.
	*   A position according to specified address will be searched for. If it
	*   exists then its address will be returned in PosX.
	*   Additionally, a start address can be specified which will replace the
	*   first part of the main Address.
	*/
	bool findPosition(const CmString& _Address, CmString _StartAddr = CmString());

private:
	/** getLocal.
	*   A Local will be returned according to received address string
	*/
	bool getLocal(const CmString& _Address, CmStringFTL*& _LocalX);

private:
	/** initLineX.
	*   The LineX reader will be prepared for reading next line.
	*/
	bool initLineX(CmString& _LineX, uint32& _LineStart, uint32& _ActiveStart, uint32& _ActiveLength, uint32& _HighlightStart, uint32& _HighlightLength);

private:
	/** checkSyncronousWriting.
	*   The first item in a line will be checked whether syncronous writing takes place.
	*/
	bool checkSyncronousWriting(CmTypeFTL _TypeX);

private:
	/** runSynchronousWriting.
	*   An item will be handled in the context of a synchronous writing operation.
	*   If an item was consumed then the return is true or otherwise false.
	*/
	bool runSynchronousWriting(CmString& _ItemX, CmTypeFTL _TypeX);

private:
	/** evaluateRepeatedPath.
	*   In case of a zero-length item, a repeated path will be evaluated.
	*   The return value will be true for a repeated path item.
	*/
	bool evaluateRepeatedPath();

private:
	/** putChildOnPath.
	*   The final path will be evaluated and a child be generated on it.
	*/
	bool putChildOnPath(CmString& _ItemX, CmTypeFTL& _TypeX, CmStringFTL *_LocalX = NULL);

private:
	// FTLight structure and type
	CmStringFTL * Parent;
	CmStringFTL * Sibling;
	CmStringFTL * Child;

protected:
	CmTypeFTL TypeX;						// TypeX according to FTLight specification
	CmDataFormat DataFormat;	// content type in CmString, e.g DATAFORMAT_FTLight,
														// for all other formats the FTLight content may be in TextX/LengthX

protected:
	// FTLight content
	size_t LengthX;
	uint8 *TextX;

private:
	typedef struct _NaviX{
		// positions in FTLight hierarchy
		CmStringFTL * Root;						// current FTLight hierarchy's root node
		CmStringFTL * PosX;						// current position in FTLight hierarchy
		CmStringFTL * PathX;					// current path in FTLight hierarchy

		// FTLight state/positions for synchronous writing
		struct {
			bool isSynchronousWriting;
			bool isFixedParentCollection;
			// hierarchy positions/parameters
			int64 FirstParentPos;
			int64 ParentPos;
			int64 Levels;
			int64 SequenceNumber;
			int64 BufferLength;
			int64 BufferPos;
		} sync;

		// parser state
		bool isPath;
		bool isRepeatedPath;
		bool isFirstItem;
		bool isOption;
		bool isStartOfCollection;
		bool isWaitForBufferStart;
		bool isWaitForBufferLength;
		bool isSingleAddressBeforeAt;
		bool isRestoreSynchronousWriting;
		bool isParentBinary;

		/*_NaviX(){
			// constructor
			Root = NULL;
			PosX = NULL;
			PathX = NULL;
		};*/

	} NaviX;

private:
	// FTLight tree navigation
	NaviX *Navigation;
	NaviX * X();

};

//------------------------------------------------------------------------------
// CmStringFTLroot class
//------------------------------------------------------------------------------
//
/** CmStringFTLroot.
*   During initialization of this class, members will be adjusted such that it
*   can function as an (empty) root in a CmStringFTL hierarchy (without identifier).
*/
class CmStringFTLroot : public CmStringFTL
{
public:
	CmStringFTLroot();
	~CmStringFTLroot();
};

} // namespace Cosmos

using namespace Cosmos;

#endif
