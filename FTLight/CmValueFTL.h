//////////////////////////////////////////////////////////////////////////////
//
// CmValueFTL.h: Declaration of the CmValueFTL class
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

#ifndef CmValueFTLH
#define CmValueFTLH

#include "FTLight/CmStringFTL.h"


namespace Cosmos
{

// forward declaration
class CmMatrix;
class CmValueINI;

//------------------------------------------------------------------------------
// CmMatrixFTL class
//------------------------------------------------------------------------------
//
/** CmMatrixFTL.
*  This is a polydimensional sparse matrix implementation. It allows for a 
*  dynamic operation of independent dimensional levels. A type information is 
*  maintained in addition to each of the stored values. A matrix element can 
*  be another CmMatrixFTL which provides to the next level  of the dimensional 
*  hierarchy inside the matrix.
*/
class CmMatrixFTL : public CmVector<uint64>
{
public:
	CmMatrixFTL();
	virtual ~CmMatrixFTL();

public:
	/** unit test for CmMatrixFTL including CmVector */
	bool testCmMatrixFTL();

public:
	/** clearMatrix.
	*  All matrix elements will be removed starting from root respectively from
	*  a specified matrix level. The matrix/sub-matrix will become empty.
	*/
	bool clearMatrix();
	bool clearMatrix(CmIndex& _Index);

public:
	/** getVectorLength.
	*  An index may point to a vector inside the matrix same as the matrix itself
	*  represents a root vector. The function returns the length of that vector.
	*  The return value will be zero whenever there is no item connected at the
	*  indexed position. It will as well be zero when the connected item is not
	*  a vector but any data item like a string, a double or another value.
	*/
	int32 getVectorLength(CmIndex& _Index);

protected:
	/** set/getScalar.
	*  A scalar value can be set/get to/from a matrix as well as information
	*  about the scalar's format.
	*/
	bool setScalar(uint64 _Scalar, CmDataFormat _ScalarFormat);
	bool getScalar(uint64& _Scalar, CmDataFormat& _ScalarFormat);

public:
	/** operator[].
	*  This default access function returns the indexed value as a double in case
	*  it was stored as a numeric value and if the last position in the Index has
	*  been reached. Otherwise it will return 0 if the indexed item is an address
	*  or a string or if it was not assigned.
	*/
	double operator[](CmIndex& _Index);
	double operator[](int32 _Index);

public:
	/** setValue/String.
	*   The binary layout of a data value will be written to a 64-bit field.
	*   Memory will be allocated in case of a string to store the string.
	*   Subsequently, the string's address will be written to the 64-bit data
	*   field. Also, the data format of the data value will be stored.
	*/
	bool setValue(int8 _Val8, CmIndex& _Index);
	bool setValue(uint8 _Val8, CmIndex& _Index);
	bool setValue(bool _ValBool, CmIndex& _Index);
	bool setValue(int16 _Val16, CmIndex& _Index);
	bool setValue(uint16 _Val16, CmIndex& _Index);
	bool setValue(int32 _Val32, CmIndex& _Index);
	bool setValue(uint32 _Val32, CmIndex& _Index);
	bool setValue(float _ValFloat, CmIndex& _Index);
	bool setValue(int64 _Val64, CmIndex& _Index);
	bool setValue(uint64 _Val64, CmIndex& _Index);
	bool setValue(double _ValDouble, CmIndex& _Index);
	bool setValue(void* _ValPointer, CmIndex& _Index); // NOTE: valid only below max double
	bool setString(const CmString& _String, CmIndex& _Index);

protected:
	/** as<data format>...
	*   These are several conversion functions for particular data formats
	*/
	CmString asString(CmIndex& _Index);
	uint64 asUInt64(CmIndex& _Index);
	int64 asInt64(CmIndex& _Index);
	double asDouble(CmIndex& _Index);
	uint32 asUInt32(CmIndex& _Index);
	int32 asInt32(CmIndex& _Index);
	float asFloat(CmIndex& _Index);
	uint16 asUInt16(CmIndex& _Index);
	int16 asInt16(CmIndex& _Index);
	uint8 asUInt8(CmIndex& _Index);
	int8 asInt8(CmIndex& _Index);
	bool asBool(CmIndex& _Index);

protected:
	/** getBeforeLastLevel.
	*  The last level of dimension that holds a value will be retrieved. If intermediate
	*  levels of dimension are still missing then they will be created.
	*/
	CmMatrixFTL* getBeforeLastLevel(CmIndex& _IndexVector, bool _isCreateNextLevel = true);

	/** set/getValue.
	*  A uint64 value will be set to specified item position in the vector. If the
	*  position is occupied by a CmMatrixFTL object then the scalar value of that
	*  object will be set/get instead. Further, format information will be assigned
	*  and retrieved.
	*/
	bool setValue(uint64 _Value, CmDataFormat _Format, CmIndex& _Index);
	bool getValue(uint64& _Value, CmDataFormat& _Format, CmIndex& _Index);

	friend CmMatrix;

	//--------workspace-----------------------------------------------------------

protected:
	// format information
	CmVector<CmDataFormat> DataFormat;
};

/** CmMatrix.
*   This class extends the CmMatrixFTL class by an index variable. The purpose
*   is to save index information inside an overloaded 'operator[]' in order to
*   use that index in overloaded 'operator <type>' and 'operator==' functions.
*/
class CmMatrix : public CmMatrixFTL
{
public:
	CmMatrix();
	~CmMatrix();

public:
	/** clearMatrix.
	*  All matrix elements will be removed starting from root respectively from
	*  a specified matrix level. The matrix/sub-matrix will become empty.
	*/
	bool clearMatrix();

public:
	/** addressing matrix element by an index */
	CmMatrix& operator[](CmIndex& _Index);
	CmMatrix& operator()();
	CmMatrix& operator()(int _a);
	CmMatrix& operator()(int _a, int _b);
	CmMatrix& operator()(int _a, int _b, int _c);
	CmMatrix& operator()(int _a, int _b, int _c, int _d);
	CmMatrix& operator()(int _a, int _b, int _c, int _d, int _e);
	CmMatrix& operator()(int _a, int _b, int _c, int _d, int _e, int _f);
	CmMatrix& operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g);
	CmMatrix& operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g, int _h);
	CmMatrix& operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g, int _h, int _i);
	CmMatrix& operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g, int _h, int _i, int _j);
	CmMatrix& operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g, int _h, int _i, int _j, int _k);
	CmMatrix& operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g, int _h, int _i, int _j, int _k, int _l);
	CmMatrix& operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g, int _h, int _i, int _j, int _k, int _l, int _m);
	CmMatrix& operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g, int _h, int _i, int _j, int _k, int _l, int _m, int _n);
	CmMatrix& operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g, int _h, int _i, int _j, int _k, int _l, int _m, int _n, int _o);

public:
	/** matrix write access */
	bool operator=(const CmString& _String);
	bool operator=(const int8* _String);
	bool operator=(void* _Value); // NOTE: valid only below max double
	bool operator=(uint64 _Value);
	bool operator=(int64 _Value);
	bool operator=(double _Value);
	bool operator=(uint32 _Value);
	bool operator=(int32 _Value);
	bool operator=(float _Value);
	bool operator=(uint16 _Value);
	bool operator=(int16 _Value);
	bool operator=(uint8 _Value);
	bool operator=(int8 _Value);
	bool operator=(bool _Value);

public:
	/** matrix read access and subsequent type conversion */
	operator CmString();
	operator const int8*();
	operator void*(); // NOTE: valid only below max double
	operator uint64();
	operator int64();
	operator double();
	operator uint32();
	operator int32();
	operator float();
	operator uint16();
	operator int16();
	operator uint8();
	operator int8();
	operator bool();

public:
	/** getSizeLastLevel. */
	int32 getSizeLastLevel();

public:
	/** addValue. A matrix level is searched for a given int32 value. If it
	*   was not found then it will be added and true is returned. The found 
	*   respectively added new position will be returned.
	*/
	bool addValue(int32& _Position, int32 _Value);

public:
	/** appendItem. A matrix level is searched for a given text item. If it
	*   was not found then it will be appended at the end. The return code 
	*   will be true if the item was appended and otherwise it will be false.
	*/
	bool appendItem(const int8* _Item);
	bool appendItem(const CmString& _Item);

public:
	/** findItem. 
	* A matrix level is searched for a given text item on scalar position. If 
	* it was not found then a new node will be appended at the end. The matrix 
	* that holds the Item at scalar position will be returned.
	*/
	CmMatrix& findScalarItem(const CmString& _Item);

public:
	/** insertItem. All items on a matrix level with an index equal or higher than
	*   a given start index will be moved one index up. A new matrix entry will be
	*   generated on specified index position. 
	*/
	bool insertItem(int32 _Position);

public:
	/** deleteItem. The item at specified position will be deleted.
	*/
	bool deleteItem(int32 _Position);


	//--------workspace-----------------------------------------------------------

private:
	// last index
	CmIndex Index;
};


//------------------------------------------------------------------------------
// CmValueFTL class
//------------------------------------------------------------------------------
//
/** CmValueFTL.
*	This class encapsulates a value which is updated from/synchronized with a 
*   FTLight structure. It can be used for delivering a value to modules that 
*   receive FTLight files/streams or when those values are sent.
*/
class CmValueFTL : public CmStringFTL
{
#define UURI_CmValueFTL		"EKD@JN58nc_Türkenfeld."APP_PROGRAM

public:
	CmValueFTL(int8 *_Address = NULL);
	CmValueFTL(CmTypeFTL _TypeFTL);
	virtual ~CmValueFTL();

public:
	/** unit test for CmValueFTL */
	bool testCmValueFTL();

public:
	/** getAddress.
	*   The address in FTLight hierarchy is returned.
	*/
	const CmString& getAddress();

public:
	/** getNextConfigValue.	*/
	CmValueFTL& getNextConfigValue(bool& _isEndOfConfigValues);

public:
	/** getConfigValue. Return indexed config value starting at this. */
	CmValueFTL* getConfigValue(int32& _Index);

public:
	/** allValuesFTL. Iterate the list of CmValueFTL */
	bool allValuesFTL(CmValueFTL** _ValueFTL);

public:
	/** update.
	*   The value(s) will be updated from a FTLight structure using stored address.
	*   Alternatively, an index can be specified as a trailing address component.
	*   Further, a start address can be determined that address calculations refers to.
	*/
	bool updateSettings(CmStringFTL& _StringFTL, CmString _StartAddr = CmString());
	bool updateFrom(CmStringFTL& _StringFTL, CmString _StartAddr = CmString());
	bool updateFrom(CmStringFTL& _StringFTL, int32 _Index, CmString _StartAddr = CmString());

public:
	/** syncSettings.
	*  A FTLight structure will be synchronized with Settings (string) values
	*/
	bool syncSettings(CmStringFTL& _StringFTL);

public:
	/** updateInfoFTL.
	*  The InfoFTL values will be updated from a CmStringFTL structure.
	*/
	bool updateInfoFTL(CmStringFTL& _InfoFTL, CmValueFTL& _Return, CmString _StartAddr = CmString());

public:
	/** serializeValueFTL
	*  An array of CmValueFTL items will synchronize its CmStringFTL template.
	*  Subsequently, the updated CmStringFTL will be converted to a string.
	*/
	bool serializeValueFTL(CmString& _StringFTL, CmStringFTL& _TemplateFTL, CmValueINI& _Return);

public:
	/** writeInfoFTL.
	*  A config's CmStringFTL representation will be updated from a config's CmValueFTL structure.
	*  Subsequently, the config's CmStringFTL representation will be written to disk.
	*  Optionally, the configuration file can be written to a subfolder with same name as the file.
	*/
	bool writeInfoFTL(CmString _ConfigPath, CmStringFTL& _InfoFTL, CmValueINI& _Return, const int8* _NameExtension = NULL);

public:
	/** readInfoFTL.
	*		A config string will be read from disk. Subsequently, the string will be converted into
	*   a CmStringFTL structure and that will be used to update a config's CmValueFTL structure.
	*   Optionally, the configuration file can read from a subfolder with same name as the file.
	*/
	bool readInfoFTL(CmString _ConfigPath, CmValueINI& _Return, const int8* _NameExtension = NULL);

public:
	/** determineListOfProfiles.
	*  The profile folder will be searched for available configuration files.
	*  A list of profile name extensions will be returned in a matrix as follows:
	*
	*  [']=update flag, [i]=profile name extension
	*/
	bool determineListOfProfiles(CmString _ConfigPath, CmString _AppProfile, CmString _AppVersion);

public:
	/** updateInformation.
	*  Local variables will be updated from a FTLight coded string.
	*/
	bool updateInformation(CmString& _Information, CmValueINI& _Return);

public:
	/** clearConfig. Local Config variables will be cleared */
	bool clearConfig();

public:
	/** setValue.
	*   The binary layout of a data value will be written to a 64-bit field.
	*   Also, the data format will be set appropriately.
	*/
#ifdef CM_TYPE_OLD_STYLE
	void setValue(int8 Val8);
	void setValue(uint8 Val8);
	void setValue(bool ValBool);
	void setValue(int16 Val16);
	void setValue(uint16 Val16);
	void setValue(int32 Val32);
	void setValue(uint32 Val32);
	void setValue(float ValFloat);
	void setValue(int64 Val64);
	void setValue(uint64 Val64);
	void setValue(double ValDouble);
	void setValue(void* ValPointer);
#endif

public:
	/** operator=(). Write access to CmValueFTL */
	const CmStringFTL& operator=(const CmStringFTL& _StringFTL);
	const CmString& operator=(const CmString& _String);
	const int8* operator=(const int8* _String);
	void* operator = (void* _ValPointer);
	int8 operator = (int8 _Val8);
	uint8 operator = (uint8 _Val8);
	bool operator = (bool _ValBool);
	int16 operator = (int16 _Val16);
	uint16 operator = (uint16 _Val16);
	int32 operator = (int32 _Val32);
	uint32 operator = (uint32 _Val32);
	float operator = (float _ValFloat);
	int64 operator = (int64 _Val64);
	uint64 operator = (uint64 _Val64);
	double operator = (double _ValDouble);

public:
	/** as<data format>...
	*   These are several conversion functions for particular data formats
	*/
	CmString asStringFTL(CmString _Prefix, int32 _Precision = -1, int32 _Digits = -1);
	CmString asStringFTLbool(CmString _Prefix);
	CmString& asStringConvert(int32 _Precision = -1, int32 _Digits = -1);
#ifdef CM_TYPE_OLD_STYLE
	const int8* getText();
	void* asPointer();
	uint64 asUInt64();
	int64 asInt64();
	double asDouble();
	uint32 asUInt32();
	int32 asInt32();
	float asFloat();
	uint16 asUInt16();
	int16 asInt16();
	uint8 asUInt8();
	int8 asInt8();
	bool asBool();
#endif

public:
	/** explicit type conversion */
	operator CmString&();
	operator const int8*();
	operator void*();
	operator uint64();
	operator int64();
	operator double();
	operator uint32();
	operator int32();
	operator float();
	operator uint16();
	operator int16();
	operator uint8();
	operator int8();
	operator bool();

public:
	/** compare operators. Content will be evaluated dependent on type */
	// string
	bool operator == (const int8* _String);
	bool operator != (const int8* _String);
	bool operator == (CmString& _String);
	bool operator != (CmString& _String);
	// double
	bool operator == (double _Value);
	bool operator != (double _Value);
	bool operator > (double _Value);
	bool operator < (double _Value);
	// int
	bool operator == (int64 _Value);
	bool operator != (int64 _Value);
	bool operator > (int64 _Value);
	bool operator < (int64 _Value);
	// uint
	bool operator == (uint64 _Value);
	bool operator != (uint64 _Value);
	bool operator > (uint64 _Value);
	bool operator < (uint64 _Value);
	// bool
	bool operator == (bool _Value);
	bool operator != (bool _Value);

public:
	// large numbers operators
	CmString& setNumber(const CmString& _Num);
	// comparison
	bool operator > (const CmValueFTL& _Value);
	bool operator < (const CmValueFTL& _Value);
	bool operator >= (const CmValueFTL& _Value);
	bool operator <= (const CmValueFTL& _Value);
	// arithmetic operations
	CmString operator+ (const CmValueFTL& _B);
	CmString& operator+= (const CmValueFTL& _B);
	CmString operator- (const CmValueFTL& _B);
	CmString& operator-= (const CmValueFTL& _B);
	CmString operator* (const CmValueFTL& _B);
	CmString& operator*= (const CmValueFTL& _B);
	CmString operator/ (const CmValueFTL& _B);
	CmString& operator/= (const CmValueFTL& _B);
	CmString operator% (const CmValueFTL& _B);
	CmString& getRemainder();

public:
	// string operations
	void operator+=(const CmString& _String);
	void operator+=(const int8* _String);
	void operator+=(int32 _Value);
	void operator+=(uint32 _Value);
	void operator+=(int64 _Value);
	void operator+=(uint64 _Value);

private:
	/** asDataFormat.
	*   A string/numeric content will be converted into requested data format
	*/
	bool asDataFormat(CmDataFormat _DataFormat, int32 _Precision = -1, int32 _Digits = -1);
	int32 estimatePrecision(double _Value, int32 _ValidDigits = 2);

//--------CmMatrixFTL-integration---------------------------------------------
public:
	/** clearMatrix.
	*  All matrix elements will be removed. The matrix will become empty.
	*/
	bool clearMatrix();

#ifdef CM_TYPE_OLD_STYLE
public:
	/** getVectorLength.
	*  An index may point to a vector inside the matrix same as the matrix itself
	*  represents a root vector. The function returns the length of that vector.
	*  The return value will be zero whenever there is no item connected at the
	*  indexed position. It will as well be zero when the connected item is not
	*  a vector but any data item like a string, a double or another value.
	*/
	int32 getVectorLength(CmIndex& _Index);
	int32 getVectorLength();

public:
	/** operator[].
	*  This default access function returns the indexed value as a double in case
	*  it was stored as a numeric value and if the last position in the Index has
	*  been reached. Otherwise it will return 0 if the indexed item is an address
	*  or a string or if it was not assigned.
	*  Furthermore, the index will be saved for a subsequent type convertion access.
	*/
	double operator[](CmIndex& _Index);

public:
	/** setValue/String.
	*   The binary layout of a data value will be written to a 64-bit field.
	*   Memory will be allocated in case of a string to store the string.
	*   Subsequently, the string's address will be written to the 64-bit data
	*   field. Also, the data format of the data value will be stored.
	*/
	bool setValue(int8 _Val8, CmIndex& _Index);
	bool setValue(uint8 _Val8, CmIndex& _Index);
	bool setValue(bool _ValBool, CmIndex& _Index);
	bool setValue(int16 _Val16, CmIndex& _Index);
	bool setValue(uint16 _Val16, CmIndex& _Index);
	bool setValue(int32 _Val32, CmIndex& _Index);
	bool setValue(uint32 _Val32, CmIndex& _Index);
	bool setValue(float _ValFloat, CmIndex& _Index);
	bool setValue(int64 _Val64, CmIndex& _Index);
	bool setValue(uint64 _Val64, CmIndex& _Index);
	bool setValue(double _ValDouble, CmIndex& _Index);
	bool setString(const CmString& _String, CmIndex& _Index);

public:
	/** as<data format>...
	*   These are several conversion functions for particular data formats
	*/
	CmString asString(CmIndex& _Index);
	uint64 asUInt64(CmIndex& _Index);
	int64 asInt64(CmIndex& _Index);
	double asDouble(CmIndex& _Index);
	uint32 asUInt32(CmIndex& _Index);
	int32 asInt32(CmIndex& _Index);
	float asFloat(CmIndex& _Index);
	uint16 asUInt16(CmIndex& _Index);
	int16 asInt16(CmIndex& _Index);
	uint8 asUInt8(CmIndex& _Index);
	int8 asInt8(CmIndex& _Index);
	bool asBool(CmIndex& _Index);

#endif

public:
	/** M.
	* Retrieve respectively create the root of a polydimensional matrix.
	*/
	CmMatrix& getMatrix();

//--------CmMatrixFTL-application---------------------------------------------

public:
	/** set/getItemUpdated.
	*  Item updates are indicated by a flag in the scalar position of a matrix.
	*
	*  [`]=item updated
	*/
	bool setItemUpdated();
	bool clearItemUpdated();

public:
	/** set/getDrawingEnabled/ItemEnabled.
	*  Drawing on screen can be activated/deactivated by a flag in the scalar
	*  position of a matrix respectively in the scalar position of an item.
	*
	*  [`]=enabled
	*/
	bool setDrawingEnabled(bool _isDrawingEnabled);
	bool getDrawingEnabled();
	bool setItemEnabled(int32 _Index, bool _isItemEnabled);
	bool getItemEnabled(int32 _Index);
	int32 getItemCount();

public:
	/** setChartBackground/Axis.
	*  The background parameters as well as the axis parameters are set/get.
	*
	* [0]=background, [1]=alpha, [2]=Xmin, [3]=Xmax, [4]=Xmin, [5]=Xmax
	*/
	bool setChartBackground(const CmString& _Color, double _Alpha);
	bool getChartBackground(CmString& _Color, double& _Alpha);
	bool setRangeX(double _Xmin, double _Xmax);
	bool getRangeX(double& _Xmin, double& _Xmax);
	bool setRangeY(double _Ymin, double _Ymax);
	bool getRangeY(double& _Ymin, double& _Ymax);


public:
	/** setChartText
	*  A chart text definition along with all parameters will be written to
	*  the matrix on a given index position, respectively it will be retrieved. 
	*  The set of display parameters can also be defined as default parameters.
	*  The text output can be disabled for particular items or in general.
	*
	*  [`]=enabled, [i]=default, [0,l]=disabled, [0,l,0/1]=position x/y, [1,l]=text, [2,l]=size, [3,l]=rotation, [4,l]=color, [5,l]=font, [6,l]=alpha
	*/
	bool setChartText(int32& _Index, const CmPoint2D& _Position, const CmString& _Text, double _FontSize = -1, double _Rotation = -1, const CmString& _Color = "", double _Alpha = -1, const CmString& _Font = "");
	bool setChartTextWrap(int32& _Index, const CmPoint2D& _Position, const int32 _PosWrap, const CmString& _Text, double _FontSize = -1, double _Rotation = -1, const CmString& _Color = "", double _Alpha = -1, const CmString& _Font = "");
	bool setChartTextDefaults(double _FontSize = 10, double _Rotation = 0, const CmString& _Color = "Black", double _Alpha = 1, const CmString& _Font = "Arial");
	bool getChartText(int32 _Index, CmPoint2D& _Position, CmString& _Text, double& _FontSize, double& _Rotation, CmString& _Color, double& _Alpha, CmString& _Font);

public:
	/** putChartLine
	*  A chart line definition along with all parameters will be written to
	*  the matrix on a given index position, respectively it will be retrieved.
	*  The set of display parameters can also be defined as default parameters.
	*  The output can be disabled for particular lines or in general.
	*
	*  [`]=enabled, [i]=default, [0,p]=disabled, [0,p,0,0../1,0..]=points, [1,p]=width, [2,p]=line color, [3,p]=fill color, [4,p]=line alpha, [5,p]=fill alpha
	*/

	bool setChartLine(int32 _Index, CmVector<float>& _PointX, CmVector<float>& _PointY, double _LineWidth = -1, const CmString& _LineColor = "", const CmString& _FillColor = "", double _LineAlpha = -1, double _FillAlpha = -1);
	bool setChartRectangle(int32 _Index, const CmRectangle2D& _Rectangle, double _LineWidth = -1, const CmString& _LineColor = "", const CmString& _FillColor = "", double _LineAlpha = -1, double _FillAlpha = -1);
	bool setChartLineDefaults(double _LineWidth = 1, const CmString& _LineColor = "Black", const CmString& _FillColor = "Gray", double _LineAlpha = 1, double _FillAlpha = 0);
	bool getChartLine(int32 _Index, CmVector<float>& _PointX, CmVector<float>& _PointY, double& _LineWidth, CmString& _LineColor, CmString& _FillColor, double& _LineAlpha, double& _FillAlpha);

public:
	// return access to LogLevel, Message and Context of a Dynamics structure,
	// assuming that those are subsequent config values
	CmValueFTL& getLogLevel();
	CmValueFTL& getMessage();
	CmValueFTL& getContext();

	

//--------workspace-----------------------------------------------------------

public:
	CmValueFTL* Next;

protected:
	// address in FTLight hierarchy
	CmString Address;

protected:
	// chain of CmValueFTL
	static CmValueFTL* ValuesFTL;
	
protected:
	// linear array of CmValueFTL elements
	CmValueFTL *Array;

	// polydimensional matrix inside this CmValueFTL 
	CmMatrix *Matrix;

private:
	// remainder of a deRemote operation
	CmString Remainder;
};

//------------------------------------------------------------------------------
// CmValueINI class
//------------------------------------------------------------------------------
//
/** CmValueINI.
*	This class generates addresses for a FTLight structure automatically and
*   initializes the structure's nodes.
*   The position inside the structure is controlled as follows:
*   NULL or "" (empty string) or a valid UURI -> root node for the UURI
*   "/name:value"    -> name:value pair on first level below root
*   "name:value"     -> a subsequent name:value on current (hierarchy) level 
*   ">name:value"    -> name:value pair on a level below last 'name' level
*   ">>name:value"   -> name:value pair on a level below last 'value' level
*   "<name:value"    -> name:value pair on a level above last 'name' level
*   "<..<name:value" -> name:value two (or more) levels above last 'name' level
*
*   Interface to FTLight repository:
*   
*   ---------------------------
*	UURI:Timestamp:Period:86400
*	/Header
*    ...
*   /Data
*   >DataTimestamp:timebase s
*   Name1:base unit
*   ...
*   NameN:base unit
*   ---------------------------
*
*   The Period determines the time from start to end of a file. The Timestamp
*   will be generated as a multiple of the Period.
*
*   All lines from Header to Data will be copied to each new file.
*
*   After the Data entry line, the column headers will be written to the next line
*   followed by an '@' as indicator for parallel writing. The column headers contain
*   information about the data values' base and unit as specified in the Interface.
*
*   Subsequently, all data sets with a DataTimestamp equal or higher to  Timestamp
*   and less than Timestamp + Period will be added after column headers.
*   Each particular data value will be a BinX coded integer. The absolute measure
*   value can be retrieved by multiplying the integer value by the factor in the 
*   column header. It will be in the unit contained as well in the column header.
*/
class CmValueINI : public CmValueFTL
{
public:
	CmValueINI(const int8 *_Init = NULL);
	virtual ~CmValueINI();

public:
	/** unit test for CmValueINI */
	bool testCmValueINI();

public:
	// retrieve name from a name:value pair
	static CmString getName(CmString _NameValue, int* _PosName = NULL);

public:
	// retrieve value from a name:value pair
	static CmString getValue(CmString _NameValue);

public:
	// An array of CmValueFTL items will be initialized by a CmStringFTL content
	static bool setDefaultInfoFTL(CmValueINI& _ValueINI);
	static bool setDefaultInfoFTL(CmValueINI& _ValueINI, CmValueFTL& _Return);

public:
	/** getInfoFTL.
	*   A InfoFTL object will be generated from initialization string at UURI.
	*   Subsequently, it will be synchronized with the CmValueFTL items array.
	*/
	bool getInfoFTL(CmStringFTL& _InfoFTL, CmValueINI& _Return);

public:
	/** operator=(). Write access to CmValueFTL */
	const CmStringFTL& operator=(const CmStringFTL& _StringFTL){ return CmValueFTL::operator=(_StringFTL); }
	const CmString& operator=(const CmString& _String){ return CmValueFTL::operator=(_String); }
	const int8* operator=(const int8* _String){ return CmValueFTL::operator=(_String); }
	void* operator = (void* _ValPointer){ return CmValueFTL::operator=(_ValPointer); }
	int8 operator = (int8 _Val8){ return CmValueFTL::operator=(_Val8); }
	uint8 operator = (uint8 _Val8){ return CmValueFTL::operator=(_Val8); }
 	bool operator = (bool _ValBool){ return CmValueFTL::operator=(_ValBool); }
	int16 operator = (int16 _Val16){ return CmValueFTL::operator=(_Val16); }
	uint16 operator = (uint16 _Val16){ return CmValueFTL::operator=(_Val16); }
	int32 operator = (int32 _Val32){ return CmValueFTL::operator=(_Val32); }
	uint32 operator = (uint32 _Val32){ return CmValueFTL::operator=(_Val32); }
	float operator = (float _ValFloat){ return CmValueFTL::operator=(_ValFloat); }
	int64 operator = (int64 _Val64){ return CmValueFTL::operator=(_Val64); }
	uint64 operator = (uint64 _Val64){ return CmValueFTL::operator=(_Val64); }
	double operator = (double _ValDouble){ return CmValueFTL::operator=(_ValDouble); }

public:
	/** drawBinDIF. BinDIF data will be projected on a canvas.*/
	template <typename T>
	bool drawBinDIF(CmVector<T>& _X, CmVector<T>& _Y, float _Left, float _Right, float _Top, float _Bottom, float _Baseline, double& _AmplitudeMin, double& _AmplitudeMax, double& _RangeMin, float _Reduction=1.05)
	{
		// decode BinDIF data
		CmVector<int64> ValDIF;
		asVector(ValDIF);

		// determine min and max amplitude
		int64 AmplitudeMin = MAX_INT64;
		int64 AmplitudeMax = MIN_INT64;
		for (int i = 0; i < ValDIF.getLength(); i++){
			if (AmplitudeMin > ValDIF[i]){
				AmplitudeMin = ValDIF[i];
			}
			if (AmplitudeMax < ValDIF[i]){
				AmplitudeMax = ValDIF[i];
			}
		}
		// adjust for even range values
		AmplitudeMin = int64(10 * ceil(_Reduction * AmplitudeMin / 10));
		AmplitudeMax = int64(10 * ceil(_Reduction * AmplitudeMax / 10));
		// determine scaling
		double ScaleMax = AmplitudeMax != 0 ? (_Top - _Baseline) / AmplitudeMax : 0;
		double ScaleMin = AmplitudeMin != 0 ? (_Bottom - _Baseline) / AmplitudeMin : 0;
		double Scale = AmplitudeMin >= 0 ? ScaleMax : AmplitudeMax <= 0 ? ScaleMin : min(ScaleMin, ScaleMax);
		// minimal range constraint
		if (_RangeMin * Scale > _Top - _Baseline){
			Scale = (_Top - _Baseline) / _RangeMin;
		}
		else{
			_RangeMin = double(AmplitudeMax > 0 ? AmplitudeMax : -AmplitudeMin);
		}

		_AmplitudeMin = double(AmplitudeMin);
		_AmplitudeMax = double(AmplitudeMax);

		// beginning left
		_X[0] = _Left;	_Y[0] = _Bottom;

		// generate chart data
		int32 DataLength = ValDIF.getLength();
		float RangeX = _Right - _Left + 1;
		int i = 0;
		for (; i < DataLength; i++){
			_X[i + 1] = T(RangeX * i / DataLength + _Left);
			_Y[i+1] = T(Scale * ValDIF[i] + _Baseline);
		}

		// finishing right
		_X[i] = T(_Right);	_Y[i] = T(_Bottom);

		return true;
	}

public:
	// access initialization string
	bool setStringINI(CmString& _StringINI);
	CmString* getStringINI();

public:
	// clear workspace
	bool clearWorkspace();

	//--------workspace-----------------------------------------------------------

private:
	// automatic address and FTL hierarchy generation
	CmMatrix* MatrixINI;
	CmString* StringINI;
};

//--------------------------------------------------------------------------------
// template for deriving a new config from a default config
//--------------------------------------------------------------------------------
template <typename T>
T& newConfig(T** T1, T& T0){
	if (NULL == *T1){
		// create a new config from default config
		*T1 = new T(T0);
		// fallback to default config if 'new' failed
		NULL == *T1 ? *T1 = &T0 : 0;
		// obtain member list
		int32 Count = sizeof(T) / sizeof(CmValueINI);
		CmValueINI* INI = (CmValueINI*)*T1;
		// adjust member list
		for (int32 i = 0; i < Count - 1; i++){
			(INI + i)->Next = INI + i + 1;
		}
		// clear workspace (which has to be disconnected from template, otherwise desctructor will fail) 
		for (int32 i = 0; i < Count; i++){
			(INI + i)->clearWorkspace();
		}
		// copy StringINI 
		CmValueINI* UURI = (CmValueINI*)&T0;
		NULL != UURI->getStringINI() ? INI->setStringINI(*UURI->getStringINI()) : 0;
	}

	return **T1;
}

} // namespace Cosmos

using namespace Cosmos;

#endif
