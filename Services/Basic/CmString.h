//////////////////////////////////////////////////////////////////////////////
//
// CmString.h - Declaration of CmString classes
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

#ifndef CmStringH
#define CmStringH

#include "Services/Basic/CmTypes.h"
#include "Services/Basic/CmException.h"


namespace Cosmos
{

//Collection of characters that function as delimiters in path strings
#define CM_PATH_DELIMITERS             "\\/"

class CmLString;
class CmMString;
template <typename T>
class CmVector;

//CmString provides convenient functions for string handling
class CmString
{
public:
	//Constructors and destructors
	CmString();
	CmString(const int8* _Text);
	CmString(const int8* _Text, uint32 _Length);
	CmString(uint32 _Length, uint8 _Init=0);
	CmString(const CmString& _String);
	CmString(CmVector<uint8>& _VectorUInt8);
	virtual ~CmString();
	void initWorkspace();

public:
	// unit test
	static bool testCmString();

public:
	// set functions for basic types
	void setAt(const uint32 _Pos, int8 _Val8);
	void setAt(const uint32 _Pos, uint8 _Val8);
	void setAt(const uint32 _Pos, bool _ValBool);
	void setAt(const uint32 _Pos, int16 _Val16);
	void setAt(const uint32 _Pos, uint16 _Val16);
	void setAt(const uint32 _Pos, int32 _Val32);
	void setAt(const uint32 _Pos, uint32 _Val32);
	void setAt(const uint32 _Pos, float _ValFloat);
	void setAt(const uint32 _Pos, int64 _Val64);
	void setAt(const uint32 _Pos, uint64 _Val64);
	void setAt(const uint32 _Pos, double _ValDouble);
	void setAt(const uint32 _Pos, void* _ValDouble);
	// get functions for basic types
	void getAt(const uint32 _Pos, int8& _Val8);
	void getAt(const uint32 _Pos, uint8& _Val8);
	void getAt(const uint32 _Pos, bool& _ValBool);
	void getAt(const uint32 _Pos, int16& _Val16);
	void getAt(const uint32 _Pos, uint16& _Val16);
	void getAt(const uint32 _Pos, int32& _Val32);
	void getAt(const uint32 _Pos, uint32& _Val32);
	void getAt(const uint32 _Pos, float& _ValFloat);
	void getAt(const uint32 _Pos, int64& _Val64);
	void getAt(const uint32 _Pos, uint64& _Val64);
	void getAt(const uint32 _Pos, void*& _Val64);
	void getAt(const uint32 _Pos, double& _ValDouble);
	// further set functions
	void clear(); // clear text content
	void reset(); // reset position markers
	void fill(size_t _Length, uint8 _Digit);
	void setText(const CmString& _String);
	void setLength(size_t _Length);
	void adjustLength(size_t _Length);
	int8* setData(const uint8* _Data, size_t _Length);
	int8* setText(const int8* _First, const int8* _Last);
	int8* setText(const int8* _Text);

#ifdef CM_TYPE_OLD_STYLE
	// assignment functions resulting in a 64-bit binary value
	void setValue(int8 _Val8);
	void setValue(uint8 _Val8);
	void setValue(bool _ValBool);
	void setValue(int16 _Val16);
	void setValue(uint16 _Val16);
	void setValue(int32 _Val32);
	void setValue(uint32 _Val32);
	void setValue(float _ValFloat);
	void setValue(int64 _Val64);
	void setValue(uint64 _Val64);
	void setValue(double _ValDouble);
	void setValue(void* _ValPointer);
#endif

public:
	// several access functions to the string and its length information
	bool isEmpty();
	uint32 getLength() const;
	int8*  getBuffer() const;
	const int8* getText() const;
	const uint8* getBinary() const;
	const int16* getUnicode();
	uint32 operator[](uint32 _uPos) const;

public: 
	// string assignment and processing operators
	void operator=(const CmString& _String);
	void operator=(const int8* _Text);
	void operator=(const int32 _Val32);
	void operator=(const uint32 _Val32);
	void operator=(const int64 _Val64);
	void operator=(const uint64 _Val64);
	void operator+(const CmString _String);
	void operator+=(const CmString& _String);
	void operator+=(const int8* _Text);
	void operator+=(int32 _Val32);
	void operator+=(uint32 _Val32);
	void operator+=(int64 _Val64);
	void operator+=(uint64 _Val64);
	// compare operators
	bool operator==(const CmString& _String) const;
	bool operator==(const int8* _Text) const;
	bool operator!=(const CmString& _String) const;
	bool operator!=(const int8* _Text) const;
	bool operator<(const CmString& _String) const;
	bool operator<(const int8* _Text) const;
	bool operator<=(const CmString& _String) const;
	bool operator<=(const int8* _Text) const;
	bool operator>(const CmString& _String) const;
	bool operator>(const int8* _Text) const;
	bool operator>=(const CmString& _String) const;
	bool operator>=(const int8* _Text) const;
	// prefix
	bool isPrefix(const CmString& _String) const;
	bool isPrefix(const int8* _Text) const;

public:
	/** double2String.
	*   Conversion of floating point numbers to a string representation
	*/
	bool double2String(double _Value, int32 _Precision = -1, int32 _Digits = -1);

	/** uint2String.
	*   Conversion of unsigned integer numbers to a string representation
	*/
	bool uint2String(uint64 _Value);

	/** int2hex.
	*   Conversion of integers to a hexadecimal string representation
	*/
	static CmString UInt2Hex(uint64 _Value);
	bool uint2hex(uint64 _Value);
	uint64 hex2uint64();
	uint32 hex2uint32();
	uint16 hex2uint16();
	uint8 hex2uint8();

public:
	/** IP2String.
	*  A network IP address along with a port number will be converted to a string.
	*/
	static CmString IP2String(uint32 _IP, uint16 _Port);

public:
	// Search functions
	int32 findPosition(char _ch, int32 _StartPos = 0);
	int32 findBackwards(char _ch, int32 _StartPos = 0);
	int32 searchPattern(const char *_Pattern, int32 _StartPos = 0);

public:
	/** assignSubString.
	*   Copy a substring from a source to this string.
	*/
	int32 assignSubString(int32 _Last, CmString *_Src = NULL, int32 _First = 0);

public:
	/** set/get/showActive/Highlight
	*   A part or all content of the full string will be marked as 'active/highlight'.
	*   A line number can be specified if LineNum is > 0.
	*/
	bool setActive(uint32 _ActiveStart, uint32 _ActiveLength, int32 _LineNumber = -1);
	bool setHighlight(uint32 _HighlightStart, uint32 _HighlightLength);
	bool getActive(uint32& _ActiveStart, uint32& _ActiveLength, int32& _LineNumber);
	bool getHighlight(uint32& _HighlightStart, uint32& _HighlightLength);
	bool getLineHighlight(int32& _LineNumber, uint32& _LineStart, uint32& _HighlightStart, uint32& _HighlightLength);
	bool showActiveHighlight();

public:
	/** getNumAsDouble/Uint64/Int32.
	*		A string will be evaluated to a double or uint64 value respectively.
	*   'double' accepts: 123, -123, 12.3, 1.2e3, 1.2E-3
	*   'uint64' accepts: 123, 0x123, 0XaBC
	*   'int32' accepts: 123, -123, 0x123, 0XaBC
	*/
	bool isConversionError;
	double getNumAsDouble();
	double getNumAsDouble(int32 _PosStart, int32 _PosEnd);
	uint64 getNumAsUint64();
	uint64 getNumAsUint64(int32 _PosStart, int32 _PosEnd);
	int32 getNumAsInt32();
	int32 getNumAsInt32(int32 _PosStart, int32 _PosEnd);

	/** getMonth. 
	*	The month will be determined from abbreviated names
	*	Jan=1, Feb=2, ... , Dec=12 or 0 will be returned 
	*/
	int32 getMonthAsInt32();
	int32 getMonthAsInt32(int32 _PosStart, int32 _PosEnd);

public:
	//Parse current string and return a list of substrings accordingly to patterns
	//defined by a regular expression. The following expressions are recognized so far:
	// characters:
	//     . - any character
	//    \s - space character (\t,\r,\n)
	//    \d - digit character 0..9
	//    \w - word character _a..zA..Z
	//    others - will be treated as the particular character, e.g. a is a and ! is !
    // modifiers:
	//	   * - any number of occurrencies from 0..MAX_UNSIGNED_INT
	//     + - at least one up to any number 1..MAX_UNSIGNED_INT
	//     ? - non-mandatory occurrence (zero or one times)
	//    () - enclose components to be extracted
	uint32 Match(CmLString**, int8 const*);

public:
	/** read/write
	*   The binary content of a CmString will be restored from file respectively
	*   it will be written to file.
	*/
	const int8* readBinary(const int8* _Filename);
	bool writeBinary(const int8* _Filename);
	bool appendBinary(const int8* _Filename);
	bool isFileExists(const int8* _Filename = NULL);
	bool isFolderExists(const int8* _FolderPath = NULL);
	bool addFilesOnPath(CmLString& _Files, const int8* _Path = NULL);
	bool getFoldersOnPath(CmLString& _Folders, const int8* _Path = NULL);
	bool allFiles(CmString& _File, const CmString& _Path);
	bool allLines(CmString& _Line, uint32 _Offset=0);

	/** allocate/releaseMemory.
	*  A bookkeeping is done whenever memory is allocated or released.
	*/
	template <typename T>
	static T* allocateMemory(int32 _ItemCount = 1, bool _isType = false)
	{
		// check for zero items
		if (0 == _ItemCount) return NULL;
		// init critical section
		if (false == isInitMemory){
			InitializeCriticalSection(&MemoryAccess);
			isInitMemory = true;
		}
		// bookkeeping
		EnterCriticalSection(&MemoryAccess);
		ItemsAllocated++;
		MemoryAllocated += sizeof(T) * (_ItemCount > 1 ? _ItemCount : 1);
		_isType ? ItemsOfTypeAllocated += (_ItemCount > 1 ? _ItemCount : 1 ): 0;
	  // memory allocation
		T* Item = _ItemCount > 1 ? new T[_ItemCount] : new T;
		LeaveCriticalSection(&MemoryAccess);

		return Item;
	}
	template <typename T>
	static void releaseMemory(T*& _Item, int32 _ItemCount = 1, bool _isType =false)
	{
		// check for NULL pointer
		if (NULL == _Item) return;
		// init critical section
		if (false == isInitMemory){
			InitializeCriticalSection(&MemoryAccess);
			isInitMemory = true;
		}
		// bookkeeping
		EnterCriticalSection(&MemoryAccess);
		ItemsReleased++;
		MemoryReleased += sizeof(T) * (_ItemCount > 1 ? _ItemCount : 1);
		_isType ? ItemsOfTypeReleased += (_ItemCount > 1 ? _ItemCount : 1) : 0;
		// memory deallocation
		// ToDo: fix memory deallocation problem
		// NOTE: This will fail if thread context has changed!
		_ItemCount > 1 ? delete[] _Item : delete _Item;
		_Item = NULL;
		LeaveCriticalSection(&MemoryAccess);
		// release critical section when all memory has been deallocated
		if (isInitMemory && 0 == MemoryAllocated){
			DeleteCriticalSection(&MemoryAccess);
			isInitMemory = false;
		}
	}
	static CmString getMemoryState(bool _isClearType = false);
	static bool clearMemoryState();

	friend	class CmLString;
	friend	class CmMString;
	friend	class CmStringFTL;

protected:
	// full content fields (allocated if not NULL)
	int8*  pText;		  //char field
	int16* pUnicode;  // Unicode conversion
	size_t  Length;		//string length

	// substring descriptors for 'active/highlight'
	int32 LineNumber;
	uint32 LineStart;
	uint32 ActiveStart;
	uint32 ActiveLength;
	uint32 HighlightStart;
	uint32 HighlightLength;

	// index for all files in a folder
	HANDLE hFile;
	WIN32_FIND_DATAA FindFileData;
};

//CmLString represents a list of MStrings
//
// NOTE: Implementation has been done for a LOW number of strings in the list.
//       In case of a huge amount of strings the implementation should be 
//       improved by more performant algorithms.
//
class CmLString : public CmString  
{
public:
	// constructor and destructor
	CmLString();
	virtual ~CmLString();

	//-----access-functions-------------------------------------------------------

	/** addString
	*   A new element will be added to the list of strings
	*/
	bool addString(const CmString& _String);

	/** clearList
	*  All subsequent elements of the list will be removed except current element
	*/
	bool clearList();

	/** getSize.
	*  The number of string in the list will be returned.
	*/
	int32 getSize();

	/** getString.
	*  Return Nth element of the string list, starting with index 0. If the index 
	*  exceeds the list size then the last element will be returned.
	*/
	CmString& operator[](int32 _Index);

	/** getLowestString.
	*  The alphabetically lowest string will be returned
	*/
	CmString& getLowestString();

	/** getMatchingString.
	*  A string that matches given pattern will be returned.
	*/
	bool getMatchingString(CmString& _MatchingString, const CmString& _Pattern);

	//-----functions-for-maintaining-a-list-of-strings----------------------------

	friend CmString;

private:
	CmLString** getAdrNext();
	CmLString* getNext();
	CmString* setNext(CmLString*);

private:
	CmLString* pNext;
};


//CmMString performs string matching
class CmMString : public CmString  
{
public:
	// Isolate a substring that is delimited by one of a given set 
	//  of delimiter characters starting from an indexed position
	void			resetPosition();
	int8*			getNextDelimitedString();  
	int8*			setDelimiters(int8* _Delimiters);
	const CmString*	setSourceString(const CmString* _SourceString);
	// Constructor and Destructor
	CmMString(const CmString* SourceString,const int8* _Delimiters =NULL);
	CmMString();
	virtual ~CmMString();

private:
	const CmString*	pSourceString;
	CmString		mSetOfDelimiters;
	int32			CurrentPosition;
};

/** CmUURI
 *  The design of Cosmos software relies on universally unique resource
 *  identifiers (UURI). A UURI is considered to be on top of any information.
 *  That concept allows for addressing ANY information in a consistent way.
 *  Actually, it also allows for a unification of information, addressing,
 *  Locals and the like by a single consistent data representation concept.
 *
 *  There are other known concepts like GUID (globally unique identifier, like
 *  e.g. 936DA01F-950-4D9D-80C7-02AF85C822A8) or Internet domain names
 *  (e.g. "wegalink.eu") which may serve as a UURI. However, the concept of a
 *  UURI is more general and therefore it will be defined as a bit field of 
 *  arbitrary length.
 *
 *  Further, the following scheme extends existing identifiers in a way, where
 *  an operator, a location and a subject is combined and promises to be as well a
 *  good candidate for being 'universally' unique.
 *
 *  The proposed UURI will be generated in the following way:
 *
 *  <operator>@<locator>_<city>.<subject>
 *             |--<location>--|
 *
 *  The UURI components are defined/described as follows:
 *
 *    <operator> - a person's name or shortcut, e.g. 'Eckhard.Kantz' or 'EKD'
 *    <locator>  - a shortcut Local to a geographical location, e.g. JN58nc
 *                 see also: http://de.wikipedia.org/wiki/QTH-Locator
 *    <city>     - a city or another readable location name, e.g. 'Türkenfeld'
 *    <subject>  - any subject designator, e.g. 'Cosmos' for this software
 *
 *    Note: <locator>_<city> will become the <location> part of a UURI.
 *
 *  A resulting complete sample UURI would be the following string:
 *
 *    'EKD@JN58nc_Türkenfeld.Cosmos'
 */
#define CM_UURI_ROOT_COSMOS     "EKD@JN58nc_Türkenfeld.Cosmos"
#define CM_UURI_ROOT_ANONYMOUS  "anonymous@Earth.UURI"
#define CM_UURI_DEFAULT         "Default"
class CmUURI
{
public:
  CmUURI(const char *_UURI =NULL, const char *_RootUURI =NULL);
	CmUURI(const CmString& _UURI);
  ~CmUURI();

	/** isUURI
	*   Evaluate a string whether it may represent an UURI.
	*/
	static bool isUURI(const int8 * _UURI);
	static bool isUURI(CmString& _UURI);


  /** setUURI.
   *  A UURI will be generated by concatenating a RootUURI and a UURI string.
   *  Usually, the user of this function should always provide for a valid UURI.
   *  However, if the UURI string is missing (NULL), then a 'Default' UURI will
   *  be generated. If on the other hand the RootUURI is missing (NULL) then
   *  'anonymous@Earth.UURI' will be used.
   */
	bool setUURI(const char *_UURI = NULL, const char *_RootUURI = NULL);
	bool setUURI(const CmString& _UURI);
	bool setUURI(CmString& _UURI, int32 _PosEnd, int32 _PosStart=0);
	bool setUURI(const CmUURI& _UURI);
	bool setUURI(const CmString& _Operator, const CmString& _Locator, const CmString& _Site, const CmString& _Subject);

	/** setSubject */
	bool setSubject(const CmString& _Subject);

	/** getOperator/Locator/Location/Subject/Config
	*   The operator, locator, location or subject (config) part of a RootUURI will be returned.
	*/
	CmString getOperator();
	CmString getLocation();
	CmString getLocator();
	CmString getSite();
	CmString getSubject();
	CmString getConfig();

  /** operator=/+=
   *  A UURI string will be assigned and accepted without any changes,
   *  respectively a string will be appended.
   */
	void operator=(const CmUURI& _UURI);
	void operator=(const char *_UURI);
	void operator+=(const char *_UURI);

  /** operator==/!=
   *  A UURI will be compared to another UURI.
   */
	bool operator==(const CmUURI& _UURI) const;
	bool operator==(const char *_UURI) const;
	bool operator!=(const CmUURI& _UURI)const;
	bool operator!=(const char *_UURI) const;
	bool operator<=(const CmUURI& _UURI) const;
	bool operator<=(const char *_UURI) const;
	bool operator>=(const CmUURI& _UURI) const;
	bool operator>=(const char *_UURI) const;

  /** getString/Text.
   *  A UURI is made accessable as a constant string or text.
   */
  const CmString& getString() const;
	const char * getText() const;
	const int16* getUnicode();

private:
  // A string representing actual UURI
  CmString UURI;        
};

/** CmVector
* A vector of values will be maintained
*/
template <typename T>
class CmVector
{
public:
	/** CmVector.
	*  @_InitialValue: all elements will be initially set to that value
	*  @_Length: the length and size will be set to exactly that value
	*  @_SizeExtension: factor for additional space when extending size
	*                   NOTE: 1.6 has been found optimal for automatically
	*                   extending the vector by incrementing index by 1. 
	*/
	CmVector(int32 _Length = 0, double _SizeExtension = 1.6){
		// initialize workspace
		Vector = NULL;
		// validate length
		_Length < 0 ? _Length = 0 : 0;
		Length = _Length;
		Size = _Length;
		SizeExtension = _SizeExtension;
		NextIndex = 0;
		Scalar = T(0);
		// create and initialize a vector of T values 
		if (_Length > 0){
			Vector = CmString::allocateMemory<T>(Size, isCmVector);
			for (int32 i = 0; i < _Length; i++){
				Vector[i] = T(0);
			}
		}
		}
	virtual ~CmVector(){
		if (NULL != Vector){
			CmString::releaseMemory<T>(Vector, Size, isCmVector);
		}
	}

public:
	/** set/getLength/getMaxIndex.
	*  Vector's current length will be adjusted/returned.
	*/
	int32 setLength(int32 _NewLength){
		// validate new length
		_NewLength < 0 ? _NewLength = Length : 0;
		// handle several cases
		if (_NewLength > Length && _NewLength <= Size){
			// new length fits inside current size
			Length = _NewLength;
		}
		else if (_NewLength > Length && _NewLength > Size){
			// extend size
			Length = adjustSize(_NewLength);
		}
		else if (_NewLength < Length){
			// reduce length and force a size adjustment
			Length = adjustSize(_NewLength);
		}

		// reset next index
		NextIndex = 0;

		return Length;
	}
	int32 getLength(){
		return Length;
	}
	int32 getMaxIndex(){
		return Length - 1;
	}

public:
	/** getData. A pointer to the data field will be returned
	*/
	T* getData(){
		return Vector;
	}

public:
	/** set/getScalar.
	*  A scalar value will be stored/retrieved.
	*/
	void setScalar(T _Scalar){
		Scalar = _Scalar;
	}
	const T& getScalar(){
		return Scalar;
	}

public:
	/** operator[].
	*  A vector element will be returned according to specified index.
	*  If the index exceeds the range then the vector will be extended to
	*  contain that number of elements
	*/
	T& operator[](int32 _Index){
		// evaluate index
		if (Size < _Index + 1){
			// extend size
			Length = adjustSize(_Index + 1);
		}
		// adjust length if it is lower than index+1
		if (Length < _Index + 1){
			Length = _Index + 1;
		}
		// navigation pointer
		NextIndex = _Index + 1;
		// return addressed vector element
		return Vector[_Index];
	}

public:
	/** operator=. */
	int32 operator=(CmVector& _Vector){
		// make this vector same size 
		setLength(_Vector.Length);
		// copy all elements
		for (int i = 0; i < Length; i++){
			operator[](i) = _Vector[i];
		}
		// copy scalar
		setScalar(_Vector.getScalar());
		// return length
		return Length;
	}

public:
	/** operator==. */
	bool operator==(CmVector& _Vector){
		// compare Length
		if (Length != _Vector.Length) 
			return false;
		// compare all elements
		for (int i = 0; i < Length; i++){
			if (operator[](i) != _Vector[i]) 
				return false;
		}
		// compare scalar
		if (getScalar() != _Vector.getScalar()) return false;
		// return length
		return true;
	}
	/** operator==. */
	bool operator!=(CmVector& _Vector){
		if (operator==(_Vector)) return false;
		return true;
	}
public:
	/** setValue.
	*  Convenience functions for setting multiple vector items in one go.
	*/
	CmVector<T>& clear(){
		setLength(0);
		return *this;
	}
	CmVector<T>& operator()(int32 a){ set(a); return *this; }
	CmVector<T>& set(int32 a){
		setLength(1);
		Vector[0] = a;
		return *this;
	}
	CmVector<T>& operator()(int32 a, int32 b){ set(a, b); return *this; }
	CmVector<T>& set(int32 a, int32 b){
		setLength(2);
		Vector[0] = a; Vector[1] = b;
		return *this;
	}
	CmVector<T>& operator()(int32 a, int32 b, int32 c){ set(a, b, c); return *this; }
	CmVector<T>& set(int32 a, int32 b, int32 c){
		setLength(3);
		Vector[0] = a; Vector[1] = b; Vector[2] = c;
		return *this;
	}
	CmVector<T>& operator()(int32 a, int32 b, int32 c, int32 d){ set(a, b, c, d); return *this; }
	CmVector<T>& set(int32 a, int32 b, int32 c, int32 d){
		setLength(4);
		Vector[0] = a; Vector[1] = b; Vector[2] = c; Vector[3] = d;
		return *this;
	}
	CmVector<T>& operator()(int32 a, int32 b, int32 c, int32 d, int32 e){ set(a, b, c, d, e); return *this; }
	CmVector<T>& set(int32 a, int32 b, int32 c, int32 d, int32 e){
		setLength(5);
		Vector[0] = a; Vector[1] = b; Vector[2] = c; Vector[3] = d; Vector[4] = e;
		return *this;
	}
	CmVector<T>& operator()(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f){ set(a, b, c, d, e, f); return *this; }
	CmVector<T>& set(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f){
		setLength(6);
		Vector[0] = a; Vector[1] = b; Vector[2] = c; Vector[3] = d; Vector[4] = e; Vector[5] = f;
		return *this;
	}
	CmVector<T>& operator()(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g){ set(a, b, c, d, e, f, g); return *this; }
	CmVector<T>& set(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g){
		setLength(7);
		Vector[0] = a; Vector[1] = b; Vector[2] = c; Vector[3] = d; Vector[4] = e; Vector[5] = f; Vector[6] = g;
		return *this;
	}
	CmVector<T>& operator()(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g, int32 h){ set(a, b, c, d, e, f, g, h); return *this; }
	CmVector<T>& set(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g, int32 h){
		setLength(8);
		Vector[0] = a; Vector[1] = b; Vector[2] = c; Vector[3] = d; Vector[4] = e; Vector[5] = f; Vector[6] = g; Vector[7] = h;
		return *this;
	}
	CmVector<T>& operator()(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g, int32 h, int32 i){ set(a, b, c, d, e, f, g, h, i); return *this; }
	CmVector<T>& set(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g, int32 h, int32 i){
		setLength(9);
		Vector[0] = a; Vector[1] = b; Vector[2] = c; Vector[3] = d; Vector[4] = e; Vector[5] = f; Vector[6] = g; Vector[7] = h; Vector[8] = i;
		return *this;
	}
	CmVector<T>& operator()(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g, int32 h, int32 i, int32 j){ set(a, b, c, d, e, f, g, h, i, j); return *this; }
	CmVector<T>& set(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g, int32 h, int32 i, int32 j){
		setLength(10);
		Vector[0] = a; Vector[1] = b; Vector[2] = c; Vector[3] = d; Vector[4] = e; Vector[5] = f; Vector[6] = g; Vector[7] = h; Vector[8] = i; Vector[9] = j;
		return *this;
	}
	CmVector<T>& operator()(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g, int32 h, int32 i, int32 j, int32 k){ set(a, b, c, d, e, f, g, h, i, j, k); return *this; }
	CmVector<T>& set(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g, int32 h, int32 i, int32 j, int32 k){
		setLength(11);
		Vector[0] = a; Vector[1] = b; Vector[2] = c; Vector[3] = d; Vector[4] = e; Vector[5] = f; Vector[6] = g; Vector[7] = h; Vector[8] = i; Vector[9] = j; Vector[10] = k;
		return *this;
	}
	CmVector<T>& operator()(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g, int32 h, int32 i, int32 j, int32 k, int32 l){ set(a, b, c, d, e, f, g, h, i, j, k, l); return *this; }
	CmVector<T>& set(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g, int32 h, int32 i, int32 j, int32 k, int32 l){
		setLength(12);
		Vector[0] = a; Vector[1] = b; Vector[2] = c; Vector[3] = d; Vector[4] = e; Vector[5] = f; Vector[6] = g; Vector[7] = h; Vector[8] = i; Vector[9] = j; Vector[10] = k; Vector[11] = l;
		return *this;
	}
	CmVector<T>& operator()(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g, int32 h, int32 i, int32 j, int32 k, int32 l, int32 m){ set(a, b, c, d, e, f, g, h, i, j, k, l, m); return *this; }
	CmVector<T>& set(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g, int32 h, int32 i, int32 j, int32 k, int32 l, int32 m){
		setLength(13);
		Vector[0] = a; Vector[1] = b; Vector[2] = c; Vector[3] = d; Vector[4] = e; Vector[5] = f; Vector[6] = g; Vector[7] = h; Vector[8] = i; Vector[9] = j; Vector[10] = k; Vector[11] = l; Vector[12] = m;
		return *this;
	}
	CmVector<T>& operator()(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g, int32 h, int32 i, int32 j, int32 k, int32 l, int32 m, int32 n){ set(a, b, c, d, e, f, g, h, i, j, k, l, m, n); return *this; }
	CmVector<T>& set(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g, int32 h, int32 i, int32 j, int32 k, int32 l, int32 m, int32 n){
		setLength(14);
		Vector[0] = a; Vector[1] = b; Vector[2] = c; Vector[3] = d; Vector[4] = e; Vector[5] = f; Vector[6] = g; Vector[7] = h; Vector[8] = i; Vector[9] = j; Vector[10] = k; Vector[11] = l; Vector[12] = m; Vector[13] = n;
		return *this;
	}
	CmVector<T>& operator()(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g, int32 h, int32 i, int32 j, int32 k, int32 l, int32 m, int32 n, int32 o){ set(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o); return *this; }
	CmVector<T>& set(int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, int32 g, int32 h, int32 i, int32 j, int32 k, int32 l, int32 m, int32 n, int32 o){
		setLength(15);
		Vector[0] = a; Vector[1] = b; Vector[2] = c; Vector[3] = d; Vector[4] = e; Vector[5] = f; Vector[6] = g; Vector[7] = h; Vector[8] = i; Vector[9] = j; Vector[10] = k; Vector[11] = l; Vector[12] = m; Vector[13] = n; Vector[14] = o;
		return *this;
	}

public:
	/** getNextIndex.
	*  The next position related to most recent access will be returned.
	*/
	int32 getNextIndex(){
		return NextIndex;
	}
	/** setNextIndex/reset.
	*  The next position will be set/reset.
	*/
	void setNextIndex(int32 _NextIndex){
		// evaluate next index against length
		NextIndex = _NextIndex < Length ? _NextIndex : Length - 1;
	}
	CmVector<T>& reset(){
		NextIndex = 0;
		return *this;
	}

public:
	/** allItems.
	*  The next item after most recently returned item will be returned.
	*/
	bool allItems(T& _Item){
		bool isValidIndex = NextIndex < Length;
		isValidIndex ? _Item = operator[](NextIndex) : 0;
		return isValidIndex;
	}

private:
	int32 adjustSize(int32 _NewLength){
		// validate new length
		_NewLength < 0 ? _NewLength = (int32)(Size / SizeExtension) : 0;
		// provide for extended/reduced size
		int32 NewSize = (int32)(_NewLength * SizeExtension);
		// extend to next 4k size
		const int32 Size4k = 4096 / sizeof(T);
		NewSize = Size4k * (1 + (NewSize - 1) / Size4k);

		// extend vector
		T* OldVector = Vector;
		NewSize > 0 ? Vector = CmString::allocateMemory<T>(NewSize, isCmVector) : Vector = NULL;

		for (int32 i = 0; i < NewSize; i++){
			Vector[i] = i < Length && i < _NewLength ? OldVector[i] : (T)0;
		}
		if (NULL != OldVector){
			CmString::releaseMemory<T>(OldVector, Size, isCmVector);
		}
		Size = NewSize;

		return _NewLength;
	}

	//--------workspace-----------------------------------------------------------

private:
	// vector state
	T* Vector;
	int32 Length;
	int32 Size;
	double SizeExtension;
	// navigation
	int32 NextIndex;
	// scalar 
	T Scalar;
};
typedef CmVector<int32> CmIndex;

// 2018-09-19/ToDo: fix access problems to member variables
typedef CmVector<CmPoint2D> CmPolygon;


// memory bookkeeping
#ifdef MEMORY_BOOKKEEPING
int64 ItemsAllocated = 0;
int64 ItemsReleased = 0;
int64 ItemsOccupied = 0;
int64 MemoryAllocated = 0;
int64 MemoryReleased = 0;
int64 MemoryOccupied = 0;
int64 ItemsOfTypeAllocated = 0;
int64 ItemsOfTypeReleased = 0;
CRITICAL_SECTION MemoryAccess;
bool isInitMemory = false;
bool isFocus = false;	// for analyzing a set of types
bool isInt8 = false;
bool isUint8 = false;
bool isInt16 = false;
bool isUint16 = false;
bool isDouble = false;
bool isNaviX = false;
bool isCmString = false;
bool isCmLString = false;
bool isCmStringFTL = false;
bool isCmStringFTLChild = false;
bool isCmServiceConnection = false;
bool isCmException = false;
bool isCmMatrixFTL = false;
bool isCmMatrix = false;
bool isCmVector = false;
#else
extern int64 ItemsAllocated;
extern int64 ItemsReleased;
extern int64 ItemsOccupied;
extern int64 MemoryAllocated;
extern int64 MemoryReleased;
extern int64 MemoryOccupied;
extern int64 ItemsOfTypeAllocated;
extern int64 ItemsOfTypeReleased;
extern CRITICAL_SECTION MemoryAccess;
extern bool isInitMemory;
extern bool isFocus;
extern bool isInt8;
extern bool isUint8;
extern bool isInt16;
extern bool isUint16;
extern bool isDouble;
extern bool isNaviX;
extern bool isCmString;
extern bool isCmLString;
extern bool isCmStringFTL;
extern bool isCmStringFTLChild;
extern bool isCmServiceConnection;
extern bool isCmException;
extern bool isCmMatrixFTL;
extern bool isCmMatrix;
extern bool isCmVector;
#endif

}  // namespace Cosmos

using namespace Cosmos;

#endif // ifndef CmStringH
