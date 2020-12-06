//////////////////////////////////////////////////////////////////////////////
//
// CmString.cpp: Implementation of CmString classes
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

#define MEMORY_BOOKKEEPING
#define CM_TYPE_OLD_STYLE
#include "Services/Basic/CmString.h"

//////////////////////////////////////////////////////////////////////
// CmUURI Class
//////////////////////////////////////////////////////////////////////
CmUURI::CmUURI(const char *_UURI, const char *_RootUURI)
{
  // Initialize the UURI string
  setUURI(_UURI,_RootUURI);

}
CmUURI::CmUURI(const CmString& _UURI)
{
	setUURI(NULL, _UURI.getText());
}
CmUURI::~CmUURI()
{

}

bool CmUURI::isUURI(const int8 * _UURI)
{
	CmString UURI(_UURI);
	return NULL == _UURI ? true : isUURI(UURI);
}
bool CmUURI::isUURI(CmString& _UURI)
{
	// one char '@' is mandatory
	uint32 PosAt = _UURI.findPosition('@');
	if (PosAt >= _UURI.getLength()) return false;

	// the '@' char must not be escaped
	if (_UURI[PosAt-1] == '\\') return false;

	// only one '@' char allowed
	if (uint32(_UURI.findPosition('@', PosAt+1)) < _UURI.getLength()) return false;

	return true;
}

bool CmUURI::setUURI(const char *_UURI, const char *_RootUURI)
{
	// Generate a UURI string
	// NOTE: The user of this function should always specify a valid UURI!
	CmString NewUURI;
	if (NULL != _UURI){
		NewUURI = _UURI;
	}	else{
		// Unknown UURI: assign a default UURI (deprecated, although accepted)
		//NewUURI = CM_UURI_DEFAULT;
	}
  // Assign a root UURI
  // NOTE: The user of this function should always specify a valid RootUURI!
	if (NULL != _RootUURI){
		UURI = _RootUURI;
	}	else{
		// Unknown root UURI: use 'anonymous@Earth.UURI' (deprecated, although accepted)
		UURI = CM_UURI_ROOT_ANONYMOUS;
	}

  // Take care for having a single delimiter between root and further UURI string
  if( UURI[(int32)(UURI.getLength())-1] == '/' && NewUURI[0] == '/' )
  {
    // remove root string's trailing delimiter (the slash)
    CmString NewRootUURI;
    NewRootUURI.assignSubString((int32)(UURI.getLength()-2),&UURI);
    UURI = NewRootUURI;
  }
  else
  if (UURI.getLength() > 0 &&  UURI[(int32)(UURI.getLength()) - 1] != '/' && NewUURI[0] != '/')
  {
    // extend root string with a delimiter (a slash)
		if (NewUURI.getLength() > 0){
			UURI += "/";
		}
  }

  // Save combined UURI assembled from a root UURI and from a UURI string
  UURI += NewUURI;

	return true;
}

bool CmUURI::setUURI(const CmString& _UURI)
{
	// Accept new UURI string
	UURI = _UURI;

	return true;
}

bool CmUURI::setUURI(CmString& _UURI, int32 _PosEnd, int32 _PosStart)
{
	// Accept specified UURI string
	UURI.assignSubString(_PosEnd, &_UURI, _PosStart);

	return true;
}

bool CmUURI::setUURI(const CmUURI& _UURI)
{
	// Accept new UURI
	UURI = _UURI.getString();

	return true;
}

bool CmUURI::setUURI(const CmString& _Operator, const CmString& _Locator, const CmString& _Site, const CmString& _Subject)
{
	UURI = _Operator;
	UURI += "@";
	UURI += _Locator;
	UURI += "_";
	UURI += _Site;
	UURI += ".";
	UURI += _Subject;

	return true;
}

bool CmUURI::setSubject(const CmString& _Subject)
{
	// replace subject in the UURI identifier string, preserve remaining components
	return setUURI(getOperator(), getLocator(), getSite(), _Subject);
}

CmString CmUURI::getOperator()
{
	// find delimiter position
	int At = UURI.findPosition('@');
	// copy substring
	CmString Operator;
	Operator.assignSubString(At-1, &UURI);

	return Operator;
}

CmString CmUURI::getLocation()
{
	// find delimiter positions
	int At = UURI.findPosition('@');
	int Dot = UURI.findPosition('.', At);
	// copy substring
	CmString Location;
	Location.assignSubString(Dot - 1, &UURI, At + 1);

	return Location;
}

CmString CmUURI::getLocator()
{
	// find delimiter positions
	int At = UURI.findPosition('@');
	int Dash = UURI.findPosition('_', At);
	// copy substring
	CmString Locator;
	Locator.assignSubString(Dash - 1, &UURI, At + 1);

	return Locator;
}

CmString CmUURI::getSite()
{
	// find delimiter positions
	int At = UURI.findPosition('@');
	int Dash = UURI.findPosition('_', At);
	int Dot = UURI.findPosition('.', Dash);
	// copy substring
	CmString Site;
	Site.assignSubString(Dot - 1, &UURI, Dash + 1);

	return Site;
}


CmString CmUURI::getSubject()
{
	// find delimiter position
	int At = UURI.findPosition('@');
	int Dot = UURI.findPosition('.', At);
	int Slash = UURI.findPosition('/');
	// copy substring
	CmString Subject;
	Subject.assignSubString(Slash-1, &UURI, Dot+1);

	return Subject;
}

CmString CmUURI::getConfig()
{
	// find delimiter position
	int At = UURI.findPosition('@');
	int Dot = UURI.findPosition('.', At);
	// copy substring
	CmString Config;
	Config.assignSubString(UURI.getLength(), &UURI, Dot + 1);
	// replace slash '/' by underscore '_' 
	int32 PosSlash = 0;
	while ((PosSlash = Config.findPosition('/', PosSlash)) < int32(Config.getLength())){
		Config.setAt(PosSlash, '_');
	}

	return Config;
}

void CmUURI::operator=(const CmUURI& _UURI)
{
	UURI = _UURI.getString();
}

void CmUURI::operator=(const char *_newUURI)
{
  // Assign a string as new UURI without any changes 
  UURI = _newUURI;
}

void CmUURI::operator+=(const char *_UURI)
{
  // Append a string to existing UURI
  this->UURI += _UURI;
}

bool CmUURI::operator==(const CmUURI& _UURI) const
{
  // Check if this UURI's string is equal to another UURI's string
  return this->UURI == _UURI.getString();
}

bool CmUURI::operator==(const char *_UURI) const
{
	// Check if this UURI's string is equal to a given string
	return this->UURI == CmString(_UURI);
}

bool CmUURI::operator!=(const CmUURI& _UURI) const
{
	// Check if this UURI's string is unequal to another UURI's string
	return this->UURI != _UURI.getString();
}

bool CmUURI::operator!=(const char *_UURI) const
{
	// Check if this UURI's string is unequal to a given string
	return this->UURI != CmString(_UURI);
}

bool CmUURI::operator<=(const CmUURI& _UURI) const
{
	// Check if this UURI's string is unequal to another UURI's string
	return this->UURI <= _UURI.getString();
}

bool CmUURI::operator<=(const char *_UURI) const
{
	// Check if this UURI's string is unequal to a given string
	return this->UURI <= CmString(_UURI);
}

bool CmUURI::operator>=(const CmUURI& _UURI) const
{
	// Check if this UURI's string is unequal to another UURI's string
	return this->UURI >= _UURI.getString();
}

bool CmUURI::operator>=(const char *_UURI) const
{
	// Check if this UURI's string is unequal to a given string
	return this->UURI >= CmString(_UURI);
}

const char * CmUURI::getText() const
{
  return UURI.getText();
}

const int16* CmUURI::getUnicode()
{
	return UURI.getUnicode();
}

const CmString& CmUURI::getString() const
{
  return UURI;
}



//////////////////////////////////////////////////////////////////////
// CmString Class
//////////////////////////////////////////////////////////////////////

CmString::CmString()
{
	initWorkspace();
}

CmString::CmString(const int8* _Text)
{
	initWorkspace();
	setText(_Text);
}

CmString::CmString(const int8* _Text, uint32 _Length)
{
	initWorkspace();

	// Generate a text array of given buffer length
	Length = _Length;
	pText = allocateMemory<int8>(int32(Length + 1), isInt8);

  // Copy text to the array and terminate with a '0'
	memcpy(pText,_Text,_Length);
  pText[Length] = 0;
}

CmString::CmString(uint32 _Length, uint8 _Init)
{
	initWorkspace();

	//Generate empty text array
	Length = _Length;
	pText = allocateMemory<int8>(int32(Length + 1), isInt8);
	memset(pText, _Init, Length + 1);
}

CmString::CmString(const CmString & _String)
{
	initWorkspace();
	setText(_String.getText());
}

CmString::CmString(CmVector<uint8>& _VectorUInt8)
{
	initWorkspace();

	//Generate text array
	Length = _VectorUInt8.getLength();
	pText = allocateMemory<int8>(int32(Length + 1), isInt8);
	memcpy(pText, _VectorUInt8.getData(), Length);
	pText[Length] = 0;
}


CmString::~CmString()
{
	//Remove text/Unicode arrays from heap
	if (NULL != pText){
		releaseMemory<int8>(pText, int32(Length + 1), isInt8);
	}
	if (NULL != pUnicode){
		releaseMemory<int16>(pUnicode, int32(Length + 1), isInt16);
	}
}

void CmString::initWorkspace()
{
	//Reset text array and length
	pText = NULL;
	pUnicode = NULL;
	Length = 0;
	// initialize descriptor variables
	LineNumber = 0;
	ActiveStart = 0;
	ActiveLength = 0;
	HighlightStart = 0;
	HighlightLength = 0;
	// initialize file search
	hFile = INVALID_HANDLE_VALUE;
}


// unit test
bool CmString::testCmString()
{
	// set/get functions for basic types
	uint8 Val8 = 0x12;
	uint16 Val16 = 0x1234;
	uint32 Val32 = 0x12345678;
	uint64 Val64 = 0x0123456789ABCDEF;

	CmString Str8(2 * sizeof(uint8));
	CmString Str16(2 * sizeof(uint16));
	CmString Str32(2 * sizeof(uint32));
	CmString Str64(2 * sizeof(uint64));

	uint8 TestVal8;
	uint16 TestVal16;
	uint32 TestVal32;
	uint64 TestVal64;

	Str8.setAt(sizeof(uint8), Val8);
	Str16.setAt(sizeof(uint16), Val16);
	Str32.setAt(sizeof(uint32), Val32);
	Str64.setAt(sizeof(uint64), Val64);

	Str8.getAt(sizeof(uint8), TestVal8);
	Str16.getAt(sizeof(uint16), TestVal16);
	Str32.getAt(sizeof(uint32), TestVal32);
	Str64.getAt(sizeof(uint64), TestVal64);

	if (TestVal8 != Val8) return false;
	if (TestVal16 != Val16) return false;
	if (TestVal32 != Val32) return false;
	if (TestVal64 != Val64) return false;

	// conversion functions
	CmString Num;
	CmString NumNeg;
	uint64 Val;
	uint64 Val5;
	int32 Val5Neg;
	// uint32
	Num = "1234567890";
	NumNeg = "-1234567890";
	Val = 1234567890;
	Val5 = 123456;
	Val5Neg = -12345;
	if (Val != Num.getNumAsUint64()) return false;
	if (Val5 != Num.getNumAsUint64(0, 5)) return false;
	if (Val != Num.getNumAsInt32()) return false;
	if (Val5 != Num.getNumAsInt32(0, 5)) return false;
	if (Val5Neg != NumNeg.getNumAsInt32(0, 5)) return false;
	// uint64
	Num = "12345678901234567890";
	Val = 12345678901234567890;
	if (Val != Num.getNumAsUint64()) return false;
	// hex
	Num = "0x1234567890aBcDeF";
	Val = 0x1234567890aBcDeF;
	if (Val != Num.getNumAsUint64()) return false;
	// test wrong number formats
	Num = "1.23";
	if ((1 != Num.getNumAsUint64()) || (false == Num.isConversionError)) return false;
	Num = "0123A";
	if((123 != Num.getNumAsUint64()) || (false == Num.isConversionError)) return false;
	Num = " 123";
	if ((123 != Num.getNumAsUint64()) || (true == Num.isConversionError))	return false;
	Num = "123 ";
	if ((123 != Num.getNumAsUint64()) || (true == Num.isConversionError))	return false;
	// double: check against a maximal acceptable error 
	const double fError = 1e-15;
	double fVal;
	// double without exponent
	Num = "-9.0123";
	fVal = -9.0123;
	if (fVal != Num.getNumAsDouble()) return false;
	// double without fraction
	Num = "123456789e+12";
	fVal = 123456789e+12;
	if (fabs((fVal - Num.getNumAsDouble()) / fVal) > fError) return false;
	// double with fraction and exponent
	Num = "1.23456789e-12";
	fVal = 1.23456789e-12;
	if (fabs((fVal - Num.getNumAsDouble()) / fVal) > fError) return false;
	// test wrong number formats
	Num = "1.23456789e--12";
	if(0 != Num.getNumAsDouble())	return false;
	Num = "1.23-456789e-12";
	if (0 != Num.getNumAsDouble()) return false;
	Num = "0-1.23456789e-12";
	if(0 != Num.getNumAsDouble())	return false;
	Num = "1.23456789e1-2";
	if(0 != Num.getNumAsDouble())	return false;
	Num = "1.2345b6789e12";
	if (1.2345 != Num.getNumAsDouble() || false == Num.isConversionError)	return false;
	Num = "12345678901234567";
	if(0 != Num.getNumAsDouble())	return false;

	// month recognition
	CmString Months("Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec");
	if (1 != Months.getMonthAsInt32(0, 2)) return false;
	if (2 != Months.getMonthAsInt32(4, 6)) return false;
	if (3 != Months.getMonthAsInt32(8, 10)) return false;
	if (4 != Months.getMonthAsInt32(12, 14)) return false;
	if (5 != Months.getMonthAsInt32(16, 18)) return false;
	if (6 != Months.getMonthAsInt32(20, 22)) return false;
	if (7 != Months.getMonthAsInt32(24, 26)) return false;
	if (8 != Months.getMonthAsInt32(28, 30)) return false;
	if (9 != Months.getMonthAsInt32(32, 34)) return false;
	if (10 != Months.getMonthAsInt32(36, 38)) return false;
	if (11 != Months.getMonthAsInt32(40, 42)) return false;
	if (12 != Months.getMonthAsInt32(44, 46)) return false;

	// test UURI
	const CmString Operator("Operator.Name");
	const CmString Location("Locator_Site");
	const CmString Locator("Locator");
	const CmString Site("Site");
	const CmString Subject("Subject");
	CmUURI UURI;
	UURI.setUURI(Operator, Locator, Site, Subject);
	if (Operator != UURI.getOperator()) return false;
	if(Location != UURI.getLocation()) return false;
	if(Locator != UURI.getLocator()) return false;
	if(Site != UURI.getSite()) return false;
	if(Subject != UURI.getSubject()) return false;

	// further tests...

	return true;
}

const int8* CmString::getText() const
{
	//Return text array or empty string
	if (NULL != pText){
		return pText;
	}
	else{
		return (int8*)"";
	}
}

const uint8* CmString::getBinary() const
{
	//Return binary or empty field
	if (NULL != pText){
		return (const uint8*)pText;
	}
	else{
		return (const uint8*)"";
	}
}

const int16* CmString::getUnicode()
{
	// re-generate a Unicode array
	if (NULL != pUnicode){
		releaseMemory<int16>(pUnicode, int32(Length + 1), isInt16);
	}
	pUnicode = allocateMemory<int16>(int32(Length + 1), isInt16);
	// convert chars to Unicode
	memset(pUnicode, 0, 2 * (Length + 1));
	for (int i = 0; i < (int32)Length; i++)
		pUnicode[i] = pText[i];

	return pUnicode;
}

int8* CmString::getBuffer() const
{
    // Check length and return text array
	if (Length>0){
		return pText;
	}else{
		return "";
	}
}

bool CmString::isEmpty()
{
	return NULL == pText ? true : false;
}
uint32 CmString::getLength() const
{
  //Return text length
	return uint32(Length);
}

int8* CmString::setText(const int8* _Text)
{
	// delete previous text
	if (NULL != pText){
		releaseMemory<int8>(pText, int32(Length + 1), isInt8);
	}
	pText = NULL;
	Length = 0;

	// check pointer
	if (NULL == _Text) return pText;

	//Replace existing text array with a new string
	// ToDo: this may cause a crash when _Text is invalid (to be solved on a higher level)
	Length = (uint32)strlen(_Text);
	pText = allocateMemory<int8>(int32(Length + 1), isInt8);
	if (Length>0){
		memcpy(pText, _Text, Length);
	}
	pText[Length] = 0;
	return pText;
}

int8* CmString::setData(const uint8* _Data, size_t _Length)
{
	//Replace existing text array with a new string
	if (NULL != pText){
		releaseMemory<int8>(pText, int32(Length + 1), isInt8);
	}
	Length = _Length;
	pText = allocateMemory<int8>(int32(Length + 1), isInt8);
	if (NULL != _Data && Length > 0){
		memcpy(pText, _Data, Length);
	}
	pText[Length] = 0;
	return pText;
}

void CmString::reset()
{
	LineStart = 0;
	LineNumber = 0;
	ActiveStart = 0;
	ActiveLength = 0;
	HighlightStart = 0;
	HighlightLength = 0;
}
void CmString::clear()
{
	if (NULL != pText){
		releaseMemory<int8>(pText, int32(Length + 1), isInt8);
	}
	pText = NULL;
	Length = 0;
}
void CmString::fill(size_t _Length, uint8 _Digit)
{
	setLength(_Length);
	// fill
	if (_Length > 0){
		memset(pText, _Digit, _Length);
	}
}
void CmString::setText(const CmString & _String)
{ 
  //Replace existing text array with the content of a given CmString object
	setText(_String.getText());
}

int8* CmString::setText(const int8* _First, const int8* _Last)
{
	// delete previous text
	if (NULL != pText){
		releaseMemory<int8>(pText, int32(Length + 1), isInt8);
	}
	pText = NULL;
	Length = 0;

	// check pointers
	if ((NULL == _First) || (NULL == _Last)) return pText;

	// Replace existing text array with a given substring
  if (_First<=_Last){
	Length = (uint32)(_Last-_First+1);
  }else{
    Length = 0;
  }
  pText = allocateMemory<int8>(int32(Length + 1), isInt8);
	if (Length>0){
	  memcpy(pText, _First, Length);
  }
	pText[Length] = 0;
	return pText;
}

void CmString::setLength(size_t _Length)
{
	// clear previous text array
	if (NULL != pText){
		releaseMemory<int8>(pText, int32(Length + 1), isInt8);
	}
	//Reallocate the text array with a new size
	Length = _Length;
	pText = allocateMemory<int8>(int32(Length + 1), isInt8);
	// clear new data field
	memset(pText,0,Length+1);
}

void CmString::adjustLength(size_t _Length)
{
  // preserve old text
  size_t OldLength = Length;
  int8* pOldText = pText;
  // allocate a new text array with new size
	Length = _Length;
	pText = allocateMemory<int8>(int32(Length + 1), isInt8);
	// clear new data field
  memset(pText,0,Length+1);
	// copy old data
  if (Length <= (int32)OldLength)
    memcpy(pText, pOldText, Length);
  else
    memcpy(pText, pOldText, OldLength);
  // delete old text array
	releaseMemory<int8>(pOldText, int32(OldLength + 1), isInt8);
}

void CmString::setAt(const uint32 _Pos, int8 _Val8)
{
	// replace a char in the text array
	if (_Pos <= Length - sizeof(int8)){
		pText[_Pos] = _Val8;
	}
}
void CmString::setAt(const uint32 _Pos, uint8 _Val8)
{
	// replace an unsigned char in the text array
	if (_Pos <= Length - sizeof(uint8)){
		pText[_Pos] = _Val8;
	}
}
void CmString::setAt(const uint32 _Pos, bool _ValBool)
{
	// replace a bool value in the text array
	if (_Pos <= Length - sizeof(bool)){
		memcpy(pText + _Pos, &_ValBool, sizeof(bool));
	}
}
void CmString::setAt(const uint32 _Pos, int16 _Val16)
{
	// replace a short value in the text array
	if (_Pos <= Length - sizeof(int16)){
		memcpy(pText + _Pos, &_Val16, sizeof(int16));
	}
}
void CmString::setAt(const uint32 _Pos, uint16 _Val16)
{
	// replace an unsigned short value in the text array
	if (_Pos <= Length - sizeof(uint16)){
		memcpy(pText + _Pos, &_Val16, sizeof(uint16));
	}
}
void CmString::setAt(const uint32 _Pos, int32 _Val32)
{
	// replace an integer value in the text array
	if (_Pos <= Length - sizeof(int32)){
		memcpy(pText + _Pos, &_Val32, sizeof(int32));
	}
}
void CmString::setAt(const uint32 _Pos, uint32 _Val32)
{
	// replace an unsigned integer value in the text array
	if (_Pos <= Length - sizeof(uint32)){
		memcpy(pText + _Pos, &_Val32, sizeof(uint32));
	}
}
void CmString::setAt(const uint32 _Pos, float _ValFloat)
{
	// replace a float value in the text array
	if (_Pos <= Length - sizeof(float)){
		memcpy(pText + _Pos, &_ValFloat, sizeof(float));
	}
}
void CmString::setAt(const uint32 _Pos, int64 _Val64)
{
	// replace a long long value in the text array
	if (_Pos <= Length - sizeof(int64)){
		memcpy(pText + _Pos, &_Val64, sizeof(int64));
	}
}
void CmString::setAt(const uint32 _Pos, uint64 _Val64)
{
	// replace an unsigned long long value in the text array
	if (_Pos <= Length - sizeof(uint64)){
		memcpy(pText + _Pos, &_Val64, sizeof(uint64));
	}
}
void CmString::setAt(const uint32 _Pos, double _ValDouble)
{
	// replace a double value in the text array
	if (_Pos <= Length - sizeof(double)){
		memcpy(pText + _Pos, &_ValDouble, sizeof(double));
	}
}
void CmString::setAt(const uint32 _Pos, void* _ValPointer)
{
	// replace a pointer value in the text array
	if (_Pos <= Length - sizeof(void*)){
    memcpy(pText + _Pos, &_ValPointer, sizeof(void*));
	}
}

void CmString::getAt(const uint32 _Pos, int8& _Val8)
{
	// return a char from text array
	if (_Pos <= Length - sizeof(int8)){
		_Val8 = pText[_Pos];
	}
}
void CmString::getAt(const uint32 _Pos, uint8& _Val8)
{
	// return an unsigned char from text array
	if (_Pos <= Length - sizeof(uint8)){
		_Val8 = pText[_Pos];
	}
}
void CmString::getAt(const uint32 _Pos, bool& _ValBool)
{
	// return a bool value from text array
	if (_Pos <= Length - sizeof(bool)){
		memcpy(&_ValBool, pText + _Pos, sizeof(bool));
	}
}
void CmString::getAt(const uint32 _Pos, int16& _Val16)
{
	// return a short from text array
	if (_Pos <= Length - sizeof(int16)){
		memcpy(&_Val16, pText + _Pos, sizeof(int16));
	}
}
void CmString::getAt(const uint32 _Pos, uint16& _Val16)
{
	// return an unsigned short from text array
	if (_Pos <= Length - sizeof(uint16)){
		memcpy(&_Val16, pText + _Pos, sizeof(uint16));
	}
}
void CmString::getAt(const uint32 _Pos, int32& _Val32)
{
	// return an integer from text array
	if (_Pos <= Length - sizeof(int32)){
		memcpy(&_Val32, pText + _Pos, sizeof(int32));
	}
}
void CmString::getAt(const uint32 _Pos, uint32& _Val32)
{
	// return an unsigned integer from text array
	if (_Pos <= Length - sizeof(uint32)){
		memcpy(&_Val32, pText + _Pos, sizeof(uint32));
	}
}
void CmString::getAt(const uint32 _Pos, float& _ValFloat)
{
	// return a float from text array
	if (_Pos <= Length - sizeof(float)){
		memcpy(&_ValFloat, pText + _Pos, sizeof(float));
	}
}
void CmString::getAt(const uint32 _Pos, int64& _Val64)
{
	// return a long long from text array
	if (_Pos <= Length - sizeof(int64)){
		memcpy(&_Val64, pText + _Pos, sizeof(int64));
	}
}
void CmString::getAt(const uint32 _Pos, uint64& _Val64)
{
	// return an unsigned long long from text array
	if (_Pos <= Length - sizeof(uint64)){
		memcpy(&_Val64, pText + _Pos, sizeof(uint64));
	}
}
void CmString::getAt(const uint32 _Pos, double& _ValDouble)
{
	// return a double from text array
	if (_Pos <= Length - sizeof(uint64)){
		memcpy(&_ValDouble, pText + _Pos, sizeof(uint64));
	}
}
void CmString::getAt(const uint32 _Pos, void*& _ValPointer)
{
	// return a double from text array
	if (_Pos <= Length - sizeof(void*)){
		memcpy(&_ValPointer, pText + _Pos, sizeof(void*));
	}
}

void CmString::setValue(int8 _Val8)
{
	setLength(8);
	setAt(0, _Val8);
}
void CmString::setValue(uint8 _Val8)
{
	setLength(8);
	setAt(0, _Val8);
}
void CmString::setValue(bool _ValBool)
{
	setLength(8);
	setAt(0, _ValBool);
}
void CmString::setValue(int16 _Val16)
{
	setLength(8);
	setAt(0, _Val16);
}
void CmString::setValue(uint16 _Val16)
{
	setLength(8);
	setAt(0, _Val16);
}
void CmString::setValue(int32 _Val32)
{
	setLength(8);
	setAt(0, _Val32);
}
void CmString::setValue(uint32 _Val32)
{
	setLength(8);
	setAt(0, _Val32);
}
void CmString::setValue(float _ValFloat)
{
	setLength(8);
	setAt(0, _ValFloat);
}
void CmString::setValue(int64 _Val64)
{
	setLength(8);
	setAt(0, _Val64);
}
void CmString::setValue(uint64 _Val64)
{
	setLength(8);
	setAt(0, _Val64);
}
void CmString::setValue(double _ValDouble)
{
	setLength(8);
	setAt(0, _ValDouble);
}
void CmString::setValue(void* _ValPointer)
{
	setLength(8);
	setAt(0, _ValPointer);
}


uint32 CmString::operator[](uint32 _uPos) const
{
  //Return single character at a given position in the text array
	if (((int32)_uPos >= Length) || (NULL == pText)){
		return 0;
	}else{
		return pText[_uPos];
	}
}

void CmString::operator+(const CmString _String)
{
	operator += (_String.getBuffer());
}

void CmString::operator += (const CmString& _String)
{
	operator += (_String.getBuffer());
}

void CmString::operator += (const int8* _Text)
{
	// check string to be added
	if (NULL == _Text) return;

	//Preserve existing text array
	int8*  pOldText   = pText;
	size_t OldLength = Length;

	//Add existing and new string into the new text array
	size_t StringLength = strlen(_Text);
	Length += StringLength;
	pText = allocateMemory<int8>(int32(Length + 1), isInt8);
	pText[Length] = 0;
	if (OldLength>0){
		memcpy(pText,pOldText,OldLength);
	}
	if (Length>OldLength){
		memcpy(pText + OldLength, _Text, StringLength);
	}
	releaseMemory<int8>(pOldText, int32(OldLength + 1), isInt8);
}

void CmString::operator += (uint32 _Val32)
{
	CmString mString;
	mString = _Val32;
	operator += (mString.getBuffer());
}

void CmString::operator += (int32 _Val32)
{
	CmString mString;
	mString = _Val32;
	operator += (mString.getBuffer());
}

void CmString::operator += (uint64 _Val64)
{
	CmString mString;
	mString = _Val64;
	operator += (mString.getBuffer());
}

void CmString::operator += (int64 _Val64)
{
	CmString mString;
	mString = _Val64;
	operator += (mString.getBuffer());
}

void CmString::operator = (const CmString& _String)
{
  //Take over content from a given CmString object
	setText(_String.getText());
}

void CmString::operator = (const int8* _Text)
{
  //Take over content from a given CmString object
	setText(_Text);
}

void CmString::operator=(const int32 _Val32)
{
	char achNum[20];
	SPRINTF4(achNum, sizeof(achNum), "%d", _Val32);
	setText(achNum);
}

void CmString::operator=(const uint32 _Val32)
{
	char achNum[20];
	SPRINTF4(achNum, sizeof(achNum), "%u", _Val32);
	setText(achNum);
}

void CmString::operator=(const int64 _Val64)
{
#define NUM_BILLION 1000000000
	char achNum[40+1];
	char *achPos = achNum;
	uint64 Num = (uint64)_Val64;
	if (_Val64 < 0){
		*achPos++ = '-';
		Num = (uint64)(-_Val64);
	}
	if (Num / NUM_BILLION >= NUM_BILLION){
		SPRINTF6(achPos, sizeof(achNum)-1, "%u%09u%09u",
			(uint32)((Num / NUM_BILLION) / NUM_BILLION),
			(uint32)((Num / NUM_BILLION) % NUM_BILLION),
			(uint32)(Num%NUM_BILLION));
	}
	else if (Num >= NUM_BILLION){
		SPRINTF5(achPos, sizeof(achNum)-1, "%u%09u",
			(uint32)(Num / NUM_BILLION),
			(uint32)(Num%NUM_BILLION));
	}
	else{
		SPRINTF4(achPos, sizeof(achNum)-1, "%u", (uint32)Num);
	}
	setText(achNum);
}

void CmString::operator=(const uint64 _Val64)
{
	char achNum[40];
	if (_Val64 / NUM_BILLION >= NUM_BILLION){
		SPRINTF6(achNum,sizeof(achNum),"%u%09u%09u",
			(uint32)((_Val64 / NUM_BILLION) / NUM_BILLION),
			(uint32)((_Val64 / NUM_BILLION) % NUM_BILLION),
			(uint32)(_Val64%NUM_BILLION));
	}
	else if (_Val64 >= NUM_BILLION){
		SPRINTF5(achNum,sizeof(achNum),"%u%09u",
			(uint32)(_Val64 / NUM_BILLION),
			(uint32)(_Val64%NUM_BILLION));
	}else{
		SPRINTF4(achNum, sizeof(achNum), "%u", (uint32)_Val64);
	}
	setText(achNum);
}

bool CmString::operator==(const CmString& _String) const
{
	return operator==(_String.getText());
		}
bool CmString::operator==(const int8* _Text) const
{
	// check pointer
	if (NULL == _Text) return false;

  //Compare the text array with a given string
	if (strlen(_Text) != Length){
		return false;
	}else{
		if (0 == strcmp(_Text, pText)){
			return true;
		}else{
			return false;
		}
	}
}

bool CmString::operator!=(const CmString& _String) const
{
	return operator!=(_String.getText());
	}
bool CmString::operator!=(const int8* _Text) const
{
	// check pointer
	if (NULL == _Text) return true;

  //Compare the text array with a given string
	if (strlen(_Text) != Length){
		return true;
	}else{
		if (0 != strcmp(_Text, pText)){
			return true;
		}else{
			return false;
		}
	}
}

bool CmString::operator<(const CmString& _String) const
{
	return operator<(_String.getText());
		}
bool CmString::operator<(const int8* _Text) const
{
	// check pointer
	if (NULL == _Text) return false;

	//Compare the text array with a given string upto shortest length
	if ((Length > strlen(_Text)) || (NULL == pText)){
		return false;
	}
	else{
		if (0 < strncmp(_Text, pText, Length)){
			return true;
		}
		else{
			return false;
		}
	}
}

bool CmString::operator<=(const CmString& _String) const
{
	return operator<=(_String.getText());
	}
bool CmString::operator<=(const int8* _Text) const
{
	// check pointer
	if (NULL == _Text) return false;

	//Compare the text array with a given string upto shortest length
	if ((Length > strlen(_Text)) || (NULL == pText)){
		return false;
	}
	else{
		if (0 <= strncmp(_Text, pText, Length)){
			return true;
		}
		else{
			return false;
		}
	}
}

bool CmString::operator>(const CmString& _String) const
{
	return operator>(_String.getText());
	}
bool CmString::operator>(const int8* _Text) const
{
	// check pointer
	if (NULL == _Text) return false;

	//Compare the text array with a given string upto shortest length
	if ((Length < strlen(_Text)) || (NULL == pText)){
		return false;
	}
	else{
		if (0 > strncmp(_Text, pText, Length)){
			return true;
		}
		else{
			return false;
		}
	}
}

bool CmString::operator>=(const CmString& _String) const
{
	return operator>=(_String.getText());
}
bool CmString::operator>=(const int8* _Text) const
{
	// check pointer
	if (NULL == _Text) return false;

	//Compare the text array with a given string upto shortest length
	if ((Length < strlen(_Text)) || (NULL == pText)){
		return false;
	}
	else{
		if (0 >= strncmp(_Text, pText, Length)){
			return true;
		}
		else{
			return false;
		}
	}
}

bool CmString::isPrefix(const CmString& _String) const
{
	return isPrefix(_String.getText());
}
bool CmString::isPrefix(const int8* _Text) const
{
	// check pointer
	if (NULL == _Text) return false;

	//Compare the first characters in the text array with a given header string
	return 0 == strncmp(pText, _Text, strlen(_Text));
}

bool CmString::double2String(double _Value, int32 _Precision, int32 _Digits)
{
	char Num[64] = { "0" };
	char Format[64];
	// estimate precision for not loosing information
	if (-1 == _Precision){
		// ToDo: for other digits
		if (abs(_Value * 10 - ((int64)_Value) * 10) > 0.1){
			_Precision = 1;
		}
	}
	// limit value to a reasonable range -1e35..1e35 (smallest quantum in our universe or its inverse)
	_Value = _Value < -1e+35 ? -1e+35 : _Value > 1e+35 ? 1e+35 : _Value;
	if (-1 == _Precision && -1 == _Digits){
		sprintf_s(Num, sizeof(Num), "%.0lf", _Value);
	}	else if (-1 == _Digits){
		sprintf_s(Format, sizeof(Format), "%%.%dlf", _Precision);
		sprintf_s(Num, sizeof(Num), Format, _Value);
	}	else{
		sprintf_s(Format, sizeof(Format), "%%%d.%dlf", _Digits, _Precision);
		sprintf_s(Num, sizeof(Num), Format, _Value);
	}
	setText(Num);

	return true;
}
bool CmString::uint2String(uint64 _Value)
{
	// uint64 has maximal 20 digits
	const int32 MaxLength = 20;
	fill(MaxLength, '0');

	// evaluate all digits
	for (int32 i = MaxLength - 1; i >= 0; i--){
		getBuffer()[i] = '0' + _Value % 10;
		_Value /= 10;
		if (_Value == 0) break;
	}

	// remove leading zero
	if (getBuffer()[0] == '0'){
		int32 LenZ;
		for (LenZ = 1; LenZ < int32(getLength()); LenZ++){
			if (getBuffer()[LenZ] != '0') break;
		}
		LenZ >= int32(getLength()) ? LenZ = int32(getLength() - 1) : 0;
		assignSubString(int32(getLength() - 1), 0, LenZ);
	}

	return true;
}

CmString CmString::UInt2Hex(uint64 _Value)
{
	CmString Hex;
	Hex.uint2hex(_Value);
	return Hex;
}

bool CmString::uint2hex(uint64 _Value)
{
	char Hex[64] = {0};
	sprintf_s(Hex, sizeof(Hex), "%I64X", _Value);
	setText(Hex);

	return true;
}
uint64 CmString::hex2uint64()
{
	uint64 Value = 0;

	// cumulate all hex digits' binary values
	for (int i = 0; i < (int)getLength(); i++){
		Value *= 16;
		switch ((*this)[i]){
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': Value += (*this)[i] - '0'; break;
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f': Value += (*this)[i] - 'a' + 10; break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F': Value += (*this)[i] - 'A' + 10; break;
		case 'x':
		case 'X': break;
		default: break;
		}
	}

	return Value;
}
uint32 CmString::hex2uint32()
{
	return (uint32)hex2uint64();
}
uint16 CmString::hex2uint16()
{
	return (uint16)hex2uint64();
}
uint8 CmString::hex2uint8()
{
	return (uint8)hex2uint64();
}

unsigned int CmString::Match(CmLString** _ppLMatch, int8 const* _pPattern)
{
	int8* pFirst    = pText; //first matching character
	int8* pSrcText  = pText; //current text position
	int8  chMatch = ' ';	 //single character to be found
	int8* szMatch = NULL;	 //group of alternative characters
	uint32 nSkip;			 //skip any character
	uint32 nMatch;		     //current match number
	uint32 nMatchMin;		 //minimal match number
	uint32 nMatchMax;		 //maximal match number
	uint32 nLMatch = 0;	     //number of valid result list members

	// check pointers
	if ((NULL == _ppLMatch) || (NULL==_pPattern)) return false;

	for(;;){

		//Evaluate next pattern element
		nSkip   = 0;
		szMatch = NULL;
		switch(*_pPattern){
		case 0:
			return nLMatch;
		case '(': 
			pFirst = pSrcText;
			_pPattern++;
			continue;
		case ')': 
			if(NULL == *_ppLMatch){
				*_ppLMatch = new CmLString; 
			}
			(*_ppLMatch)->setText(pFirst, pSrcText-1);
			_ppLMatch = (*_ppLMatch)->getAdrNext();
			nLMatch++;
			_pPattern++;
			continue;
		case '.':
			_pPattern++;
			//Determine modifiers
			switch(*_pPattern){
			case'*':
				nSkip = MAX_UINT32;
				_pPattern++;
				break;
			case '+':
				nSkip = MAX_UINT32;
				pSrcText++;
				_pPattern++;
			default:
				nSkip = 1;
				break;
			}
			break;
		case '\\':
			_pPattern++;
			switch(*_pPattern){
			case 's':	//white spaces
				szMatch = " \t\r\n";		
				break;
			case 'd':	//digits
				szMatch = "0123456789";		
				break;
			case 'w':	//word characters
				szMatch = "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ";		
				break;
			default:
				chMatch = *_pPattern;
				break;
			}
			_pPattern++;
			break;
		default:
			chMatch = *_pPattern++;
			break;
		}

		//Determine possible modifiers
		switch(*_pPattern){
		case '*':
			nMatchMin = 0;
			nMatchMax = MAX_UINT32;
			_pPattern++;
			break;
		case '+':
			nMatchMin = 1;
			nMatchMax = MAX_UINT32;
			_pPattern++;
			break;
		case '?':
			nMatchMin = 0;
			nMatchMax = 1;
			_pPattern++;
			break;
		default:
			nMatchMin = 1;
			nMatchMax = 1;
			break;
		}

		//Step over matching character(s)
		for(nMatch=0;;){
			if(NULL == szMatch){
				while(nMatch < nMatchMax && chMatch == *pSrcText){
					nMatch++;
					pSrcText++;
				}
			}else{
				while(nMatch < nMatchMax && NULL != strchr(szMatch, (int32)*pSrcText)){
					nMatch++;
					pSrcText++;
				}
			}
			if(nMatch < nMatchMin){
				if(nSkip > nMatch){
					nSkip -= nMatch + 1;
					pSrcText++;
					nMatch = 0;
					break;
				}
				return nLMatch;
			}
			break;
		}
	}
}

// convert a network IP address along with a port number to a string.
CmString CmString::IP2String(uint32 _IP, uint16 _Port)
{
	// convert 4 subsequent address digits
	CmString IP;
	for (int i = 3; i >= 0; i--){
		uint8 Digit = (uint8)(_IP >> (i * 8));
		IP += Digit;
		IP += ".";
	}
	IP.setAt((uint32)IP.getLength() - 1, ':');
	IP += _Port;

	return IP;
}

// Search functions
int32 CmString::findPosition(char _ch, int32 _StartPos)
{
	// handle empty text field
	if (NULL == pText){
		Length = 0;
	}
	else{
  // Search for a given character starting at a determined position
  for( int32 i = _StartPos; i < (int32)Length; i++ )
  {
    if( *(pText + i) == _ch )
    return i;
  }
	}
  
  // Not found
  return (int32)Length + 1;
}

int32 CmString::findBackwards(char _ch, int32 _StartPos)
{
  // Adjust start position
  if( _StartPos == 0 )
    _StartPos = (int32)Length;

  // Search backwards for a given character starting at a determined position
  for( int32 i = _StartPos; i >= 0; i-- )
  {
    if( *(pText + i) == _ch )
    return i;
  }
  
  // Not found
  return (int32)Length + 1;
}

// Search pattern
int32 CmString::searchPattern(const char *_Pattern, int32 _StartPos)
{
  // Check search range
  if (_StartPos>=(int32)Length) return (int32)Length+1; // outside data

  // search for pattern
  char * Found = strstr(pText + _StartPos, _Pattern);
  if (NULL!=Found) 
    return (int32)(Found - pText);  // pattern found

  // Try binary search
  int32 Pos = _StartPos;
  while (Pos<(int32)Length){
    Pos = findPosition(*_Pattern,Pos);
    if ((Pos<=(int32)Length) && 0==strncmp(pText+Pos,_Pattern,strlen(_Pattern))) 
      return Pos;  // pattern found
    Pos++;
  }
  return (int32)Length+1; // not found
}


// Sub strings
int32 CmString::assignSubString(int32 _Last, CmString *_Src, int32 _First)
{
  size_t DstLength;
	size_t OldLength = Length;

  // Check for source string
  if( _Src == NULL)
    _Src = this;

  // Determine actual target length
  if( (int32)(_Src->getLength()) <= _First )
    DstLength = 0;
  else
  if( (int32)(_Src->getLength()) <= _Last )
    DstLength = _Src->getLength() - _First;
  else
  if( _Last < _First )
    DstLength = 0;
  else
    DstLength = _Last - _First + 1;

  // Check if the target string is empty
  if( DstLength == 0 )
    setLength(0);
  else
  // Check if the substring is assigned to current string object
  if( _Src == this )
  {
    // move current buffer away
    int8 *pOldText = pText;

    // Allocate new buffer
		Length = DstLength;
		pText = allocateMemory<int8>(int32(Length + 1), isInt8);

    // Transfer substring
    memcpy(pText, pOldText + _First, Length);
    *(pText + Length) = 0;

    // Delete previous string
		releaseMemory<int8>(pOldText, int32(OldLength + 1), isInt8);
	}
  else
  {
    // Adjust current string length
    setLength(DstLength);

    // Transfer content
	_Src->pText != NULL ? memcpy(pText, _Src->pText + _First, DstLength) : 0;
  }

  return (int32)DstLength;
}

bool CmString::setActive(uint32 _ActiveStart, uint32 _ActiveLength, int32 _LineNumber)
{
	// assigne a new active range and line number (if valid)
	ActiveStart = _ActiveStart;
	ActiveLength = _ActiveLength;
	if (_LineNumber >= 0){
		LineNumber = _LineNumber;
		// a valid line number also causes LineStart be initialized with ActiveStart
		LineStart = ActiveStart;
	}

	return true;
}
bool CmString::setHighlight(uint32 _HighlightStart, uint32 _HighlightLength)
{
	// assigne a new highlight range
	HighlightStart = _HighlightStart;
	HighlightLength = _HighlightLength;

	return true;
}
bool CmString::getActive(uint32& _ActiveStart, uint32& _ActiveLength, int32& _LineNumber)
{
	// return active range and line number
	_ActiveStart = ActiveStart;
	_ActiveLength = ActiveLength;
	_LineNumber = LineNumber;

	return true;
}
bool CmString::getHighlight(uint32& _HighlightStart, uint32& _HighlightLength)
{
	// return highlight range
	_HighlightStart = HighlightStart;
	_HighlightLength = HighlightLength;

	return true;
}
bool CmString::getLineHighlight(int32& _LineNumber, uint32& _LineStart, uint32& _HighlightStart, uint32& _HighlightLength)
{
	// return line and highlight information
	_LineNumber = LineNumber;
	_LineStart = LineStart;
	_HighlightStart = HighlightStart;
	_HighlightLength = HighlightLength;

	return true;
}
bool CmString::showActiveHighlight()
{
	int8 Active[256] = { 0 };
	int8 Highlight[256] = { 0 };
	uint32 Length_l = ActiveLength;

	if (Length_l > sizeof(Active)){
		Length_l = sizeof(Active);
	}
	memcpy(Active, getText() + ActiveStart, Length_l);

	Length_l = HighlightLength;
	if (Length_l > sizeof(Highlight)){
		Length_l = sizeof(Highlight);
	}
	memcpy(Highlight, getText() + HighlightStart, Length_l);

	return true;
}

// Conversion function
double CmString::getNumAsDouble()
{
	const double MaxDouble = 1e15;
	const double Error = 0;
	double Value = 0;
	double Fraction = 0;
	double FractionLength = 0;
	double Exponent = 0;
	double ValSign = 1.0;
	double ExpoSign = 1.0;
	double* Part = &Value;
	double* Sign = &ValSign;
	bool isFinished = false;
	bool isUnexpected = false;

	// preset conversion error to true
	isConversionError = true;

	for (int32 i = 0; i < (int32)(getLength()); i++){
		// check against overflow conditions
		if (Value > MaxDouble) return Error;
		if (Fraction > MaxDouble) return Error;
		if (Exponent > MaxDouble) return Error;
		// assemble values
		switch (*(pText + i)){
		// decimal digits
		case 0: isFinished = true; break;
		case '0': *Part = *Part * 10 + 0; break;
		case '1': *Part = *Part * 10 + 1; break;
		case '2': *Part = *Part * 10 + 2; break;
		case '3': *Part = *Part * 10 + 3; break;
		case '4': *Part = *Part * 10 + 4; break;
		case '5': *Part = *Part * 10 + 5; break;
		case '6': *Part = *Part * 10 + 6; break;
		case '7': *Part = *Part * 10 + 7; break;
		case '8': *Part = *Part * 10 + 8; break;
		case '9': *Part = *Part * 10 + 9; break;
		case ' ':
			if (0 == *Part){
				continue; // skip spaces at start
			}
			else{
				isFinished = true;
				break; // space ends conversion properly
			}

		// apply sign
		case '+': break;
		case '-': 
			if (*Sign != 1.0) return Error;
			if (Part == &Fraction) return Error;
			if (Part == &Value && i > 0) return Error;
			if (Part == &Exponent && Exponent > 0) return Error;
			*Sign = -*Sign;
			break;
		// start fraction
		case '.':
		case ',': Part = &Fraction; break;
		// start exponent
		case 'e':
		case 'E': Part = &Exponent; Sign = &ExpoSign; break;
		// reject/handle all others
		default: 
			if (0 == *Part){
				isUnexpected = true;
				break; // invalid digit at start 
			}
			else{
				isUnexpected = true; 
				isFinished = true; 
				break; // invalid digit forces end of conversion
			}
		}
		if (isFinished) break;
		if (Part == &Fraction) FractionLength++;
	}

	// add fraction
	if (FractionLength != 0){
		Value = ValSign * (Value + Fraction * pow(10.0, -(FractionLength-1)));
	}
	else{
		Value = ValSign * Value;
	}
	// add exponent
	if (Exponent != 0){
		Value *= pow(10.0, ExpoSign * Exponent);
	}

	// success: clear conversion error, unexpected: is conversion error 
	isConversionError = isUnexpected ? true : false;

	return Value;
}

double CmString::getNumAsDouble(int32 _PosStart, int32 _PosEnd)
{
	CmString Num;
	Num.assignSubString(_PosEnd, this, _PosStart);
	return Num.getNumAsDouble();
}

uint64 CmString::getNumAsUint64()
{
	const uint64 Error = 0;
	uint64 uNum = 0;
	int Significance = 10;

	// preset conversion error to true
	isConversionError = true;

	for (int32 i = 0; i < (int32)(getLength()); i++){
		// test end of string
		if (0 == *(pText + i)) return uNum;
		// process next digit
		uNum *= Significance;
		switch (*(pText + i)){
		// decimal digits
		case '0': uNum += 0; break;
		case '1': uNum += 1; break;
		case '2': uNum += 2; break;
		case '3': uNum += 3; break;
		case '4': uNum += 4; break;
		case '5': uNum += 5; break;
		case '6': uNum += 6; break;
		case '7': uNum += 7; break;
		case '8': uNum += 8; break;
		case '9': uNum += 9; break;
		case ' ':				
			if (0 == uNum)
				continue;	// skip leading spaces
			else{
				isConversionError = false;
				return uNum / Significance; // space ends conversion properly
			}

		// start a hex number
		case 'x':
		case 'X':
			if (uNum != 0) return Error;
			Significance = 16;
			break;
		// hex digits
		default:
			if (Significance != 16){
				if (0 == uNum)
					return Error;	// invalid digit before any value
				else
					return uNum / Significance; // invalid digit forces end of conversion
			}
			switch (*(pText + i)){
				// hex
			case 'a':
			case 'A':	uNum += 10; break;
			case 'b':
			case 'B': uNum += 11; break;
			case 'c':
			case 'C': uNum += 12; break;
			case 'd':
			case 'D': uNum += 13; break;
			case 'e':
			case 'E': uNum += 14; break;
			case 'f':
			case 'F': uNum += 15; break;
				// reject/handle others
			default: 
				if (0 == uNum){
					return Error; // invalid digit before any value
				}
				else{
					return uNum / Significance; // invalid char forces end of conversion
				}
			}
		}
	}

	// success: clear conversion error
	isConversionError = false;

	return uNum;
}

uint64 CmString::getNumAsUint64(int32 _PosStart, int32 _PosEnd)
{
	CmString Num;
	Num.assignSubString(_PosEnd, this, _PosStart);
	return Num.getNumAsUint64();
}

int32 CmString::getNumAsInt32()
{
	return int32(getNumAsUint64());
}

int32 CmString::getNumAsInt32(int32 _PosStart, int32 _PosEnd)
{
	if (NULL == pText) return 0;
	CmString Num;
	int32 Sign = pText[_PosStart] == '-' ? -1 : 1;
	Num.assignSubString(_PosEnd, this, pText[_PosStart] == '-' ? _PosStart + 1 : _PosStart);
	return Sign * int32(Num.getNumAsUint64());
}

int32 CmString::getMonthAsInt32()
{
	return operator==("Jan") ? 1 : operator==("Feb") ? 2 : operator==("Mar") ? 3 : operator==("Apr") ? 4 : operator==("May") ? 5 : operator==("Jun") ? 6 : operator==("Jul") ? 7 : operator==("Aug") ? 8 : operator==("Sep") ? 9 : operator==("Oct") ? 10 : operator==("Nov") ? 11 : operator==("Dec") ? 12 : 0;
}

int32 CmString::getMonthAsInt32(int32 _PosStart, int32 _PosEnd)
{
	CmString Month;
	Month.assignSubString(_PosEnd, this, _PosStart);
	return Month.getMonthAsInt32();
}

// Binary backup and restore
const int8* CmString::readBinary(const int8* _Filename)
{
	FILE*   pFile;
	struct  stat buf;
	int32   fh;
	uint32  uFileSize = 0;

	// check pointer
	if (NULL == _Filename) return pText;

	//Determine file size
#ifdef MSVS
	_sopen_s(&fh, _Filename, O_RDONLY, _SH_DENYWR, 0);
	if (-1 != fh){
		if (0==fstat(fh,&buf)){
			uFileSize = buf.st_size;
		}else{
			throw CmException("File status not accessible.");
		}
		close( fh );
	}else{
		throw CmException("Data file not found.");
	}
#else
	if (-1!=(fh = open(pFilename,O_RDONLY))){
		if (0==fstat(fh,&buf)){
			uFileSize = buf.st_size;
		}else{
			throw CmException("File status not accessible.");
		}
		close( fh );
	}else{
		throw CmException("Data file not found.");
	}
#endif

	#ifdef MSVS
		fopen_s(&pFile, _Filename, "rb");
	#else
		pFile = fopen(Filename,"rb");
	#endif
	if(NULL==pFile){
		throw CmException("Data file can not be opened.");
	}else{
		try{
			setLength(uFileSize);
			fread(pText,1,uFileSize,pFile);
			fclose(pFile);
		}catch(...){
			fclose(pFile);
			throw CmException("Data file not readable.");
		}
	}
	return pText;
}

bool CmString::writeBinary(const int8* _Filename)
{
	FILE*   pFile;

	// check pointer
	if (NULL == _Filename) return false;

#ifdef MSVS
		fopen_s(&pFile, _Filename, "wb");
	#else
		pFile = fopen(_pFilename,"wb");
	#endif

	// check if file could be opened
	if (NULL == pFile) return false;

	// write file to disk
	fwrite(pText,Length,1,pFile);
	fclose(pFile);

	return true;
}

bool CmString::appendBinary(const int8* _Filename)
{
	FILE*   pFile;

	// check pointer
	if (NULL == _Filename) return false;

#ifdef MSVS
	fopen_s(&pFile, _Filename, "ab");
#else
	pFile = fopen(pFilename, "ab");
#endif

	// check if file could be opened
	if (NULL == pFile) return false;

	// write file to disk
	fwrite(pText, Length, 1, pFile);
	fclose(pFile);

	return true;
}

bool CmString::isFileExists(const int8* _Filename)
{
	const int8* Filename = (_Filename != NULL ? _Filename : pText);
	if (NULL == Filename) return false;

	// determine whether file exists
	int32   fh;
#ifdef MSVS
	_sopen_s(&fh, Filename, O_RDONLY, _SH_DENYWR, 0);
	if (-1 != fh){
		close(fh);
	}
	else{
		return false;
	}
#else
	if (-1 == (fh = open(Filename, O_RDONLY))){
		close(fh);
	}
	else{
		return false;
	}
#endif

	return true;
}
bool CmString::isFolderExists(const int8* _FolderPath)
{
	const int8* FolderPath = (_FolderPath != NULL ? _FolderPath : pText);
	if (NULL == FolderPath) return false;

	/* workaround:
	*  Folder exists if it contains at least one file.
	*  ToDo: handle empty case
	*        PathFileExistsA(..) ?
	*/
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	hFind = FindFirstFileA(FolderPath, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) return false;
	FindClose(hFind);

	return true;
}
bool CmString::addFilesOnPath(CmLString& _Files, const int8* _Path){
	// check input parameters
	const int8* Path = (_Path != NULL ? _Path : pText);
	if (NULL == Path) return false;

	// validate folder path
	CmString File;
	CmString Search(Path);
	Search += "*";
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	hFind = FindFirstFileA(Search.getText(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) return false;
	// get all filenames on path
	for (;;){
		// check if item is a file
		if (false == (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
			// add found file to result list
			File = getText();   // path
			File += FindFileData.cFileName;
			_Files.addString(File);
		}
		// find next item
		if (false == FindNextFileA(hFind, &FindFileData)) break;
	}
	FindClose(hFind);

	if (_Files.getSize() == 0) return false;

	return true;
}

bool CmString::getFoldersOnPath(CmLString& _Folders, const int8* _Path){
	// check input parameters
	const int8* Path = (_Path != NULL ? _Path : pText);
	if (NULL == Path) return false;

	// initialize returned list of folders
	_Folders.clearList();

	// validate path
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	hFind = FindFirstFileA(Path, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) return false;
	// get all folders on path
	for (;;){
		// check if item is a folder
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			// add found folder to result list
			CmString Folder(FindFileData.cFileName);
			if (Folder != "." && Folder != ".."){
				_Folders.addString(Folder);
			}
		}
		// find next item
		if (false == FindNextFileA(hFind, &FindFileData)) break;
	}
	FindClose(hFind);

	if (_Folders.getSize() == 0) return false;

	return true;
}

bool CmString::allFiles(CmString& _File, const CmString& _Path)
{
	CmString Path(_Path);
	Path += "\\*";

	// check for start of file search
	if (INVALID_HANDLE_VALUE == hFile){
		// get first file
		hFile = FindFirstFileA(Path.getText(), &FindFileData);
		if (hFile == INVALID_HANDLE_VALUE) return false;
	}
	else{
		// find next file
		if (false == FindNextFileA(hFile, &FindFileData)){
			FindClose(hFile);
			hFile = INVALID_HANDLE_VALUE;
			return false;
		}
	}
	// evaluate filename
	CmString Filename(FindFileData.cFileName);
	for (;;){
		// skip entry if not a file
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || Filename == '.' || Filename == '..'){
			if (false == FindNextFileA(hFile, &FindFileData)){
				FindClose(hFile);
				hFile = INVALID_HANDLE_VALUE;
				return false;
			}
			Filename = FindFileData.cFileName;
			continue;
		}
		break;
	}
	// return file content
	Filename = _Path;
	Filename += "\\";
	Filename += FindFileData.cFileName;
	try{
		_File.readBinary(Filename.getText());
	}
	catch (...){
		return false;
	}

	return true;
}

bool CmString::allLines(CmString& _Line, uint32 _Offset)
{
	// find end-of-line
	uint32 LineEnd = searchPattern("\x0A", LineStart);
	// verify valid line
	if (LineEnd >= Length) return false;
	// evaluate offset
	_Line.clear();
	if (_Offset < LineEnd){
		// copy line
		_Line.assignSubString(LineEnd, this, LineStart + _Offset);
	}
	// step one line forward
	LineStart = LineEnd + 1;

	return true;
}

bool CmString::clearMemoryState()
{
	ItemsAllocated = ItemsReleased = ItemsOccupied = MemoryAllocated = MemoryReleased = MemoryOccupied = 0;

	return true;
}
CmString CmString::getMemoryState(bool _isClearType)
{
	CmString ItemCount;
	CmString MemorySize;
	CmString Memory;
	CmString ItemsChanged;
	CmString ItemsOfTypeCount;
	CmString MemoryChange;

	// item state
	int64 ItemsUsed = ItemsAllocated - ItemsReleased;
	int64 ItemsDif = ItemsUsed - ItemsOccupied;
	int64 ItemsOfTypeUsed = ItemsOfTypeAllocated - ItemsOfTypeReleased;
	ItemsOccupied = ItemsUsed;
	// memory state
	int64 MemoryUsed = MemoryAllocated - MemoryReleased;
	int64 MemoryDif = MemoryUsed - MemoryOccupied;
	MemoryOccupied = MemoryUsed;

	// generate a memory report message
	ItemCount.double2String(ItemsUsed / 1000.0, 3);
	ItemsChanged.double2String(double(ItemsDif), 0, 6);
	Memory = "Memory items= ";
	Memory += ItemCount;
	Memory += "k (dif:";
	Memory += ItemsChanged;
	Memory += ") ";
	if ((ItemsOfTypeAllocated != 0) || (ItemsOfTypeReleased != 0)){
		ItemsOfTypeCount.double2String(double(ItemsOfTypeUsed));
		Memory += "{";
		isFocus ? Memory += "focus " : 0;
		isInt8 ? Memory += "int8 " : 0;
		isUint8 ? Memory += "uint8 " : 0;
		isInt16 ? Memory += "int16 " : 0;
		isUint16 ? Memory += "uint16 " : 0;
		isDouble ? Memory += "double " : 0;
		isNaviX ? Memory += "NaviX " : 0;
		isCmString ? Memory += "CmString " : 0;
		isCmLString ? Memory += "CmLString " : 0;
		isCmStringFTL ? Memory += "CmStringFTL " : 0;
		isCmStringFTLChild ? Memory += "CmStringFTLChild " : 0;
		isCmServiceConnection ? Memory += "CmServiceConnection " : 0;
		isCmException ? Memory += "CmException " : 0;
		isCmMatrixFTL ? Memory += "CmMatrixFTL " : 0;
		isCmMatrix ? Memory += "CmMatrix " : 0;
		isCmVector ? Memory += "CmVector<T> " : 0;
		Memory += "= ";
		Memory += ItemsOfTypeCount;
		Memory += "}";
	}
	MemorySize.double2String((MemoryUsed) / 1000.0, 1);
	MemoryChange.double2String(double(MemoryDif), 0, 6);
	Memory += "  bytes= ";
	Memory += MemorySize;
	Memory += "k (dif:";
	Memory += MemoryChange;
	Memory += ")";

	// clear items of type
	if (_isClearType){
		ItemsOfTypeAllocated = 0;
		ItemsOfTypeReleased = 0;
	}

	return Memory;
}


//////////////////////////////////////////////////////////////////////
// CmLString Class
//////////////////////////////////////////////////////////////////////
CmLString::CmLString()
{
	pNext = NULL;
}
CmLString::~CmLString()
{
	// remove all subsequent elements
	clearList();
}

//-----access-functions-------------------------------------------------------

bool CmLString::addString(const CmString& _String)
{
	// assign string to root element if it is still empty
	if (0 == Length){
		setText(_String);
		return true;
	}

	// go to last element
	CmLString* pLString = this;
	while (NULL != pLString->pNext){
		pLString = pLString->pNext;
	}

	// extend list by a new element
	pLString->pNext = allocateMemory<CmLString>(1, isCmLString);
	pLString->pNext->setText(_String);

	return true;
}
bool CmLString::clearList()
{
	// clear string list of next element
	if (NULL != pNext){
		releaseMemory<CmLString>(pNext, 1, isCmLString);
	}
	pNext = NULL;

	// clear also root string
	clear();

	return true;
}
int32 CmLString::getSize()
{
	int32 Size = Length == 0 ? 0 : 1;

	// count list elements
	CmLString* pString = this->pNext;
	while (NULL != pString){
		Size++;
		pString = pString->pNext;
	}

	return Size;
}
CmString& CmLString::operator[](int32 _Index)
{
	// iterate list upto index, return last element if the index is too high
	CmLString* pString = this;
	while (_Index-- > 0 && NULL != pString){
		pString = pString->pNext;
	}

	return *pString;
}
CmString& CmLString::getLowestString()
{
	// return single item
	if (1 == getSize()) return *this;

	int32 IndexLowestString = 0;
	for (int i = 1; i < getSize(); i++){
		// compare string lengths
		if (this->operator[](i).getLength() < this->operator[](IndexLowestString).getLength()){
			IndexLowestString = i;
		}
		else if (this->operator[](i).getLength() == this->operator[](IndexLowestString).getLength()){
			if (strncmp(this->operator[](i).getText(), this->operator[](IndexLowestString).getText(), this->operator[](i).getLength()) < 0){
				IndexLowestString = i;
			}
		}
	}
	return this->operator[](IndexLowestString);
}
bool CmLString::getMatchingString(CmString& _MatchingString, const CmString& _Pattern)
{
	// clear result
	_MatchingString.clear();

	// match all strings against pattern
	for (int i = 0; i < getSize(); i++){
		if (this->operator[](i).searchPattern(_Pattern.getText()) < (int32)(this->operator[](i).getLength())){
			// return matching string
			_MatchingString = this->operator[](i);
			break;
		}
	}
	// check result
	if (_MatchingString.getLength() == 0) return false;

	return true;
}

//-----functions-for-maintaining-a-list-of-strings----------------------------

CmLString* CmLString::getNext()
{
	return pNext;
}
CmString* CmLString::setNext(CmLString* _Next)
{
	return(pNext = _Next);
}
CmLString** CmLString::getAdrNext()
{
	return &pNext;
}



//////////////////////////////////////////////////////////////////////
// CmMString Class
//////////////////////////////////////////////////////////////////////
CmMString::CmMString(const CmString* _SourceString,const int8* _Delimiters)
{
  if( NULL == _Delimiters )
    _Delimiters = CM_PATH_DELIMITERS;

	CurrentPosition = 0;
	mSetOfDelimiters.setText(_Delimiters);
	pSourceString = _SourceString;
}

CmMString::CmMString()
{

}

CmMString::~CmMString()
{
}

void CmMString::resetPosition()
{
	CurrentPosition = 0;
}

int8* CmMString::getNextDelimitedString()
{
	int32 SubStringLength = 0;
	bool   fDelimiterFound  = false;

	//Current position must not be greater than the string length
	if(NULL==pSourceString || CurrentPosition >= (int32)(pSourceString->Length)){
		return NULL;
	}
	for(;CurrentPosition<(int32)((*pSourceString).Length);CurrentPosition++){
		for(int32 k=0;k<(int32)(mSetOfDelimiters.getLength());k++){
			if((uint32)*(pSourceString->getText()+CurrentPosition) == mSetOfDelimiters[k]){
				fDelimiterFound = true;
				break;
			}
		}
		if(true==fDelimiterFound){
			if(SubStringLength>0){
				break;
			}
			fDelimiterFound = false;
		}else{
			SubStringLength++;
		}
	}
	//Transfer substring
	setLength(SubStringLength);
	for(int32 m=0;m<SubStringLength;m++){
		setAt(m, (uint8)*(pSourceString->getText()+CurrentPosition-SubStringLength+m));
	}

	return pText;
}

int8* CmMString::setDelimiters(int8* _Delimiters)
{
	return mSetOfDelimiters.setText(_Delimiters);
}

const CmString* CmMString::setSourceString(const CmString* _SourceString)
{
	return pSourceString = _SourceString;
}


