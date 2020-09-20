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

#define CmValueFTL_Config

#include "FTLight/CmStringFTL.h"

/*------CmStringFTL--------------------ToDo-----------------------------------
*
*  - process IP as text, e.g. 192.168.42.30
*  - configuration of "0.02" failed
*
*/


/** BinX conversion */
uint16* BinXconvert = NULL;							// BinX conversion table
const int32 BinXconvertSize = 32768;		// number of entries in the conversion table
int32 CountStringFTL = 0;

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

	// counter for BinXconvert bookkeeping
	CountStringFTL++;

	// init navigator
	/*X()->Root = this;
	X()->PosX = this;
	X()->PathX = this;*/
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

	// counter for BinXconvert bookkeeping
	CountStringFTL++;
}

CmStringFTL::~CmStringFTL()
{
	clearFTLightHierarchy();

	// remove conversion table with last CmStringFTL
	CountStringFTL--;
	if (0 == CountStringFTL && NULL != BinXconvert){
		CmString Memory;
		Memory.releaseMemory(BinXconvert, BinXconvertSize);
		BinXconvert = NULL;
	}
}

// unit test for CmStringFTL
bool CmStringFTL::testCmStringFTL()
{
#define BIN_LEN   1421507
#define STR_LEN   30
	CmString Memory;
	uint8* pField = Memory.allocateMemory<uint8>(BIN_LEN + 1, isUint8);
	uint8* pField1 = Memory.allocateMemory<uint8>(BIN_LEN + 1, isUint8);
	uint16* pBinMCL = Memory.allocateMemory<uint16>(16 * BIN_LEN / 15, isUint16);
	CmString mBinX;
	CmString mBinX1(STR_LEN);
	CmString mBinX2(STR_LEN);

	//printf("\nTesting BinX conversion\n");
	for (int i = 0; i<BIN_LEN; i++){
		pField[i] = (uint8)(i * 43);
	}

	// Convert binary to a BinX string
	bin2BinX(&mBinX, pField, BIN_LEN);
	MEMCPY(mBinX1.getBuffer(), mBinX1.getLength(), mBinX.getBuffer(), STR_LEN);
	MEMCPY(mBinX2.getBuffer(), mBinX2.getLength(), mBinX.getBuffer() + mBinX.getLength() - STR_LEN, STR_LEN);

	// Convert a BinX string to binary
	BinX2bin(pField1, BIN_LEN, mBinX);

	// Compare
	if (0 == memcmp(pField, pField1, BIN_LEN)){
		//printf("BinX conversion successful for %u bytes.\n", BIN_LEN);
	}
	else{
		printf("BinX (%3d): \"%s.....%s\"", (int32)mBinX.getLength(), mBinX1.getBuffer(), mBinX2.getBuffer());
		printf("\nBinX conversion ---FAILED---\n");
		return false;
	}

	// ToDo: fix BinMCL2bin test

	//// Convert binary to a BinMCL string
	//bin2BinMCL(BIN_LEN/30,(uint16*)pField,pBinMCL);
	//MEMCPY(mBinX1.getBuffer(),mBinX1.getLength(),(char*)pBinMCL,STR_LEN);
	//printf("\nBinMCL: \"%s\"\n",mBinX1.getBuffer());
	//
	//// Convert a BinMCL string to binary
	//BinMCL2bin(BIN_LEN/30,pBinMCL,(uint16*)pField1);

	//// Compare
	//if (0 == memcmp(pField, pField1, BIN_LEN - BIN_LEN % 2)){
	//	printf("\nBinMCL conversion successful for %u bytes.", BIN_LEN);
	//}
	//else{
	//	printf("\n\nBinMCL conversion ---FAILED---");
	//	return false;
	//}

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
		//printf("Numeric conversion (%20I64u): %9s len=%u (%20I64u) %s\n",	u64Num, mBinX.getBuffer(), (int32)mBinX.getLength(), u64Num1, u64Num == u64Num1 ? "OK" : "failed");
		u64Factor *= RADIX_216;
	}
	MBenchmark   mBenchmark(3);
	try{
		while (mBenchmark.run()){
			bin2BinX(&mBinX, pField, BIN_LEN);
		}
		printf("\n bin2BinX   / Byte: %s", mBenchmark.info(850, BIN_LEN).getText());
	}
	catch (CmException E){
		//throw CmException(E.getMessage(),"Dump");
		return false;
	}
	try{
		while (mBenchmark.run()){
			bin2BinMCL(BIN_LEN / 30, (uint16*)pField, pBinMCL);
		}
		printf("\n bin2BinMCL / Byte: %s", mBenchmark.info(850, BIN_LEN).getText());
	}
	catch (CmException E){
		//throw CmException(E.getMessage(),"Dump");
		return false;
	}
	// cleanup resources
	if (NULL != pField)
		Memory.releaseMemory<uint8>(pField, BIN_LEN + 1, isUint8);
	if (NULL != pField1)
		Memory.releaseMemory<uint8>(pField1, BIN_LEN + 1, isUint8);
	if (NULL != pBinMCL)
		Memory.releaseMemory<uint16>(pBinMCL, 16 * BIN_LEN / 15, isUint16);

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
	// test data formats retrieved from a FTLight structure
	// 
#define TEST_DATA_FORMAT "EKD@JN58nc_Türkenfeld.CmStringFTL,0\n\
,UInt64:0x1234567890abCDef\n\
,Int64:-1234567890123456\n\
,Double:0.1234567890123456\n\
,UInt32:0x1234abCD\n\
,Int32:-1234567890\n\
,Float:0.12345678\n\
,UInt16:0x12ab\n\
,Int16:-32768\n\
,UInt8:0xFF\n\
,Int8:-128\n\
,False:00\n\
,True:11\n\
"
	CmStringFTL FormatX;
	FormatX.processStringFTL(TEST_DATA_FORMAT);
	// UURI
	CmValueFTL X_UURI("0");
	if (false == X_UURI.updateFrom(FormatX)) return false;
	if (CmString(X_UURI) != CmString("EKD@JN58nc_Türkenfeld.CmStringFTL")) return false;
	// uint64
	CmValueFTL X_uint64("0-1-0");
	if (false == X_uint64.updateFrom(FormatX)) return false;
	if (0x1234567890abcdef != uint64(X_uint64)) return false;
	// int64
	CmValueFTL X_int64("0-2-0");
	if (false == X_int64.updateFrom(FormatX)) return false;
	if (-1234567890123456 != int64(X_int64)) return false;
	// double
	CmValueFTL X_double("0-3-0");
	if (false == X_double.updateFrom(FormatX)) return false;
	if (0.1234567890123456 != double(X_double)) return false;
	// uint32
	CmValueFTL X_uint32("0-4-0");
	if (false == X_uint32.updateFrom(FormatX)) return false;
	if (0x1234abcd != uint32(X_uint32)) return false;
	// int32
	CmValueFTL X_int32("0-5-0");
	if (false == X_int32.updateFrom(FormatX)) return false;
	if (-1234567890 != int32(X_int32)) return false;
	// float
	CmValueFTL X_float("0-6-0");
	if (false == X_float.updateFrom(FormatX)) return false;
	if (fabs(0.12345678 - float(X_float)) > 0.000000005) return false;
	// uint16
	CmValueFTL X_uint16("0-7-0");
	if (false == X_uint16.updateFrom(FormatX)) return false;
	if (0x12ab != uint16(X_uint16)) return false;
	// int16
	CmValueFTL X_int16("0-8-0");
	if (false == X_int16.updateFrom(FormatX)) return false;
	if (-32768 != int16(X_int16)) return false;
	// uint8
	CmValueFTL X_uint8("0-9-0");
	if (false == X_uint8.updateFrom(FormatX)) return false;
	if (0xFF != uint8(X_uint8)) return false;
	// int8
	CmValueFTL X_int8("0-10-0");
	if (false == X_int8.updateFrom(FormatX)) return false;
	if (-128 != int8(X_int8)) return false;
	// bool: false
	CmValueFTL X_false("0-11-0");
	if (false == X_false.updateFrom(FormatX)) return false;
	if (false != bool(X_false)) return false;
	// bool: true
	CmValueFTL X_true("0-12-0");
	if (false == X_true.updateFrom(FormatX)) return false;
	if (true != bool(X_true)) return false;

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

	return true;
}

bool CmStringFTL::isConversionTable()
{
	// check if a conversion table has been created before
	if (NULL == BinXconvert){
		// get memory
		CmString Memory;
		if (NULL == (BinXconvert = Memory.allocateMemory<uint16>(BinXconvertSize, isUint16))) return false;
		// fill conversion table
		for (int32 i = 0; i < BinXconvertSize; i++){
			*(BinXconvert + i) = (uint8)(256 * bcode((uint8)(i / 216)) + bcode((uint8)(i % 216)));
		}
	}
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
	if (NULL != Navigation)
		releaseMemory<NaviX>(Navigation, 1, isNaviX);

	// delete TextX
	if (NULL != TextX)
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);

	// re-initialize workspace
	Child = NULL;
	TextX = NULL;
	Navigation = NULL;
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
	if (NULL != _Data && 0 != _Length)
		setData(_Data, (int32)_Length);

	// Determine resulting binary length
	LengthX = getLengthBinX2bin((int32)Length);
	TextX = allocateMemory<uint8>((int32)LengthX + 1, isUint8);
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
	/*else{
		// init navigation
		X()->Root = this;
		X()->PosX = this;
		X()->PathX = this;
	}*/

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
				//LineFTL += LevelChild == 3 ? (NodeSibling->isBinary(isParentBinary) ? "\n;" : "\n,") : (NodeSibling->isBinary(isParentBinary) ? ";" : ",");
				//LevelChild < 3 ? LineFTL += (NodeSibling->isBinary(isParentBinary) ? ";" : ",") : 0;
				// ToDo: remove 'LevelChild < 6' limitation
				if (LevelChild >= 3 && LevelChild < 6){
					//LineFTL += (NodeSibling->isBinary(isParentBinary) ? ";" : ",");
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
			default:
				Element += "TYPEFTL_UNKNOWN";
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
#include "Shlwapi.h"
bool CmStringFTL::getConfigPath(CmString _AppData, CmString _AppCompany, CmString _AppProgram, CmString _AppProfile, CmString _AppVersion)
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
	if (_AppProfile.getLength() > 0 && _AppVersion.getLength() > 0){
		*this += "\\";
		*this += _AppProfile;
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

bool CmStringFTL::getIdentifier(const CmString& _Info, CmUURI& _UURI)
{
	// initialize LineX reader for reading first ItemX
	CmStringFTL Info;
	Info.setText(_Info);
	int32 LineLength = Info.searchPattern("\x0A", 0);
	int32 LineNumber = 1;
	Info.setActive(0, LineLength, LineNumber);

	// get identifier
	CmString Identifier;
	CmTypeFTL TypeX;
	bool isStartOfCollection;
	const bool isParentBinary = false;
	if (false == Info.getNextItemX(Info, Identifier, TypeX, isStartOfCollection, isParentBinary)) return false;

	// check type
	if (TYPEFTL_IDENTIFIER != TypeX) return false;

	// return found identifier as UURI
	_UURI = CmUURI(NULL,Identifier.getText());

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
unsigned char CmStringFTL::bcode(unsigned char _byt)
{
	unsigned char b;
	switch (_byt)
	{
	case 12: b = 248; break;      // avoid ,
	case 13: b = 249; break;      // avoid -
	case 26: b = 250; break;      // avoid :
	case 27: b = 251; break;      // avoid ;
	case 29: b = 252; break;      // avoid =
	case 32: b = 253; break;      // avoid @
	case 64: b = 254; break;      // avoid `
	case 95: b = 255; break;      // avoid del
	default: b = _byt + 32;
	}
	return b;
}

// FTLight bin decode
unsigned char CmStringFTL::bdecode(unsigned char _byt)
{
	unsigned char b;
	switch (_byt)
	{
	case 248: b = 12; break;      // avoid ,
	case 249: b = 13; break;      // avoid -
	case 250: b = 26; break;      // avoid :
	case 251: b = 27; break;      // avoid ;
	case 252: b = 29; break;      // avoid =
	case 253: b = 32; break;      // avoid @
	case 254: b = 64; break;      // avoid `
	case 255: b = 95; break;      // avoid del
	default: b = _byt - 32;
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
			uchCode = *(uint8*)(BinXconvert+uchCode);
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

// uuu_packer_w converts n groups of 15 words into 32n FTLight legal chars
bool CmStringFTL::bin2BinMCL(int _n, uint16 *_inp, uint16 *_out)
{
	int i;
	union shifter {
	uint16 wrd[2];			//[0]=lsw [1]=msw
	unsigned long lwr;
	} shf;

	// check conversion table
	if (false == isConversionTable()) return false;

	for (i=0;i<_n;i++){									//30270 = 32768*15/16
		shf.lwr = 0;

		shf.wrd[0]=*_inp++;								//get input word
		shf.lwr=shf.lwr<<1;								//save msb
		*_out++=*(uint16*)(BinXconvert+(shf.wrd[0]>>1));	//table lookup and store two chars

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinXconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinXconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinXconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinXconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinXconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinXconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinXconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinXconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinXconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinXconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinXconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinXconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinXconvert+(shf.wrd[0]>>1));

		shf.wrd[0]=*_inp++;
		shf.lwr=shf.lwr<<1;
		*_out++=*(uint16*)(BinXconvert+(shf.wrd[0]>>1));

		*_out++=*(uint16*)(BinXconvert+shf.wrd[1]);                   //store the word made of msb's
	}
	return true;
}

//unpacks n 32 byte groups into n 15 word groups
bool CmStringFTL::BinMCL2bin(int _n, uint16 *_inp, uint16 *_out)
{
	uint16 w,wb[15];
	for (int i=0;i<_n;i++)
  {
	  for (int j=0;j<15;j++){
			w=*_inp++;
			wb[j]=216*bdecode((uint8)(w/256))+bdecode((uint8)(w%256));
    }
    w=*_inp++;
    w=216*bdecode((uint8)(w/256))+bdecode((uint8)(w%256));
    w=w<<1;
		for (int j=0;j<15;j++)
    {
			*_out++=(wb[j])+(w&32768);
			w=w<<1;
    }
  }
	return true;
}

bool CmStringFTL::setFTLightType(CmString& _BinX, FTLight_DATA_TYPE_IDENTIFIER _FTLightType)
{
	// set result length
	_BinX.setLength(4);
	// decompose identifier value
	uint32 TypeFTL = _FTLightType;
	for (int i = 0; i < 4; i++){
		_BinX.setAt(i, bcode((uint8)(TypeFTL % 216)));
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
		FTLightType += bdecode(*(_BinX.getBuffer() + ActiveStart + i));
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
		// after sorting out all other data types this item is binary
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

//----------------------------------------------------------------------------
// CmMatrixFTL class
//----------------------------------------------------------------------------
//
CmMatrixFTL::CmMatrixFTL()
{

}
CmMatrixFTL::~CmMatrixFTL()
{
	clearMatrix();
}

bool CmMatrixFTL::testCmMatrixFTL()
{
	bool Result = true;

	// test values of different formats
	CmString TestString("67890.12345");
	double TestValue;
	sscanf_s(TestString.getText(), "%lf", &TestValue);
	const void* TestPointer = (void*)&TestValue;
	const uint64 TestUInt64 = (uint64)TestValue;
	const int64 TestInt64 = (int64)TestValue;
	const double TestDouble = (double)TestValue;
	const uint32 TestUInt32 = (uint32)TestValue;
	const int32 TestInt32 = (int32)TestValue;
	const float TestFloat = (float)TestValue;
	const uint16 TestUInt16 = (uint16)TestValue;
	const int16 TestInt16 = (int16)TestValue;
	const uint8 TestUInt8 = (uint8)TestValue;
	const int8 TestInt8 = (int8)TestValue;
	const bool TestBool = true;

	// test CmVector
	double Value;
	CmVector<double> Vector;
	Vector[0] = 1;
	Vector[2] = 1;
	if (1 != Vector[2]) Result = false;
	Vector[1] = TestInt32;
	if (TestInt32 != Vector[1]) Result = false;
	// vector extension
	if (Vector.getLength() != 3) Result = false;
	Vector[3] = TestInt32;
	if (Vector.getLength() != 4) Result = false;
	if (4 != Vector.getLength()) Result = false;
	if (TestInt32 != Vector[3]) Result = false;
	if (TestInt32 != Vector[1]) Result = false;
	// further size extension
	const int32 Index = 1500;
	Vector[Index] = TestInt32;
	if (Vector.getLength() != Index + 1) Result = false;
	if (1 != Vector[0]) Result = false;
	if (TestInt32 != Vector[1]) Result = false;
	if (TestInt32 != Vector[3]) Result = false;
	if (TestInt32 != Vector[Index]) Result = false;
	// size reduction
	const int32 Length = 3;
	Vector.setLength(Length);
	if (Vector.getLength() != Length) Result = false;
	if (1 != Vector[0]) Result = false;
	if (TestInt32 != Vector[1]) Result = false;
	if (1 != Vector[2]) Result = false;
	// length extension, size unchanged
	const int32 ExtendedLength = 5;
	Vector.setLength(ExtendedLength);
	if (Vector.getLength() != ExtendedLength) Result = false;
	if (1 != Vector[0]) Result = false;
	if (0 != Vector[ExtendedLength - 1]) Result = false;
	// length extension, size extension
	const int32 ExtendedLengthSize = 1500;
	Vector.setLength(ExtendedLengthSize);
	if (Vector.getLength() != ExtendedLengthSize) Result = false;
	if (1 != Vector[0]) Result = false;
	if (0 != Vector[ExtendedLengthSize - 1]) Result = false;

	// vector copy & compare
	Vector(1, 2, 3);
	Vector.setScalar(5);
	CmVector<double> Vector1;
	Vector1 = Vector;
	if (false == (Vector1 == Vector)) Result = false;

	// runtime 2000
	CmTimestamp Time;
	const char* MsgInit = "\n runtime double[%d] %s           %5.1f ns";
	int32 MaxIndex = 2000;
	// read
	Time.startRuntime();
	for (int32 i = 0; i < MaxIndex; i++){
		Value = Vector[i];
	}
	printf(MsgInit, MaxIndex, "read         ", (double)Time.getRuntime_ns() / MaxIndex);
	// read
	Time.startRuntime();
	for (int32 i = 0; i < MaxIndex; i++){
		Vector[i] = Value;
	}
	printf(MsgInit, MaxIndex, "write        ", (double)Time.getRuntime_ns() / MaxIndex);
	// write/read
	Time.startRuntime();
	for (int32 i = 0; i < MaxIndex; i++){
		Vector[i] = Value;
		Value = Vector[i];
	}
	printf(MsgInit, MaxIndex, "write/read   ", (double)Time.getRuntime_ns() / MaxIndex);
	// write/read 1000000
	MaxIndex = 1000000;
	Time.startRuntime();
	for (int32 i = 0; i < MaxIndex; i++){
		Vector[i] = Value;
		Value = Vector[i];
	}
	printf(MsgInit, MaxIndex, "write/read", (double)Time.getRuntime_ns() / MaxIndex);

	// test CmMatrixFTL constructor/destructor
	CmIndex I;
	const int32 Count = 10000;
	CmMatrixFTL* Array[Count];
	const int32 Levels = 3;
	I(1, 2, 3);
	const char* MsgMatrix = "\n runtime matrix new/delete(%d)          %8.1f ns";
	Time.startRuntime();
	for (int i = 0; i < Count; i++){
		Array[i] = CmString::allocateMemory<CmMatrixFTL>(1, isCmMatrixFTL);
		Array[i]->setString(TestString, I);
	}
	CmString Delete;
	for (int i = 0; i < Count; i++){
		Delete.releaseMemory<CmMatrixFTL>(Array[i], 1, isCmMatrixFTL);
	}
	printf(MsgMatrix, Count, (double)Time.getRuntime_ns() / (Levels * Count));

	//---test skalar
	CmMatrixFTL Matrix;
	Matrix.setValue(TestUInt16, I(0, 0));
	Matrix.setValue(TestUInt32, I(0, 0, 0));
	Matrix.setValue(TestUInt8, I(0));
	if (TestUInt16 != uint16(Matrix[I(0, 0)])) Result = false;
	if (TestUInt32 != uint32(Matrix[I(0, 0, 0)])) Result = false;
	if (TestUInt8 != uint8(Matrix[I(0)])) Result = false;

	//---test CmMatrixFTL 2x3
	// write item 1,2,3
	Matrix.setValue(TestDouble, I(1, 2));
	Matrix.setValue(TestInt64, I(0, 1));
	Matrix.setValue(TestUInt8, I(1, 0));
	// read item 1,2,3
	if (TestDouble != double(Matrix[I(1, 2)])) Result = false;
	if (TestInt64 != int64(Matrix[I(0, 1)])) Result = false;
	if (TestUInt8 != uint8(Matrix[I(1, 0)])) Result = false;

	//---test getVectorLength() 
	if (2 != Matrix.getVectorLength(I.clear())) Result = false;
	if (3 != Matrix.getVectorLength(I(1))) Result = false;
	if (0 != Matrix.getVectorLength(I(1, 2))) Result = false;
	if (0 != Matrix.getVectorLength(I(1, 2, 3))) Result = false;

	//---test clearMatrix()
	if (false == Matrix.clearMatrix()) return false;

	//---test formats
	// string
	Matrix.setString(TestString, I);
	if (Matrix.asString(I) != TestString) Result = false;
	// pointer
	Matrix.setValue((void*)TestPointer, I);
	if ((void*)uint64(Matrix[I]) != TestPointer) Result = false;
	// uint64
	Matrix.setValue(TestUInt64, I);
	if (uint64(Matrix[I]) != TestUInt64) Result = false;
	// int64
	Matrix.setValue(TestInt64, I);
	if (int64(Matrix[I]) != TestInt64) Result = false;
	// double
	Matrix.setValue(TestDouble, I);
	if (double(Matrix[I]) != TestDouble) Result = false;
	// uint32
	Matrix.setValue(TestUInt32, I);
	if (uint32(Matrix[I]) != TestUInt32) Result = false;
	// int32
	Matrix.setValue(TestInt32, I);
	if (int32(Matrix[I]) != TestInt32) Result = false;
	// float
	Matrix.setValue(TestFloat, I);
	if (float(Matrix[I]) != TestFloat) Result = false;
	// uint16
	Matrix.setValue(TestUInt16, I);
	if (uint16(Matrix[I]) != TestUInt16) Result = false;
	// int16
	Matrix.setValue(TestInt16, I);
	if (int16(Matrix[I]) != TestInt16) Result = false;
	// uint8
	Matrix.setValue(TestUInt8, I);
	if (uint8(Matrix[I]) != TestUInt8) Result = false;
	// int8
	Matrix.setValue(TestInt8, I);
	if (int8(Matrix[I]) != TestInt8) Result = false;
	// bool
	Matrix.setValue(TestBool, I);
	if (Matrix.asBool(I) != TestBool) Result = false;

	//---test full CmMatrixFTL 3x4x5
	const char* MsgInitMatrix = "\n runtime matrix %s           %8.1f ns";
	double Length3D;
	uint32 ItemCount = 0;
	Time.startRuntime();
	// write
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 4; j++){
			for (int k = 0; k < 5; k++){
				// length of the space diagonal
				I(i, j, k);
				Length3D = sqrt((double)(i*i + j*j + k*k));
				Length3D = k % 2 == 0 ? Length3D : -Length3D;
				// write 
				Matrix.setValue(Length3D, I);
				ItemCount++;
			}
		}
	}
	// read
	for (int k = 0; k < 5; k++){
		for (int j = 0; j < 4; j++){
			for (int i = 0; i < 3; i++){
				// length of the space diagonal
				I(i, j, k);
				Length3D = sqrt((double)(i*i + j*j + k*k));
				Length3D = k % 2 == 0 ? Length3D : -Length3D;
				// read and compare
				if (double(Matrix[I]) != Length3D) Result = false;
			}
		}
	}
	printf(MsgInitMatrix, "3x4x5 write/read", (double)Time.getRuntime_ns() / ItemCount);

	//---test sparse CmMatrixFTL 15D
	CmIndex I1;
	CmIndex I2;
	CmIndex I3;
	CmIndex I4;
	const int Loops = 5000;
	Time.startRuntime();
	for (int i = 0; i < Loops; i++){
		// modify indexes
		I1(1, 1, i, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3);
		I2(1, 1, 1, 1, 1, 2, 2, i, 2, 2, 3, 3, 3, 3, 3);
		I3(1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, i, 3, 3);
		I4(1, 1, i, 1, 1, 2, 2, i, 2, 2, 3, 3, i, 3, 3);
		// write 4 entries
		Matrix.setValue(1 * TestDouble, I1);
		Matrix.setValue(2 * TestDouble, I2);
		Matrix.setValue(3 * TestDouble, I3);
		Matrix.setString(TestString, I4);
		// read and compare 4 entries
		if (double(Matrix[I1]) != 1 * TestDouble) Result = false;
		if (double(Matrix[I2]) != 2 * TestDouble) Result = false;
		if (double(Matrix[I3]) != 3 * TestDouble) Result = false;
		if (Matrix.asString(I4) != TestString) Result = false;
	}
	printf(MsgInitMatrix, "15D write/read  ", (double)Time.getRuntime_ns() / (4 * Loops));

	//---test CmMatrix
	CmMatrix M;
	// string
	M[I(0)] = TestString;
	if (CmString(M(0)) != TestString) Result = false;
	// void*
	M[I(0,1)] = (void*)TestPointer;
	if ((void*)uint64(M(0,1)) != TestPointer) Result = false;
	// uint64
	M[I(0, 1, 2)] = TestUInt64;
	if (uint64(M(0, 1, 2)) != TestUInt64) Result = false;
	// int64
	M[I(0, 1, 2, 3)] = TestInt64;
	if (int64(M(0, 1, 2, 3)) != TestInt64) Result = false;
	// uint32
	M[I(0, 1, 2, 3, 4)] = TestUInt32;
	if (uint32(M(0, 1, 2, 3, 4)) != TestUInt32) Result = false;
	// int32
	M[I(0, 1, 2, 3, 4, 5)] = TestInt32;
	if (int32(M(0, 1, 2, 3, 4, 5)) != TestInt32) Result = false;
	// float
	M[I(0, 1, 2, 3, 4, 5, 6)] = TestFloat;
	if (float(M(0, 1, 2, 3, 4, 5, 6)) != TestFloat) Result = false;
	// uint16
	M[I(0, 1, 2, 3, 4, 5, 6, 7)] = TestUInt16;
	if (uint16(M(0, 1, 2, 3, 4, 5, 6, 7)) != TestUInt16) Result = false;
	// int16
	M[I(0, 1, 2, 3, 4, 5, 6, 7, 8)] = TestInt16;
	if (int16(M(0, 1, 2, 3, 4, 5, 6, 7, 8)) != TestInt16) Result = false;
	// uint8
	M[I(0, 1, 2, 3, 4, 5, 6, 7, 8, 9)] = TestUInt8;
	if (uint8(M(0, 1, 2, 3, 4, 5, 6, 7, 8, 9)) != TestUInt8) Result = false;
	// int8
	M[I(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10)] = TestInt8;
	if (int8(M(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10)) != TestInt8) Result = false;
	// bool
	M[I(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)] = TestBool;
	if (bool(M(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)) != TestBool) Result = false;
	// re-test all entries
	if (CmString(M(0)) != TestString) Result = false;
	if ((void*)uint64(M(0, 1)) != TestPointer) Result = false;
	if (uint64(M(0, 1, 2)) != TestUInt64) Result = false;
	if (int64(M(0, 1, 2, 3)) != TestInt64) Result = false;
	if (uint32(M(0, 1, 2, 3, 4)) != TestUInt32) Result = false;
	if (int32(M(0, 1, 2, 3, 4, 5)) != TestInt32) Result = false;
	if (float(M(0, 1, 2, 3, 4, 5, 6)) != TestFloat) Result = false;
	if (uint16(M(0, 1, 2, 3, 4, 5, 6, 7)) != TestUInt16) Result = false;
	if (int16(M(0, 1, 2, 3, 4, 5, 6, 7, 8)) != TestInt16) Result = false;
	if (uint8(M(0, 1, 2, 3, 4, 5, 6, 7, 8, 9)) != TestUInt8) Result = false;
	if (int8(M(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10)) != TestInt8) Result = false;
	if (bool(M(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)) != TestBool) Result = false;
	// intended mismatch
	if (int32(M(0, 1, 2, 3, 4, 6)) == TestInt32) Result = false;

	return Result;
}

bool CmMatrixFTL::clearMatrix()
{
	// travel vectors and delete all created objects
	for (int32 i = 0; i < getLength(); i++){
		if (DATAFORMAT_STRING == DataFormat[i]){
			CmString* String = (CmString*)CmVector<uint64>::operator[](i);
			if (NULL != String){
				CmString::releaseMemory<CmString>(String, 1, isCmString);
			}
			DataFormat[i] = DATAFORMAT_NONE;
		}
		if (DATAFORMAT_MATRIX == DataFormat[i]){
			CmMatrixFTL* Vector = (CmMatrixFTL*)CmVector<uint64>::operator[](i);
			if (NULL != Vector){
				CmString::releaseMemory<CmMatrixFTL>(Vector, 1, isCmMatrixFTL);
			}
			DataFormat[i] = DATAFORMAT_NONE;
		}
	}
	setLength(0);

	// delete objects that are stored in the scalar
	uint64 Scalar;
	CmDataFormat ScalarFormat;
	getScalar(Scalar, ScalarFormat);
	if (DATAFORMAT_STRING == ScalarFormat){
		CmString* String = (CmString*)Scalar;
		if (NULL != String){
			CmString::releaseMemory<CmString>(String, 1, isCmString);
		}
		setScalar(NULL, DATAFORMAT_NONE);
	}
	if (DATAFORMAT_MATRIX == ScalarFormat){
		CmMatrixFTL* Vector = (CmMatrixFTL*)Scalar;
		if (NULL != Vector){
			CmString::releaseMemory<CmMatrixFTL>(Vector, 1, isCmMatrixFTL);
		}
		setScalar(NULL, DATAFORMAT_NONE);
	}

	return true;
}

int32 CmMatrixFTL::getVectorLength(CmIndex& _Index)
{
	// check for root vector
	if (0 == _Index.getLength()){
		return getLength();
	}

	// retrieve last level of dimension
	bool isCreateNextLevel = false;
	CmMatrixFTL* LastMatrixLevel = getLastLevel(_Index.reset(), isCreateNextLevel);
	if (NULL == LastMatrixLevel) return 0;

	// get current value and format
	int32 Index = _Index[_Index.getMaxIndex()];
	CmMatrixFTL* Vector = (CmMatrixFTL*)LastMatrixLevel->CmVector<uint64>::operator[](Index);
	CmDataFormat Format = LastMatrixLevel->DataFormat[Index];

	// check format for valid matrix vector
	if (DATAFORMAT_MATRIX != Format || NULL == Vector) return 0;

	return Vector->getLength();
}

bool CmMatrixFTL::setScalar(uint64 _Scalar, CmDataFormat _ScalarFormat)
{
	CmVector<uint64>::setScalar(_Scalar);
	DataFormat.setScalar(_ScalarFormat);

	return true;
}
bool CmMatrixFTL::getScalar(uint64& _Scalar, CmDataFormat& _ScalarFormat)
{
	_Scalar = CmVector<uint64>::getScalar();
	_ScalarFormat = DataFormat.getScalar();

	return true;
}

double CmMatrixFTL::operator[](CmIndex& _Index)
{
	// retrieve last level of dimension
	bool isCreateNextLevel = false;
	CmMatrixFTL* MatrixFTL = getLastLevel(_Index.reset(), isCreateNextLevel);
	if (NULL == MatrixFTL) return 0;

	// get current value and format
	uint64 Value = 0;
	CmDataFormat Format = DATAFORMAT_NONE;
	MatrixFTL->getValue(Value, Format, _Index);

	// check data format for numeric value
	double Double = 0;
	switch (Format){
		// non-numeric formats
	case DATAFORMAT_NONE: break;
	case DATAFORMAT_FTLight: break;
	case DATAFORMAT_STRING: break;
		// numeric formats
	case DATAFORMAT_BINARY: Double = (double)Value; break;
	case DATAFORMAT_UINT64: Double = (double)(uint64)Value; break;
	case DATAFORMAT_INT64: Double = (double)(int64)Value; break;
	case DATAFORMAT_DOUBLE: Double = *((double*)&Value); break;
	case DATAFORMAT_UINT32: Double = (double)(uint32)Value; break;
	case DATAFORMAT_INT32: Double = (double)(int32)Value; break;
	case DATAFORMAT_FLOAT: Double = (double)(*((float*)&Value)); break;
	case DATAFORMAT_UINT16: Double = (double)(uint16)Value; break;
	case DATAFORMAT_INT16: Double = (double)(int16)Value; break;
	case DATAFORMAT_UINT8: Double = (double)(uint8)Value; break;
	case DATAFORMAT_INT8: Double = (double)(int8)Value; break;
	case DATAFORMAT_BOOL: Double = (double)Value; break;
		// vector
	case DATAFORMAT_MATRIX: break;
	default: break;
	}

	return Double;
}
double CmMatrixFTL::operator[](int32 _Index)
{
	// get current value and format
	CmIndex I(_Index);
	uint64 Value = 0;
	CmDataFormat Format = DATAFORMAT_NONE;
	getValue(Value, Format, I);

	// check data format for numeric value
	double Double = 0;
	switch (Format){
		// non-numeric formats
	case DATAFORMAT_NONE: break;
	case DATAFORMAT_FTLight: break;
	case DATAFORMAT_STRING: break;
		// numeric formats
	case DATAFORMAT_BINARY: Double = (double)Value; break;
	case DATAFORMAT_UINT64: Double = (double)(uint64)Value; break;
	case DATAFORMAT_INT64: Double = (double)(int64)Value; break;
	case DATAFORMAT_DOUBLE: Double = *((double*)&Value); break;
	case DATAFORMAT_UINT32: Double = (double)(uint32)Value; break;
	case DATAFORMAT_INT32: Double = (double)(int32)Value; break;
	case DATAFORMAT_FLOAT: Double = (double)(*((float*)&Value)); break;
	case DATAFORMAT_UINT16: Double = (double)(uint16)Value; break;
	case DATAFORMAT_INT16: Double = (double)(int16)Value; break;
	case DATAFORMAT_UINT8: Double = (double)(uint8)Value; break;
	case DATAFORMAT_INT8: Double = (double)(int8)Value; break;
	case DATAFORMAT_BOOL: Double = (double)Value; break;
		// vector
	case DATAFORMAT_MATRIX: break;
	default: break;
	}

	return Double;
}
bool CmMatrixFTL::setValue(int8 _Value, CmIndex& _Index)
{
	CmMatrixFTL* MatrixFTL = getLastLevel(_Index.reset());
	return NULL == MatrixFTL ? false : MatrixFTL->setValue((uint64)_Value, DATAFORMAT_INT8, _Index);
}
bool CmMatrixFTL::setValue(uint8 _Value, CmIndex& _Index)
{
	CmMatrixFTL* MatrixFTL = getLastLevel(_Index.reset());
	return NULL == MatrixFTL ? false : MatrixFTL->setValue((uint64)_Value, DATAFORMAT_UINT8, _Index);
}
bool CmMatrixFTL::setValue(bool _Value, CmIndex& _Index)
{
	CmMatrixFTL* MatrixFTL = getLastLevel(_Index.reset());
	return NULL == MatrixFTL ? false : MatrixFTL->setValue((uint64)_Value ? 1 : 0, DATAFORMAT_BOOL, _Index);
}
bool CmMatrixFTL::setValue(int16 _Value, CmIndex& _Index)
{
	CmMatrixFTL* MatrixFTL = getLastLevel(_Index.reset());
	return NULL == MatrixFTL ? false : MatrixFTL->setValue((uint64)_Value, DATAFORMAT_INT16, _Index);
}
bool CmMatrixFTL::setValue(uint16 _Value, CmIndex& _Index)
{
	CmMatrixFTL* MatrixFTL = getLastLevel(_Index.reset());
	return NULL == MatrixFTL ? false : MatrixFTL->setValue((uint64)_Value, DATAFORMAT_UINT16, _Index);
}
bool CmMatrixFTL::setValue(int32 _Value, CmIndex& _Index)
{
	CmMatrixFTL* MatrixFTL = getLastLevel(_Index.reset());
	return NULL == MatrixFTL ? false : MatrixFTL->setValue((uint64)_Value, DATAFORMAT_INT32, _Index);
}
bool CmMatrixFTL::setValue(uint32 _Value, CmIndex& _Index)
{
	CmMatrixFTL* MatrixFTL = getLastLevel(_Index.reset());
	return NULL == MatrixFTL ? false : MatrixFTL->setValue((uint64)_Value, DATAFORMAT_UINT32, _Index);
}
bool CmMatrixFTL::setValue(float _Value, CmIndex& _Index)
{
	CmMatrixFTL* MatrixFTL = getLastLevel(_Index.reset());
	return NULL == MatrixFTL ? false : MatrixFTL->setValue(*((uint32*)&_Value), DATAFORMAT_FLOAT, _Index);
}
bool CmMatrixFTL::setValue(int64 _Value, CmIndex& _Index)
{
	CmMatrixFTL* MatrixFTL = getLastLevel(_Index.reset());
	return NULL == MatrixFTL ? false : MatrixFTL->setValue((uint64)_Value, DATAFORMAT_INT64, _Index);
}
bool CmMatrixFTL::setValue(uint64 _Value, CmIndex& _Index)
{
	CmMatrixFTL* MatrixFTL = getLastLevel(_Index.reset());
	return NULL == MatrixFTL ? false : MatrixFTL->setValue((uint64)_Value, DATAFORMAT_UINT64, _Index);
}
bool CmMatrixFTL::setValue(double _Value, CmIndex& _Index)
{
	CmMatrixFTL* MatrixFTL = getLastLevel(_Index.reset());
	return NULL == MatrixFTL ? false : MatrixFTL->setValue(*((uint64*)&_Value), DATAFORMAT_DOUBLE, _Index);
}
bool CmMatrixFTL::setValue(void* _Value, CmIndex& _Index)
{
	CmMatrixFTL* MatrixFTL = getLastLevel(_Index.reset());
	return NULL == MatrixFTL ? false : MatrixFTL->setValue(*((uint64*)&_Value), DATAFORMAT_BINARY, _Index);
}
bool CmMatrixFTL::setString(const CmString& _String, CmIndex& _Index)
{
	CmMatrixFTL* MatrixFTL = getLastLevel(_Index.reset());

	CmString* String = CmString::allocateMemory<CmString>(1, isCmString);
	(*String) = _String;
	return NULL == MatrixFTL ? false : MatrixFTL->setValue((uint64)String, DATAFORMAT_STRING, _Index);
}

CmString CmMatrixFTL::asString(CmIndex& _Index)
{
	// retrieve last level of dimension
	bool isCreateNextLevel = false;
	CmMatrixFTL* MatrixFTL = getLastLevel(_Index.reset(), isCreateNextLevel);
	if (NULL == MatrixFTL) return CmString();

	// get current value and format
	uint64 Value = 0;
	CmDataFormat Format = DATAFORMAT_NONE;
	MatrixFTL->getValue(Value, Format, _Index);

	// check for string
	if (DATAFORMAT_STRING != Format || NULL == Value) return CmString();

	return *(CmString*)Value;
}
#ifdef CM_TYPE_OLD_STYLE
uint64 CmMatrixFTL::asUInt64(CmIndex& _Index)
{
	return (uint64)this->operator[](_Index);
}
int64 CmMatrixFTL::asInt64(CmIndex& _Index)
{
	return (int64)this->operator[](_Index);
}
double CmMatrixFTL::asDouble(CmIndex& _Index)
{
	return this->operator[](_Index);
}
uint32 CmMatrixFTL::asUInt32(CmIndex& _Index)
{
	return (uint32)this->operator[](_Index);
}
int32 CmMatrixFTL::asInt32(CmIndex& _Index)
{
	return (int32)this->operator[](_Index);
}
float CmMatrixFTL::asFloat(CmIndex& _Index)
{
	return (float)this->operator[](_Index);
}
uint16 CmMatrixFTL::asUInt16(CmIndex& _Index)
{
	return (uint16)this->operator[](_Index);
}
int16 CmMatrixFTL::asInt16(CmIndex& _Index)
{
	return (int16)this->operator[](_Index);
}
uint8 CmMatrixFTL::asUInt8(CmIndex& _Index)
{
	return (uint8)this->operator[](_Index);
}
int8 CmMatrixFTL::asInt8(CmIndex& _Index)
{
	return (int8)this->operator[](_Index);
}
#endif
bool CmMatrixFTL::asBool(CmIndex& _Index)
{
	return this->operator[](_Index) == 0 ? false : true;
}

CmMatrixFTL* CmMatrixFTL::getLastLevel(CmIndex& _IndexVector, bool _isCreateNextLevel)
{
	// initialize return value
	CmMatrixFTL* NextMatrixLevel = this;

	// check for empty _IndexVector
	if (0 == _IndexVector.getLength()) return NextMatrixLevel;

	// check for next level of dimension
	int32 Level = _IndexVector.getNextIndex();
	if (Level < _IndexVector.getMaxIndex()){
		// get current entries
		int32 Index = _IndexVector[Level];
		// reject negative indices
		if (Index < 0) return NULL;
		NextMatrixLevel = (CmMatrixFTL*)CmVector<uint64>::operator[](Index);
		CmDataFormat Format = DataFormat[Index];
		// check if a next level of dimension has to be created
		if (DATAFORMAT_MATRIX != Format || NULL == NextMatrixLevel){
			if (false == _isCreateNextLevel) return NULL;
			NextMatrixLevel = CmString::allocateMemory<CmMatrixFTL>(1, isCmMatrixFTL);
			// put previous value to scalar of the new vector
			NextMatrixLevel->setScalar(CmVector<uint64>::operator[](Index), DataFormat[Index]);
			// assign new vector to indexed position
			CmVector<uint64>::operator[](Index) = (uint64)NextMatrixLevel;
			DataFormat[Index] = DATAFORMAT_MATRIX;
			if (NULL == NextMatrixLevel) return NULL;
		}
		// retrieve last level of dimension
		NextMatrixLevel = NextMatrixLevel->getLastLevel(_IndexVector);
	}
	else{
		//if (false == _isCreateNextLevel){
		//	return NULL;
		//}
	}

	return NextMatrixLevel;
}
bool CmMatrixFTL::setValue(uint64 _Value, CmDataFormat _Format, CmIndex& _Index)
{
	// check for empty vector
	if (0 == _Index.getLength()){
		// write value and format to own scalar
		setScalar(_Value, _Format);
	}
	else{
		uint32 Index = _Index[_Index.getMaxIndex()];
		// delete string if one is attached
		if (DATAFORMAT_STRING == DataFormat[Index]){
			CmString* String = (CmString*)CmVector<uint64>::operator[](Index);
			if (NULL != String){
				CmString::releaseMemory<CmString>(String, 1, isCmString);
			}
			DataFormat[Index] = DATAFORMAT_NONE;
		}
		// check if a matrix is attached
		if (DATAFORMAT_MATRIX == DataFormat[Index] && NULL != CmVector<uint64>::operator[](Index)){
			// write value and format to attached matrix's scalar
			CmMatrixFTL* MatrixFTL = (CmMatrixFTL*)CmVector<uint64>::operator[](Index);
			MatrixFTL->setScalar(_Value, _Format);
		}
		else{
			// set new value and format on indexed position
			CmVector<uint64>::operator[](Index) = _Value;
			DataFormat[Index] = _Format;
		}
	}

	return true;
}
bool CmMatrixFTL::getValue(uint64& _Value, CmDataFormat& _Format, CmIndex& _Index)
{
	// check for empty vector
	if (0 == _Index.getLength()){
		// get value/format from own scalar
		getScalar(_Value, _Format);
	}
	else{
		int32 Index = _Index[_Index.getMaxIndex()];
		// validate index
		if (Index < 0){
			_Value = 0;
			_Format = DATAFORMAT_NONE;
		}else
		// check if a matrix is attached
		if (DATAFORMAT_MATRIX == DataFormat[Index] && NULL != CmVector<uint64>::operator[](Index)){
			// get value/format from matrix's scalar
			CmMatrixFTL* MatrixFTL = (CmMatrixFTL*)CmVector<uint64>::operator[](Index);
			MatrixFTL->getScalar(_Value, _Format);
		}else{
			// get value/format from indexed position
			_Value = CmVector<uint64>::operator[](Index);
			_Format = DataFormat[Index];
		}
	}

	return true;
}

//----------------------------------------------------------------------------
// CmMatrix class
//----------------------------------------------------------------------------
//
CmMatrix::CmMatrix()
{

}
CmMatrix::~CmMatrix()
{

}

/** addressing matrix element by an index */
CmMatrix& CmMatrix::operator[](CmIndex& _Index)
{
	Index = _Index;
	return *this;
}
CmMatrix& CmMatrix::operator()(){
	Index.clear();
	return *this;
}
CmMatrix& CmMatrix::operator()(int _a){
	Index(_a);
	return *this;
}
CmMatrix& CmMatrix::operator()(int _a, int _b){
	Index(_a, _b);
	return *this;
}
CmMatrix& CmMatrix::operator()(int _a, int _b, int _c){
	Index(_a, _b, _c);
	return *this;
}
CmMatrix& CmMatrix::operator()(int _a, int _b, int _c, int _d){
	Index(_a, _b, _c, _d);
	return *this;
}
CmMatrix& CmMatrix::operator()(int _a, int _b, int _c, int _d, int _e){
	Index(_a, _b, _c, _d, _e);
	return *this;
}
CmMatrix& CmMatrix::operator()(int _a, int _b, int _c, int _d, int _e, int _f){
	Index(_a, _b, _c, _d, _e, _f);
	return *this;
}
CmMatrix& CmMatrix::operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g){
	Index(_a, _b, _c, _d, _e, _f, _g);
	return *this;
}
CmMatrix& CmMatrix::operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g, int _h){
	Index(_a, _b, _c, _d, _e, _f, _g, _h);
	return *this;
}
CmMatrix& CmMatrix::operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g, int _h, int _i){
	Index(_a, _b, _c, _d, _e, _f, _g, _h, _i);
	return *this;
}
CmMatrix& CmMatrix::operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g, int _h, int _i, int _j){
	Index(_a, _b, _c, _d, _e, _f, _g, _h, _i, _j);
	return *this;
}
CmMatrix& CmMatrix::operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g, int _h, int _i, int _j, int _k){
	Index(_a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k);
	return *this;
}
CmMatrix& CmMatrix::operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g, int _h, int _i, int _j, int _k, int _l){
	Index(_a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l);
	return *this;
}
CmMatrix& CmMatrix::operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g, int _h, int _i, int _j, int _k, int _l, int _m){
	Index(_a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l, _m);
	return *this;
}
CmMatrix& CmMatrix::operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g, int _h, int _i, int _j, int _k, int _l, int _m, int _n){
	Index(_a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l, _m, _n);
	return *this;
}
CmMatrix& CmMatrix::operator()(int _a, int _b, int _c, int _d, int _e, int _f, int _g, int _h, int _i, int _j, int _k, int _l, int _m, int _n, int _o){
	Index(_a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l, _m, _n, _o);
	return *this;
}

/** matrix write access */
bool CmMatrix::operator=(const CmString& _String){
	return setString(_String, Index);
}
bool CmMatrix::operator=(const int8* _String){
	return setString(_String, Index);
}
bool CmMatrix::operator=(void* _Value){
	return setValue(_Value, Index);
}
bool CmMatrix::operator=(uint64 _Value){
	return setValue(_Value, Index);
}
bool CmMatrix::operator=(int64 _Value){
	return setValue(_Value, Index);
}
bool CmMatrix::operator=(double _Value){
	return setValue(_Value, Index);
}
bool CmMatrix::operator=(uint32 _Value){
	return setValue(_Value, Index);
}
bool CmMatrix::operator=(int32 _Value){
	return setValue(_Value, Index);
}
bool CmMatrix::operator=(float _Value){
	return setValue(_Value, Index);
}
bool CmMatrix::operator=(uint16 _Value){
	return setValue(_Value, Index);
}
bool CmMatrix::operator=(int16 _Value){
	return setValue(_Value, Index);
}
bool CmMatrix::operator=(uint8 _Value){
	return setValue(_Value, Index);
}
bool CmMatrix::operator=(int8 _Value){
	return setValue(_Value, Index);
}
bool CmMatrix::operator=(bool _Value){
	return setValue(_Value, Index);
}

/** matrix read acces and subsequent type conversion */
CmMatrix::operator CmString(){
	return asString(Index);
}
CmMatrix::operator const int8*(){
	return asString(Index).getText();
}
CmMatrix::operator void*(){
	return (void*)uint64(CmMatrixFTL::operator[](Index));
}
CmMatrix::operator uint64(){
	return uint64(CmMatrixFTL::operator[](Index));
}
CmMatrix::operator int64(){
	return int64(CmMatrixFTL::operator[](Index));
}
CmMatrix::operator double(){
	return CmMatrixFTL::operator[](Index);
}
CmMatrix::operator uint32(){
	return uint32(CmMatrixFTL::operator[](Index));
}
CmMatrix::operator int32(){
	return int32(CmMatrixFTL::operator[](Index));
}
CmMatrix::operator float(){
	return float(CmMatrixFTL::operator[](Index));
}
CmMatrix::operator uint16(){
	return uint16(CmMatrixFTL::operator[](Index));
}
CmMatrix::operator int16(){
	return int16(CmMatrixFTL::operator[](Index));
}
CmMatrix::operator uint8(){
	return uint8(CmMatrixFTL::operator[](Index));
}
CmMatrix::operator int8(){
	return int8(CmMatrixFTL::operator[](Index));
}
CmMatrix::operator bool(){
	return CmMatrixFTL::operator[](Index) == 0 ? false : true;
}

/** matrix functions */
int32 CmMatrix::getSizeLastLevel()
{
	return CmMatrixFTL::getVectorLength(Index);
}
bool CmMatrix::addValue(int32& _Position, int32 _Value)
{
	int32 SizeLL = getSizeLastLevel();
	int32 Levels = Index.getLength();
	int32 i;
	int32 Val;
	bool isInserted = false;
	for (i = 0; i < SizeLL; i++){
		Index[Levels] = i;
		if (_Value == (Val = int32(operator[](Index)))){
			break;
		}
	}
	// insert new P1
	if (i >= SizeLL){
		Index[Levels] = i;
		operator=(_Value);
		isInserted = true;
	}
	_Position = i;

	return isInserted;
}
bool CmMatrix::appendItem(const int8* _Item)
{
	CmString Item(_Item);
	return appendItem(Item);
}
bool CmMatrix::appendItem(const CmString& _Item)
{
	int32 SizeLL = getSizeLastLevel();
	int32 Levels = Index.getLength();
	int32 i;
	CmString Item;
	bool isInserted = false;
	for (i = 0; i < SizeLL; i++){
		Index[Levels] = i;
		Item = CmString(operator[](Index));
		// check if the item exists already
		if (_Item == Item){
			break;
		}
	}

	// insert new Item
	if (i >= SizeLL){
		Index[Levels] = i;
		operator=(_Item);
		isInserted = true;
	}

	return isInserted;
}
bool CmMatrix::insertItem(int32 /*_Position*/)
{

	return true;
}
bool CmMatrix::deleteItem(int32 /*_Position*/)
{

	return true;
}

//----------------------------------------------------------------------------
// CmValueFTL class
//----------------------------------------------------------------------------
//
CmValueFTL* CmValueFTL::ValuesFTL = NULL;

CmValueFTL::CmValueFTL(int8 *_Address)
{
	if (NULL == _Address){
		Address = "0";
	}
	else if(strlen(_Address) == 0){
		Address = "0";
	}
	else{
		Address = _Address;
	}

	char Addr[256];
	strcpy_s(Addr, sizeof(Addr),Address.getBuffer());

	// initialize list of values
	Next = NULL;
	if (Address.getLength() == 1){
		// the UURI will be the container for subsequent CmValueFTL
		ValuesFTL = this;
	}
	else if(NULL != ValuesFTL){
		// insert into list of CmValueFTL
		CmValueFTL* ValueFTL = ValuesFTL;
		while (NULL != ValueFTL->Next){
			ValueFTL = ValueFTL->Next;
		}
		ValueFTL->Next = this;
	}

	// reset root of linear CmValueFTL array
	Array = NULL;

	// reset root of a multidimensional CmMatrixFTL matrix
	Matrix = NULL;
}
CmValueFTL::CmValueFTL(CmTypeFTL _TypeFTL)
{
	// set type
	TypeX = _TypeFTL;

	// initialize workspace
	Address = "0";
	Next = NULL;
	Array = NULL;
	Matrix = NULL;
	Remainder = "0";
}
CmValueFTL::~CmValueFTL()
{
	if (NULL != Matrix){
		CmString::releaseMemory(Matrix);
		Matrix = NULL;
	}
}

bool CmValueFTL::testCmValueFTL()
{
	bool Result = true;
	printf("\n");

	// test format conversion
	CmString TestString("67890.12345");
	double TestValue;
	sscanf_s(TestString.getText(), "%lf", &TestValue);
	const uint64 TestUInt64 = (uint64)TestValue;
	const int64 TestInt64 = (int64)TestValue;
	const double TestDouble = (double)TestValue;
	const uint32 TestUInt32 = (uint32)TestValue;
	const int32 TestInt32 = (int32)TestValue;
	const float TestFloat = (float)TestValue;
	const uint16 TestUInt16 = (uint16)TestValue;
	const int16 TestInt16 = (int16)TestValue;
	const uint8 TestUInt8 = (uint8)TestValue;
	const int8 TestInt8 = (int8)TestValue;
	const bool TestBool = true;

	// check particular formats
	CmValueFTL Val;
	Val = TestDouble;
	if (TestDouble != double(Val))	{ printf("\n 'double' failed: %.5f != %.5f", TestDouble, double(Val)); Result = false; }
	if (TestString != Val.asStringConvert(5))	{ printf("\n 'CmString' failed: %s != %s", TestString.getText(), Val.asStringConvert()); Result = false; }
	// NOTE: conversion from any format to 'uint64' is not supported (cannot be done exactly using 'double' as an intermediate format, CPU/compiler problems?)
	if (TestUInt64 == uint64(Val))	{ printf("\n 'uint64' failed: %llu != %llu", TestUInt64, uint64(Val)); Result = false; }
	if (TestInt64 != int64(Val))	{ printf("\n 'int64' failed: %lld != %lld", TestInt64, int64(Val)); Result = false; }
	if (TestUInt32 != uint32(Val))	{ printf("\n 'uint32' failed: %u != %u", TestUInt32, uint32(Val)); Result = false; }
	if (TestInt32 != int32(Val))	{ printf("\n 'int32' failed: %d != %d", TestInt32, int32(Val)); Result = false; }
	if (TestFloat != float(Val))	{ printf("\n 'float' failed: %f != %f", TestFloat, float(Val)); Result = false; }
	if (TestUInt16 != uint16(Val))	{ printf("\n 'uint16' failed: %u != %u", TestUInt16, uint16(Val)); Result = false; }
	if (TestInt16 != int16(Val))	{ printf("\n 'int16' failed: %d != %d", TestInt16, int16(Val)); Result = false; }
	if (TestUInt8 != uint8(Val))	{ printf("\n 'uint8' failed: %u != %u", TestUInt8, uint8(Val)); Result = false; }
	if (TestInt8 != int8(Val))	{ printf("\n 'int8' failed: %d != %d", TestInt8, int8(Val)); Result = false; }
	if (TestBool != bool(Val))	{ printf("\n 'bool' failed: %s != %s", TestBool ? "true" : "false", bool(Val) ? "true" : "false"); Result = false; }
	// re-check initial 'double' value
	if (TestDouble != double(Val))	{ printf("\n 'double' failed: %.5f != %.5f", TestDouble, double(Val)); Result = false; }
	// check string with length in format specification
	CmString TestLength(" ");
	TestLength += TestString;
	if (TestLength != Val.asStringConvert(5, 12))	{ printf("\n 'CmString' failed: %s != %s", TestLength.getText(), Val.asStringConvert().getText()); Result = false; }
	// (1) check 'uint64' conversion
	uint64 TestUInt64_1 = 0xFFFFFFFFFFFFFFFF;
	const double TestDouble_1 = (double)TestUInt64_1;
	Val = TestUInt64_1;
	if (TestUInt64_1 != uint64(Val))	{ printf("\n 'uint64(1)' failed: %llu != %llu", TestUInt64_1, uint64(Val)); Result = false; }
	if (TestDouble_1 != double(Val))	{ printf("\n 'double(1)' failed: %.5f != %.5f", TestDouble_1, double(Val)); Result = false; }
	// (2) check 'uint32' conversion
	uint32 TestUInt32_2 = 0xFFFFFFFF;
	CmString TestString_2("4294967295");
	const double TestDouble_2 = (double)TestUInt32_2;
	Val = TestUInt32_2;
	if (TestUInt32_2 != uint32(Val))	{ printf("\n 'uint32(2)' failed: %lu != %lu", TestUInt32_2, uint32(Val)); Result = false; }
	if (TestDouble_2 != double(Val))	{ printf("\n 'double(2)' failed: %.0f != %.0f", TestDouble_2, double(Val)); Result = false; }
	if (TestString_2 != Val.asStringConvert())	{ printf("\n 'CmString(2)' failed: %s != %s", TestString_2.getText(), Val.asStringConvert().getText()); Result = false; }
	if (TestUInt32_2 != uint32(Val))	{ printf("\n 'uint32(2)' failed: %lu != %lu", TestUInt32_2, uint32(Val)); Result = false; }
	// (3) check 'float' conversion
	float TestFloat_3 = TestFloat;
	const double TestDouble_3 = (double)TestFloat_3;
	Val = TestFloat_3;
	if (TestFloat_3 != float(Val))	{ printf("\n 'float(3)' failed: %f != %f", TestFloat_3, float(Val)); Result = false; }
	if (TestDouble_3 != double(Val))	{ printf("\n 'double(3)' failed: %f != %f", TestDouble_3, double(Val)); Result = false; }
	if (TestFloat_3 != float(Val))	{ printf("\n 'float(3)' failed: %f != %f", TestFloat_3, float(Val)); Result = false; }

	//---test matrix formats
	CmIndex I;
	CmMatrix M = Val.getMatrix();
	I(1, 2, 3);
	// string
	M[I] = TestString;
	if (CmString(M[I]) != TestString) Result = false;
	// uint64
	M[I] = TestUInt64;
	if (uint64(M[I]) != TestUInt64) Result = false;
	// int64
	M[I] = TestInt64;
	if ((int64)M[I] != TestInt64) Result = false;
	// double
	M[I] = TestDouble;
	if ((double)M[I] != TestDouble) Result = false;
	// uint32
	M[I] = TestUInt32;
	if ((uint32)M[I] != TestUInt32) Result = false;
	// int32
	M[I] = TestInt32;
	if ((int32)M[I] != TestInt32) Result = false;
	// float
	M[I] = TestFloat;
	if ((float)M[I] != TestFloat) Result = false;
	// uint16
	M[I] = TestUInt16;
	if ((uint16)M[I] != TestUInt16) Result = false;
	// int16
	M[I] = TestInt16;
	if ((int16)M[I] != TestInt16) Result = false;
	// uint8
	M[I] = TestUInt8;
	if ((uint8)M[I] != TestUInt8) Result = false;
	// int8
	M[I] = TestInt8;
	if ((int8)M[I] != TestInt8) Result = false;
	// bool
	M[I] = TestBool;
	if ((bool)M[I] != (TestBool ? 1 : 0)) Result = false;

	//---test getVectorLength() 
	if (2 != M().getSizeLastLevel()) Result = false;
	if (3 != M(1).getSizeLastLevel()) Result = false;
	if (4 != M(1, 2).getSizeLastLevel()) Result = false;
	if (0 != M(1, 2, 3).getSizeLastLevel()) Result = false;
	if (0 != M(1, 2, 3, 4).getSizeLastLevel()) Result = false;

	//--test bignum operations
	const uint64 ValA = 4785604381;
	const uint64 ValB = 2175489273;
	const uint64 ValD = 14856043819707253914;
	uint64 ValC;
	uint64 ValR;
	uint64 SumR = ValA + ValB;
	uint64 DifR = ValA - ValB;
	uint64 MulR = ValA * ValB;
	uint64 DivR = ValD / ValB;
	uint64 RemR = ValD % ValB;
	CmValueFTL A(TYPEFTL_NUMBER);
	CmValueFTL B(TYPEFTL_NUMBER);
	CmValueFTL C(TYPEFTL_NUMBER);
	CmValueFTL D(TYPEFTL_NUMBER);
	CmString R;
	A.uint2String(ValA);
	B.uint2String(ValB);
	D.uint2String(ValD);

	// runtime
	CmTimestamp Time;
	Time.startRuntime();
	char* MsgInit = "\n runtime[%d] %s %s %10.1f us";
	char* Info = "64-bit                     ";
	int32 Loops = 1000;

	// sum
	for (int32 i = 0; i < Loops; i++){ R = A + B; }
	ValR = R.getNumAsUint64();
	if (ValR != SumR) Result = false;
	printf(MsgInit, Loops, "A + B", Info, (double)Time.getRuntime_us() / Loops);
	if (ValR != SumR) printf(" failed");
	// sum=
	for (int32 i = 0; i < Loops; i++){ C.setNumber(A); C += B; }
	ValC = C.getNumAsUint64();
	if (ValC != SumR) Result = false;
	printf(MsgInit, Loops, "C +=A", Info, (double)Time.getRuntime_us() / Loops);
	if (ValC != SumR) printf(" failed");
	// dif
	for (int32 i = 0; i < Loops; i++){ R = A - B; }
	ValR = R.getNumAsUint64();
	if (ValR != DifR) Result = false;
	printf(MsgInit, Loops, "A - B", Info, (double)Time.getRuntime_us() / Loops);
	if (ValR != DifR) printf(" failed");
	for (int32 i = 0; i < Loops; i++){ R = B - A; }
	R.assignSubString(int32(R.getLength()) - 1, 0, 1);
	ValR = R.getNumAsUint64();
	if (ValR != DifR) Result = false;
	printf(MsgInit, Loops, "B - A", Info, (double)Time.getRuntime_us() / Loops);
	if (ValR != DifR) printf(" failed");
	// dif=
	for (int32 i = 0; i < Loops; i++){ C.setNumber(A); C -= B; }
	ValC = C.getNumAsUint64();
	if (ValC != DifR) Result = false;
	printf(MsgInit, Loops, "C -=A", Info, (double)Time.getRuntime_us() / Loops);
	if (ValC != DifR) printf(" failed");
	// mul
	for (int32 i = 0; i < Loops; i++){ R = A * B; }
	ValR = R.getNumAsUint64();
	if (ValR != MulR) Result = false;
	printf(MsgInit, Loops, "A * B", Info, (double)Time.getRuntime_us() / Loops);
	if (ValR != MulR) printf(" failed");
	// mul=
	for (int32 i = 0; i < Loops; i++){ C.setNumber(A); C *= B; }
	ValC = C.getNumAsUint64();
	if (ValC != MulR) Result = false;
	printf(MsgInit, Loops, "C *=A", Info, (double)Time.getRuntime_us() / Loops);
	if (ValC != MulR) printf(" failed");
	// div
	for (int32 i = 0; i < Loops; i++){ R = D / B; }
	ValR = R.getNumAsUint64();
	if (ValR != DivR) Result = false;
	printf(MsgInit, Loops, "D / B", Info, (double)Time.getRuntime_us() / Loops);
	if (ValR != DivR) printf(" failed");
	// div
	for (int32 i = 0; i < Loops; i++){ C.setNumber(D); C /= B; }
	ValC = C.getNumAsUint64();
	if (ValC != DivR) Result = false;
	printf(MsgInit, Loops, "D /=B", Info, (double)Time.getRuntime_us() / Loops);
	if (ValC != DivR) printf(" failed");
	// rem
	for (int32 i = 0; i < Loops; i++){ R = D % B; }
	ValR = R.getNumAsUint64();
	if (ValR != RemR) Result = false;
	printf(MsgInit, Loops, "D % B", Info, (double)Time.getRuntime_us() / Loops);
	if (ValR != RemR) printf(" failed");
	printf("\n");

	// generate bignum
	int32 Chunks = 10;
	CmString BigA;
	CmString BigB;
	CmString BigD;
	for (int32 i = 0; i < Chunks; i++){
		BigA += CmString(A);
		BigB += CmString(B);
		BigD += CmString(A);
		BigD += CmString(B);
	}
	A.setNumber(BigA);
	B.setNumber(BigB);
	D.setNumber(BigD);
	int32 Digits = int32(A.getLength() + B.getLength());
	Info = "bignum 100 digits          ";

	Loops = 100;
	// sum
	for (int32 i = 0; i < Loops; i++){ R = A + B; }
	printf(MsgInit, Loops, " A + B", Info, (double)Time.getRuntime_us() / Loops);
	// dif
	for (int32 i = 0; i < Loops; i++){ R = A - B; }
	printf(MsgInit, Loops, " A - B", Info, (double)Time.getRuntime_us() / Loops);
	for (int32 i = 0; i < Loops; i++){ R = B - A; }
	printf(MsgInit, Loops, " B - A", Info, (double)Time.getRuntime_us() / Loops);
	// mul
	Loops = 10;
	for (int32 i = 0; i < Loops; i++){ R = A * B; }
	printf(MsgInit, Loops, "  A * B", Info, (double)Time.getRuntime_us() / Loops);
	// div
	Loops = 10;
	for (int32 i = 0; i < Loops; i++){ R = D / B; }
	printf(MsgInit, Loops, "  D / B", Info, (double)Time.getRuntime_us() / Loops);
	printf("\n");

	Chunks = 24;
	A.uint2String(ValA);
	B.uint2String(ValB);
	BigA.setLength(0);
	BigB.setLength(0);
	BigD.setLength(0);
	for (int32 i = 0; i < Chunks; i++){
		BigA += CmString(A);
		BigB += CmString(B);
		BigD += CmString(A);
		BigD += CmString(B);
	}
	A.setNumber(BigA);
	B.setNumber(BigB);
	D.setNumber(BigD);
	Digits = int32(A.getLength() + B.getLength());
	Info = "bignum 240 digits          ";

	Loops = 100;
	// sum
	for (int32 i = 0; i < Loops; i++){ R = A + B; }
	printf(MsgInit, Loops, " A + B", Info, double(Time.getRuntime_us()) / Loops);
	// dif
	for (int32 i = 0; i < Loops; i++){ R = A - B; }
	printf(MsgInit, Loops, " A - B", Info, double(Time.getRuntime_us()) / Loops);
	for (int32 i = 0; i < Loops; i++){ R = B - A; }
	printf(MsgInit, Loops, " B - A", Info, double(Time.getRuntime_us()) / Loops);
	// mul
	Loops = 10;
	for (int32 i = 0; i < Loops; i++){ R = A * B; }
	printf(MsgInit, Loops, "  A * B", Info, double(Time.getRuntime_us()) / Loops);
	// div
	Loops = 10;
	for (int32 i = 0; i < Loops; i++){ R = D / B; }
	printf(MsgInit, Loops, "  D / B", Info, double(Time.getRuntime_us()) / Loops);
	printf("\n");

	// RSA numbers
	int32 Count = 7;
	CmValueFTL RSA;
	CmValueFTL P;
	CmValueFTL Q;

	for (int32 i = 0; i < Count; i++){
		switch (i){
		case 0:	Info = "  RSA-100  330_bits  ";
			RSA.setNumber("1522605027922533360535618378132637429718068114961380688657908494580122963258952897654000350692006139");
			P.setNumber("37975227936943673922808872755445627854565536638199");
			Q.setNumber("40094690950920881030683735292761468389214899724061");
			break;
		case 1:	Info = "  RSA-130  430_bits  ";
			RSA.setNumber("1807082088687404805951656164405905566278102516769401349170127021450056662540244048387341127590812303371781887966563182013214880557");
			P.setNumber("39685999459597454290161126162883786067576449112810064832555157243");
			Q.setNumber("45534498646735972188403686897274408864356301263205069600999044599");
			break;
		case 2:	Info = "  RSA-150  496_bits  ";
			RSA.setNumber("155089812478348440509606754370011861770654545830995430655466945774312632703463465954363335027577729025391453996787414027003501631772186840890795964683");
			P.setNumber("348009867102283695483970451047593424831012817350385456889559637548278410717");
			Q.setNumber("445647744903640741533241125787086176005442536297766153493419724532460296199");
			break;
		case 3:	Info = "  RSA-174  576_bits  ";
			RSA.setNumber("188198812920607963838697239461650439807163563379417382700763356422988859715234665485319060606504743045317388011303396716199692321205734031879550656996221305168759307650257059");
			P.setNumber("398075086424064937397125500550386491199064362342526708406385189575946388957261768583317");
			Q.setNumber("472772146107435302536223071973048224632914695302097116459852171130520711256363590397527");
			break;
		case 4:	Info = "  RSA-200  663_bits  ";
			RSA.setNumber("27997833911221327870829467638722601621070446786955428537560009929326128400107609345671052955360856061822351910951365788637105954482006576775098580557613579098734950144178863178946295187237869221823983");
			P.setNumber("3532461934402770121272604978198464368671197400197625023649303468776121253679423200058547956528088349");
			Q.setNumber("7925869954478333033347085841480059687737975857364219960734330341455767872818152135381409304740185467");
			break;
		case 5:	Info = "  RSA-230  762_bits  ";
			RSA.setNumber("17969491597941066732916128449573246156367561808012600070888918835531726460341490933493372247868650755230855864199929221814436684722874052065257937495694348389263171152522525654410980819170611742509702440718010364831638288518852689");
			P.setNumber("3968132623150957588532394439049887341769533966621957829426966084093049516953598120833228447171744337427374763106901");
			Q.setNumber("4528450358010492026612439739120166758911246047493700040073956759261590397250033699357694507193523000343088601688589");
			break;
		case 6:	Info = "  RSA-232  768_bits  ";
			RSA.setNumber("1230186684530117755130494958384962720772853569595334792197322452151726400507263657518745202199786469389956474942774063845925192557326303453731548268507917026122142913461670429214311602221240479274737794080665351419597459856902143413");
			P.setNumber("33478071698956898786044169848212690817704794983713768568912431388982883793878002287614711652531743087737814467999489");
			Q.setNumber("36746043666799590428244633799627952632279158164343087642676032283815739666511279233373417143396810270092798736308917");
			break;
		case 7:	Info = "  RSA-470 1559_bits  ";
			RSA.setNumber("17051473784681185209081599238887028025183255852149159683588918369809675398036897711442383602526314519192366612270595815510311970886116763177669964411814095748660238871306469830461919135901638237924444074122866545522954536883748558744552128950445218096208188788876324395049362376806579941053305386217595984047709603954312447692725276887594590658792939924609261264788572032212334726855302571883565912645432522077138010357669555555071044090857089539320564963576770285413369");
			P.setNumber("3847538028593018472389908148350764205843210893478047089347180907511475367987843270908432737486765382147094348974217195367100435478567510397658349192715687404381714314578910938001438475193740863923847081748561931007453847100368970425127");
			Q.setNumber("4756842193475028547381937805637418507649354718093471513790852479097284019785319708523419734364718529043250897946518734805247385195493874049510890384576810894713854903815147089384708917138475910189580381197082438147053740853420198038475");
			break;

		default:
			break;
		}

		for (int32 i = 0; i < Loops; i++){ R = P * Q; }
		printf(MsgInit, Loops, Info, "P * Q        ", double(Time.getRuntime_us()) / Loops);
		if (R != RSA){ printf(" failed"); Result = false; }
		for (int32 i = 0; i < Loops; i++){ R = RSA / P; }
		printf(MsgInit, Loops, Info, "RSA/P        ", double(Time.getRuntime_us()) / Loops);
		if (R != Q){ printf(" failed"); Result = false; }
		printf("\n");
	}

	return Result;
}

const CmString& CmValueFTL::getAddress()
{ 
	return Address; 
}

CmValueFTL& CmValueFTL::getNextConfigValue()
{
	return NULL == Next ? *this : *Next;
}

CmValueFTL* CmValueFTL::getConfigValue(int32 _Index)
{
	CmValueFTL* ValueFTL = this;
	while (NULL != ValueFTL->Next && _Index-- > 0){
		ValueFTL = ValueFTL->Next;
	}

	return ValueFTL;
}

bool CmValueFTL::updateSettings(CmStringFTL& _StringFTL, CmString _StartAddr)
{
	// travel the list of contained values
	CmValueFTL* ValueFTL = this;
	do{
		// update value from StringFTL
		if (false == ValueFTL->updateFrom(_StringFTL, _StartAddr)){
			// address missing in StringFTL
			CmString Message("Settings failed with address '");
			Message += ValueFTL->getAddress();
			Message += "'.";
			throw CmException((void*)CMLOG_Error, Message.getText(), getText());
		}
		// next value
		ValueFTL = ValueFTL->Next;
	} while (NULL != ValueFTL);

	return true;
}
bool CmValueFTL::updateFrom(CmStringFTL& _StringFTL, CmString _StartAddr)
{
	// update value from specified address
	return _StringFTL.getValueX(Address, *this, _StartAddr);
}
bool CmValueFTL::updateFrom(CmStringFTL& _StringFTL, int32 _Index, CmString _StartAddr)
{
	// update value from specified address plus index
	CmString _Address = Address;
	_Address += "-";
	_Address += _Index;
	return _StringFTL.getValueX(_Address, *this);
}

bool CmValueFTL::syncSettings(CmStringFTL& _StringFTL)
{
	// travel the list of contained values
	CmValueFTL* ValueFTL = this;
	do{
		// synchronize StringFTL with current value
		CmString ValueX(ValueFTL->asStringFTL(""));
		if (false == _StringFTL.setValueX(ValueFTL->Address, ValueX)){ 
			// address missing in StringFTL
			CmString Message("Settings failed with address '");
			Message += ValueFTL->getAddress();
			Message += "'.";
			throw CmException((void*)CMLOG_Error, Message.getText(), getText());
		}
		// next value
		ValueFTL = ValueFTL->Next;
	} while (NULL != ValueFTL);

	return true;
}

bool CmValueFTL::updateInfoFTL(CmStringFTL& _InfoFTL, CmValueINI& _Return, CmString _StartAddr)
{
	// get return values
	CmValueFTL& _LogLevel = _Return.getLogLevel();
	CmValueFTL& _Message = _Return.getMessage();
	CmValueFTL& _Context = _Return.getContext();

	// check for pending error
	if (int32(_LogLevel) == CMLOG_Error) return false;
	_LogLevel = CMLOG_None;

	try{
		updateSettings(_InfoFTL, _StartAddr);
	}
	catch (CmException e){
		_LogLevel = int32(e.getData());
		_Message.setText(e.getMessage());
		_Context.setText(e.getContext());
		return false;
	}

	return true;
}

bool CmValueFTL::serializeValueFTL(CmString& _StringFTL, CmStringFTL& _TemplateFTL, CmValueINI& _Return)
{
	// get return values
	CmValueFTL& _LogLevel = _Return.getLogLevel();
	CmValueFTL& _Message = _Return.getMessage();
	CmValueFTL& _Context = _Return.getContext();

	// synchronize _TemplateFTL with the control ValueFTL array
	try{
		syncSettings(_TemplateFTL);
	}
	catch (CmException e){
		_LogLevel = int32(e.getData());
		_Message.setText(e.getMessage());
		_Context.setText(e.getContext());
		return false;
	}

	// generate StringFTL
	if (false == _TemplateFTL.serializeStringFTL(_StringFTL)) return false;

	return true;
}


bool CmValueFTL::writeInfoFTL(CmString _ConfigPath, CmStringFTL& _InfoFTL, CmValueINI& _Return, const int8* _NameExtension)
{
	// get return values
	CmValueFTL& _LogLevel = _Return.getLogLevel();
	CmValueFTL& _Message = _Return.getMessage();
	CmValueFTL& _Context = _Return.getContext();

	// check for pending error
	if (int32(_LogLevel) == CMLOG_Error) return false;
	_LogLevel = CMLOG_None;

	// check if config path exists
	if (false == PathFileExistsA(_ConfigPath.getText())){
		_LogLevel = CMLOG_Error;
		_Message.setText(_ConfigPath);
		_Context.setText("Config path not available (could not be created?):");
		return false;
	}

	// complete config filename
	CmString ConfigFile(_ConfigPath);
	CmString ConfigFileExtended;
	CmUURI UURI;
	UURI = getText();
	ConfigFile += UURI.getConfig();
	//ConfigFile += _AppProfile;
	//ConfigFile += "_";
	//ConfigFile += _AppVersion;
	ConfigFile += ".ftl";
	if (_NameExtension != NULL){
		ConfigFileExtended = ConfigFile;
		ConfigFileExtended += "_";
		ConfigFileExtended += _NameExtension;
		ConfigFileExtended += ".ftl";
	}

	// synchronize ProFTL with Profile values
	try{
		syncSettings(_InfoFTL);
	}
	catch (CmException e){
		_LogLevel = int32(e.getData());
		_Message.setText(e.getMessage());
		_Context.setText(e.getContext());
		return false;
	}

	// generate config string from profile
	CmString ConfigString;
	_InfoFTL.serializeStringFTL(ConfigString);

	// write application profile to disk
	if (false == ConfigString.writeBinary(ConfigFile.getText())){
		_LogLevel = CMLOG_Error;
		_Message.setText(_ConfigPath);
		_Context.setText("Config file could not be written:");
	}
	if (ConfigFileExtended.getLength() > 0){
		if (false == ConfigString.writeBinary(ConfigFileExtended.getText())){
			_LogLevel = CMLOG_Error;
			_Message.setText(_ConfigPath);
			_Context.setText("Config file (extended) could not be written:");
		}
	}

	// wait for folder synchronisation
	Sleep(100);
	return true;
}
bool CmValueFTL::readInfoFTL(CmString _ConfigPath, CmValueINI& _Return, const int8* _NameExtension)
{
	// get return values
	CmValueFTL& _LogLevel = _Return.getLogLevel();
	CmValueFTL& _Message = _Return.getMessage();
	CmValueFTL& _Context = _Return.getContext();

	// check for pending error
	if (int32(_LogLevel) == CMLOG_Error) return false;
	_LogLevel = CMLOG_None;

	// complete config filename
	CmString ConfigFile(_ConfigPath);
	CmUURI UURI;
	UURI = getText();
	ConfigFile += UURI.getConfig();
	//ConfigFile += _AppProfile;
	//ConfigFile += "_";
	//ConfigFile += _AppVersion;
	if (_NameExtension != NULL){
		ConfigFile += "_";
		ConfigFile += _NameExtension;
	}
	ConfigFile += ".ftl";

	// check file availability
	_Message.setText("- new version -");
	if (false == isFileExists(ConfigFile.getText())) return false;
	// read profile from disk
	try{
		CmString ConfigString;
		CmStringFTL StringFTL;
		ConfigString.readBinary(ConfigFile.getText());
		StringFTL.processStringFTL(ConfigString);

		// update application profile 
		_Message.setText("message");
		if (false == updateInfoFTL(StringFTL, _Return)){
			// DIAG: write failed StringFTL to disk
			ConfigFile += "_failed.txt";
			CmString FailedStringFTL;
			StringFTL.serializeStringFTL(FailedStringFTL);
			FailedStringFTL.writeBinary(ConfigFile.getBuffer());
			return false;
		}
	}
	catch (...){
		CmString Message("Config file '");
		Message += ConfigFile;
		Message += "' failed";
		_LogLevel = CMLOG_Error;
		_Message.setText(Message.getText());
		_Context.setText(getText());
		return false;
	}

	return true;
}
bool CmValueFTL::determineListOfProfiles(CmString _ConfigPath, CmString _AppProfile, CmString _AppVersion)
{
	// get list of available profiles
	CmLString ProfileList;

	// profile name
	CmString ProfileName(_AppProfile);
	ProfileName += "_";
	ProfileName += _AppVersion;
	// path to configuration profiles
	CmString ProfilePath(_ConfigPath);
	ProfilePath += "\\*";

	// get all files on current path
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	CmString Filename;
	CmString Extension;
	hFind = FindFirstFileA(ProfilePath.getText(), &FindFileData);
	while (hFind != INVALID_HANDLE_VALUE){
		// check if item is a file
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0){
			// add the extension of a found configuration file to the result list
			Filename = FindFileData.cFileName;
			if (Filename >= ProfileName){
				Extension.assignSubString(int32(Filename.getLength()), &Filename, 1 + int32(ProfileName.getLength()));
			}
			Extension.getLength() > 0 ? ProfileList.addString(Extension) : 0;
		}
		// find next item
		if (false == FindNextFileA(hFind, &FindFileData)) break;
	}
	hFind != INVALID_HANDLE_VALUE ? FindClose(hFind) : 0;


	CmMatrix& M = getMatrix();

	// [']=update, [i]=profile

	for (int32 i = 0; i < ProfileList.getSize(); i++){
		// add profile to matrix
		M(i) = ProfileList[i];
	}
	// set update flag true
	M() = true;

	return true;
}

//------Information-----------------------------------------------------------
bool CmValueFTL::updateInformation(CmString& _Information, CmValueINI& _Return)
{
	// get return values
	CmValueFTL& _LogLevel = _Return.getLogLevel();
	//CmValueFTL& _Message = _Return.getMessage();
	//CmValueFTL& _Context = _Return.getContext();

	// generate a CmStringFTL hierarchy
	CmStringFTL InfoFTL;
	InfoFTL.processStringFTL(_Information);

	_LogLevel = CMLOG_None;
	return updateInfoFTL(InfoFTL, _Return);
}

/** operator=(). Write access to CmValueFTL */
const CmStringFTL& CmValueFTL::operator=(const CmStringFTL& /*_StringFTL*/)
{
	// ToDo

	return *this;
}
const CmString& CmValueFTL::operator=(const CmString& _String)
{
	CmString::setText(_String);
	DataFormat = DATAFORMAT_STRING;
	TypeX = TYPEFTL_BINX;
	// delete TextX
	if (NULL != TextX) {
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}
	return _String;
}
const int8* CmValueFTL::operator=(const int8* _String)
{
	CmString::setText(_String);
	DataFormat = DATAFORMAT_STRING;
	TypeX = TYPEFTL_BINX;
	// delete TextX
	if (NULL != TextX) {
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}
	return _String;
}
int8 CmValueFTL::operator = (int8 _Val8)
{
	CmString::setValue(_Val8);
	DataFormat = DATAFORMAT_INT8;
	TypeX = TYPEFTL_BINX;
	// delete TextX
	if (NULL != TextX) {
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}
	return _Val8;
}
uint8 CmValueFTL::operator = (uint8 _Val8)
{
	CmString::setValue(_Val8);
	DataFormat = DATAFORMAT_UINT8;
	TypeX = TYPEFTL_BINX;
	// delete TextX
	if (NULL != TextX) {
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}
	return _Val8;
}
bool CmValueFTL::operator = (bool _ValBool)
{
	CmString::setValue(_ValBool);
	DataFormat = DATAFORMAT_BOOL;
	TypeX = TYPEFTL_BINX;
	// delete TextX
	if (NULL != TextX) {
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}
	return _ValBool;
}
int16 CmValueFTL::operator = (int16 _Val16)
{
	CmString::setValue(_Val16);
	DataFormat = DATAFORMAT_INT16;
	TypeX = TYPEFTL_BINX;
	// delete TextX
	if (NULL != TextX) {
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}
	return _Val16;
}
uint16 CmValueFTL::operator = (uint16 _Val16)
{
	CmString::setValue(_Val16);
	DataFormat = DATAFORMAT_UINT16;
	TypeX = TYPEFTL_BINX;
	// delete TextX
	if (NULL != TextX) {
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}
	return _Val16;
}
int32 CmValueFTL::operator = (int32 _Val32)
{
	CmString::setValue(_Val32);
	DataFormat = DATAFORMAT_INT32;
	TypeX = TYPEFTL_BINX;
	// delete TextX
	if (NULL != TextX) {
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}
	return _Val32;
}
uint32 CmValueFTL::operator = (uint32 _Val32)
{
	CmString::setValue(_Val32);
	DataFormat = DATAFORMAT_UINT32;
	TypeX = TYPEFTL_BINX;
	// delete TextX
	if (NULL != TextX) {
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}
	return _Val32;
}
float CmValueFTL::operator = (float _ValFloat)
{
	CmString::setValue(_ValFloat);
	DataFormat = DATAFORMAT_FLOAT;
	TypeX = TYPEFTL_BINX;
	// delete TextX
	if (NULL != TextX) {
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}
	return _ValFloat;
}
int64 CmValueFTL::operator = (int64 _Val64)
{
	CmString::setValue(_Val64);
	DataFormat = DATAFORMAT_INT64;
	TypeX = TYPEFTL_BINX;
	// delete TextX
	if (NULL != TextX) {
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}
	return _Val64;
}
uint64 CmValueFTL::operator = (uint64 _Val64)
{
	CmString::setValue(_Val64);
	DataFormat = DATAFORMAT_UINT64;
	TypeX = TYPEFTL_BINX;
	// delete TextX
	if (NULL != TextX) {
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}
	return _Val64;
}
double CmValueFTL::operator = (double _ValDouble)
{
	CmString::setValue(_ValDouble);
	DataFormat = DATAFORMAT_DOUBLE;
	TypeX = TYPEFTL_BINX;
	// delete TextX
	if (NULL != TextX) {
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}
	return _ValDouble;
}
void* CmValueFTL::operator = (void* _ValPointer)
{
	CmString::setValue(_ValPointer);
	DataFormat = DATAFORMAT_BINARY;
	TypeX = TYPEFTL_BINX;
	// delete TextX
	if (NULL != TextX) {
		releaseMemory<uint8>(TextX, (int32)LengthX + 1, isUint8);
		TextX = NULL;
		LengthX = 0;
	}
	return _ValPointer;
}

#ifdef CM_TYPE_OLD_STYLE
void CmValueFTL::setValue(int8 _Val8)
{
	operator = (_Val8);
}
void CmValueFTL::setValue(uint8 _Val8)
{
	operator = (_Val8);
}
void CmValueFTL::setValue(bool _ValBool)
{
	operator = (_ValBool);
}
void CmValueFTL::setValue(int16 _Val16)
{
	operator = (_Val16);
}
void CmValueFTL::setValue(uint16 _Val16)
{
	operator = (_Val16);
}
void CmValueFTL::setValue(int32 _Val32)
{
	operator = (_Val32);
}
void CmValueFTL::setValue(uint32 _Val32)
{
	operator = (_Val32);
}
void CmValueFTL::setValue(float _ValFloat)
{
	operator = (_ValFloat);
}
void CmValueFTL::setValue(int64 _Val64)
{
	operator = (_Val64);
}
void CmValueFTL::setValue(uint64 _Val64)
{
	operator = (_Val64);
}
void CmValueFTL::setValue(double _ValDouble)
{
	operator = (_ValDouble);
}
void CmValueFTL::setValue(void* _ValPointer){
	operator = (_ValPointer);
}
#endif 

CmString CmValueFTL::asStringFTL(CmString _Prefix, int32 _Precision, int32 _Digits)
{
	// generate FTL string
	double ValDouble;
	CmString Val;
	switch (DataFormat){
	case DATAFORMAT_NONE:					Val = getText(); break;
	case DATAFORMAT_FTLight: 
		switch (TypeX){
		case TYPEFTL_OPERATOR:				Val = getText(); break;
		case TYPEFTL_LOCATION:				Val = getText(); break;
		case TYPEFTL_IDENTIFIER:			Val = getText(); break;
		case TYPEFTL_QUERY:						Val = getText(); break;
		case TYPEFTL_TEXT:						Val = getText(); break;
		case TYPEFTL_TEXT_BINX:				Val = getText(); break;
		case TYPEFTL_NUMBER:					ValDouble = getNumAsDouble(); isConversionError ? Val = getText() : Val.double2String(ValDouble, _Precision > -1 ? _Precision : estimatePrecision(ValDouble), _Digits); break;
		case TYPEFTL_NUMBER_BINX:			ValDouble = getNumAsDouble(); isConversionError ? Val = getText() : Val.double2String(ValDouble, _Precision > -1 ? _Precision : estimatePrecision(ValDouble), _Digits); break;
		case TYPEFTL_Local:				Val = ""; break;
		case TYPEFTL_Local_BINX:	Val = ""; break;
		case TYPEFTL_FIXED_PARENTS:		Val = ""; break;
		case TYPEFTL_CHECKSUM:				Val = ""; break;
		case TYPEFTL_OPTION:					Val = getText(); break;
		case TYPEFTL_DEFAULT:					Val = ""; break;
		case TYPEFTL_COMMENT:					Val = getText(); break;
		case TYPEFTL_BINX:						Val = ""; break;
		case TYPEFTL_CONTROLX:				Val = ""; break;
		case TYPEFTL_FTLightOPEN:			Val = ""; break;
		case TYPEFTL_FTLightWRAP:			Val = ""; break;
		case TYPEFTL_BINMCL:					Val = ""; break;
		case TYPEFTL_BINXBINARY:			Val = ""; break;
		case TYPEFTL_BINXSTRING:			Val = ""; break;
		case TYPEFTL_BINXVALUE:				Val = ""; break;
		case TYPEFTL_BINXTIME:				Val = ""; break;
		case TYPEFTL_CMXTOKEN:				Val = ""; break;
		case TYPEFTL_CMXLINK:					Val = ""; break;
		default: 										Val = ""; break;
		}
		break;
	case DATAFORMAT_STRING:				Val = getText(); break;
	case DATAFORMAT_BINARY:				Val.uint2hex(uint64(*this)); break;
	case DATAFORMAT_UINT64:				Val.uint2hex(uint64(*this)); break;
	case DATAFORMAT_INT64:				Val.double2String((double)int64(*this)); break;
	case DATAFORMAT_DOUBLE:				Val.double2String(double(*this), _Precision, _Digits); break;
	case DATAFORMAT_UINT32:				Val.double2String(int32(*this)); break;
	case DATAFORMAT_INT32:				Val.double2String(int32(*this)); break;
	case DATAFORMAT_FLOAT:				break;
	case DATAFORMAT_UINT16:				Val.double2String(uint16(*this)); break;
	case DATAFORMAT_INT16:				Val.double2String(int16(*this)); break;
	case DATAFORMAT_UINT8:				Val.double2String(uint8(*this)); break;
	case DATAFORMAT_INT8:					Val.double2String(int8(*this)); break;
	case DATAFORMAT_BOOL:					Val = bool(*this) ? "1" : "0"; break;
	}
	_Prefix += Val;

	return _Prefix;
}

CmString CmValueFTL::asStringFTLbool(CmString _Prefix)
{
	// generate FTL string
	CmString Val(bool(*this) ? "1" : "0");
	_Prefix += Val;

	return _Prefix;
}

#ifdef CM_TYPE_OLD_STYLE
CmString& CmValueFTL::asStringConvert(int32 _Precision, int32 _Digits)
{
	if (false == asDataFormat(DATAFORMAT_STRING, _Precision, _Digits)) this->setText("<unknown>");
	return *this;
}
const int8*  CmValueFTL::getText(){
	return operator const int8*();
}
void* CmValueFTL::asPointer(){
	return operator void*();
}
uint64 CmValueFTL::asUInt64(){
	return operator uint64();
}
int64 CmValueFTL::asInt64(){
	return operator int64();
}
double CmValueFTL::asDouble(){
	return operator double();
}
uint32 CmValueFTL::asUInt32(){
	return operator uint32();
}
int32 CmValueFTL::asInt32(){
	return operator int32();
}
float CmValueFTL::asFloat(){
	return operator float();
}
uint16 CmValueFTL::asUInt16(){
	return operator uint16();
}
int16 CmValueFTL::asInt16(){
	return operator int16();
}
uint8 CmValueFTL::asUInt8(){
	return operator uint8();
}
int8 CmValueFTL::asInt8(){
	return operator int8();
}
bool CmValueFTL::asBool(){
	return operator bool();
}
#endif

/** explicit type conversion */
CmValueFTL::operator CmString&(){
	return asStringConvert();
}
CmValueFTL::operator const int8*(){
	return asStringConvert().getText();
}
CmValueFTL::operator void*(){
	if (false == asDataFormat(DATAFORMAT_UINT64)) return NULL;
	return *(void**)getBinary();
}
CmValueFTL::operator uint64(){
	if (false == asDataFormat(DATAFORMAT_UINT64)) return 0;
	return *(uint64*)getBinary();
}
CmValueFTL::operator int64(){
	if (false == asDataFormat(DATAFORMAT_INT64)) return 0;
	return *(int64*)getBinary();
}
CmValueFTL::operator double(){
	if (false == asDataFormat(DATAFORMAT_DOUBLE)) return 0;
	return *(double*)getBinary();
}
CmValueFTL::operator uint32(){
	if (false == asDataFormat(DATAFORMAT_UINT32)) return 0;
	return *(uint32*)getBinary();
}
CmValueFTL::operator int32(){
	if (false == asDataFormat(DATAFORMAT_INT32)) return 0;
	return *(int32*)getBinary();
}
CmValueFTL::operator float(){
	if (false == asDataFormat(DATAFORMAT_FLOAT)) return 0;
	return *(float*)getBinary();
}
CmValueFTL::operator uint16(){
	if (false == asDataFormat(DATAFORMAT_UINT16)) return 0;
	return *(uint16*)getBinary();
}
CmValueFTL::operator int16(){
	if (false == asDataFormat(DATAFORMAT_INT16)) return 0;
	return *(int16*)getBinary();
}
CmValueFTL::operator uint8(){
	if (false == asDataFormat(DATAFORMAT_UINT8)) return 0;
	return *(uint8*)getBinary();
}
CmValueFTL::operator int8(){
	if (false == asDataFormat(DATAFORMAT_INT8)) return 0;
	return *(int8*)getBinary();
}
CmValueFTL::operator bool(){
	if (false == asDataFormat(DATAFORMAT_BOOL)) return false;
	return *(bool*)getBinary();
}

/** compare operators */
// string
bool CmValueFTL::operator == (const int8* _String){
	if (NULL == _String) {
		return operator const int8*() == NULL ? true : false;
	}
	return operator CmString&() == _String;
}
bool CmValueFTL::operator != (const int8* _String){
	if (NULL == _String) {
		return operator const int8*() != NULL ? true : false;
	}
	return operator CmString&() != _String;
}
bool CmValueFTL::operator == (CmString& _String){
	return operator CmString&() == _String;
}
bool CmValueFTL::operator != (CmString& _String){
	return operator CmString&() != _String;
}
// double
bool CmValueFTL::operator == (double _Value){
	return operator double() == _Value;
}
bool CmValueFTL::operator != (double _Value){
	return operator double() != _Value;
}
bool CmValueFTL::operator > (double _Value){
	return operator double() > _Value;
}
bool CmValueFTL::operator < (double _Value){
	return operator double() < _Value;
}
// int
bool CmValueFTL::operator == (int64 _Value){
	return operator int64() == _Value;
}
bool CmValueFTL::operator != (int64 _Value){
	return operator int64() != _Value;
}
bool CmValueFTL::operator > (int64 _Value){
	return operator int64() > _Value;
}
bool CmValueFTL::operator < (int64 _Value){
	return operator int64() < _Value;
}
// uint
bool CmValueFTL::operator == (uint64 _Value){
	return operator uint64() == _Value;
}
bool CmValueFTL::operator != (uint64 _Value){
	return operator uint64() != _Value;
}
bool CmValueFTL::operator > (uint64 _Value){
	return operator uint64() > _Value;
}
bool CmValueFTL::operator < (uint64 _Value){
	return operator uint64() < _Value;
}
// bool
bool CmValueFTL::operator == (bool _Value){
	return operator bool() == _Value;
}
bool CmValueFTL::operator != (bool _Value){
	return operator bool() != _Value;
}

// large numbers operators
CmString& CmValueFTL::setNumber(const CmString& _Num)
{
	*this = _Num;
	this->TypeX = TYPEFTL_NUMBER;

	return *this;
}
// comparison
bool CmValueFTL::operator > (const CmValueFTL& _Value)
{
	// only numbers (strings) are evaluated
	if ((TypeX != TYPEFTL_NUMBER) || (_Value.TypeX != TYPEFTL_NUMBER)) return false;

	if (getLength() < _Value.getLength()) return false;
	if (getLength() > _Value.getLength()) return true;

	return strcmp(getBuffer(), _Value.getBuffer()) > 0 ? true : false;
}
bool CmValueFTL::operator < (const CmValueFTL& _Value)
{
	// only numbers (strings) are evaluated
	if ((TypeX != TYPEFTL_NUMBER) || (_Value.TypeX != TYPEFTL_NUMBER)) return false;

	if (getLength() < _Value.getLength()) return true;
	if (getLength() > _Value.getLength()) return false;

	return strcmp(getBuffer(), _Value.getBuffer()) < 0 ? true : false;
}
bool CmValueFTL::operator >= (const CmValueFTL& _Value)
{
	// only numbers (strings) are evaluated
	if ((TypeX != TYPEFTL_NUMBER) || (_Value.TypeX != TYPEFTL_NUMBER)) return false;

	if (getLength() < _Value.getLength()) return false;
	if (getLength() > _Value.getLength()) return true;

	return strcmp(getBuffer(), _Value.getBuffer()) >= 0 ? true : false;
}
bool CmValueFTL::operator <= (const CmValueFTL& _Value)
{
	// only numbers (strings) are evaluated
	if ((TypeX != TYPEFTL_NUMBER) || (_Value.TypeX != TYPEFTL_NUMBER)) return false;

	if (getLength() < _Value.getLength()) return true;
	if (getLength() > _Value.getLength()) return false;

	return strcmp(getBuffer(), _Value.getBuffer()) <= 0 ? true : false;
}
// arithmetic operations
CmString CmValueFTL::operator + (const CmValueFTL& _B)
{
	// only numbers (strings) are accepted
	CmString Result("sum");
	if ((TypeX != TYPEFTL_NUMBER) || (_B.TypeX != TYPEFTL_NUMBER)) return Result;

	// determine size
	int32 LenA = int32(getLength());
	int32 LenB = int32(_B.getLength());
	int32 LenResult = max(LenA, LenB) + 1;
	Result.setLength(LenResult);

	// Result = A
	for (int32 r = LenResult, a = LenA; r >= 0; r--, a--){
		memset(Result.getBuffer() + r, a >= 0 ? CmString(*this)[a] : '0', 1);
	}
	// add B
	for (int32 b = LenB - 1; b >= 0; b--){
		int32 B = CmString(_B)[b] - '0';
		for (int32 r = LenResult - (LenB - b); r >= 0; r--){
			memset(Result.getBuffer() + r, '0' + (B += (Result[r] - '0')) % 10, 1);
			if ((B /= 10) == 0) break;
		}
	}

	// remove leading zero if needed
	if (Result[0] == '0'){
		Result.assignSubString(int32(Result.getLength() - 1), 0, 1);
	}

	return Result;
}
CmString& CmValueFTL::operator+= (const CmValueFTL& _B)
{
	setNumber(operator+(_B));
	return *this;
}
CmString CmValueFTL::operator - (const CmValueFTL& _B)
{
	// only number (strings) are accepted
	CmString Result("dif");
	if ((TypeX != TYPEFTL_NUMBER) || (_B.TypeX != TYPEFTL_NUMBER)) return Result;

	// handle sign
	bool isNegative = this->operator>=(_B) ? false : true;
	const CmValueFTL& A = isNegative ? _B : *this;
	const CmValueFTL& B = isNegative ? *this : _B;

	// determine size
	int32 LenA = int32(A.getLength());
	int32 LenB = int32(B.getLength());
	int32 LenResult = max(LenA, LenB) + 1;
	Result.setLength(LenResult);

	// Result = A
	for (int32 r = LenResult, a = LenA; r >= 0; r--, a--){
		memset(Result.getBuffer() + r, a >= 0 ? CmString(A)[a] : '0', 1);
	}
	// sub B
	for (int32 b = LenB - 1; b >= 0; b--){
		int32 ValB = CmString(B)[b] - '0';
		int32 Carry = 0;
		for (int32 r = LenResult - (LenB - b); r > 0; r--){
			int32 ValR = Result[r] - '0' - Carry;
			Carry = ValR < ValB ? 1 : 0;
			int32 Digit = ValR - ValB + Carry * 10;
			memset(Result.getBuffer() + r, '0' + Digit, 1);
			if (Carry == 0){
				break;
			}
			else { ValB = 0; }
		}
	}

	// remove leading zero if needed
	if (Result[0] == '0'){
		int32 LenZ;
		for (LenZ = 1; LenZ < int32(Result.getLength()); LenZ++){
			if (Result[LenZ] != '0') break;
		}
		LenZ >= int32(Result.getLength()) ? LenZ = int32(Result.getLength() - 1) : 0;
		Result.assignSubString(int32(Result.getLength() - 1), 0, LenZ);
	}
	if (isNegative){
		CmString Value = Result;
		Result = "-";
		Result += Value;
	}

	return Result;
}
CmString& CmValueFTL::operator-= (const CmValueFTL& _B)
{
	setNumber(operator-(_B));
	return *this;
}
CmString CmValueFTL::operator * (const CmValueFTL& _B)
{
	// only number (strings) are accepted
	CmString Result("0");
	if ((TypeX != TYPEFTL_NUMBER) || (_B.TypeX != TYPEFTL_NUMBER)) return Result;

	// determine size
	int32 LenA = int32(getLength());
	int32 LenB = int32(_B.getLength());
	int32 LenResult = LenA + LenB;
	Result.setLength(LenResult);
	memset(Result.getBuffer(), '0', LenResult);

	/** long multiplication:
	*   - every digit of operand A will be multiplied by each digit of operand B
	*   - the sum of particular products yields the result
	*/
	for (int32 a = LenA - 1; a >= 0; a--){
		for (int32 b = LenB - 1; b >= 0; b--){
			int32 A = CmString(*this)[a] - '0';
			int32 B = CmString(_B)[b] - '0';
			int32 P = A*B;
			for (int32 r = a + b + 1; r >= 0; r--){
				memset(Result.getBuffer() + r, '0' + (P += (Result[r] - '0')) % 10, 1);
				if((P /= 10) == 0) break;
			}
		}
	}

	return Result;
}
CmString& CmValueFTL::operator*= (const CmValueFTL& _B)
{
	setNumber(operator*(_B));
	return *this;
}
CmString CmValueFTL::operator / (const CmValueFTL& _B)
{
	// only number (strings) are accepted
	CmString Result("0");
	if ((TypeX != TYPEFTL_NUMBER) || (_B.TypeX != TYPEFTL_NUMBER)) return Result;

	// initialize result
	int32 LenR = int32(getLength() - _B.getLength() + 1);
	LenR > 0 ? Result.fill(LenR, '0') : 0;

	// return remainder immediately if A < B
	if (*this < _B){
		Remainder = getText();
		return Result;
	}

	// operands
	CmString Val;
	CmValueFTL A; 
	A.setNumber(*this);
	CmVector<CmValueFTL*> B(10);
	for (int32 i = 0; i < 10; i++){
		B[i] = new CmValueFTL;
		if (i == 0){
			Val.fill(LenR, '0');
			B[i]->setNumber(Val);
		}
		if (i == 1) {
			Val = _B;
			Val += CmString(*B[0]);
			B[i]->setNumber(Val); 
		}
		if (i > 1) B[i]->setNumber(*B[i - 1] + *B[1]);
		// TEST
		//Val.setText(*B[i]);
	} 

	// subtract divisor
	for (int32 i = 0; i < LenR; i++){
		// decrement exponent
		for (int32 e = 0; e < 10; e++){
			B[e]->adjustLength(B[e]->getLength() - 1);
			// TEST
			Val.setText(*B[e]);
		}
		if (B[0]->getLength() == 0){ B[0]->setNumber("0"); }

		// TEST
		//Val.setText(A);

		// btree compare
		int32 Digit = 0;
		if (*B[5] > A){
			if (*B[2] > A){
				if (*B[1] > A){
					Digit = 0;
				}	else{	Digit = 1; }
			}	else if (*B[4] > A){
				if (*B[3] > A){
					Digit = 2; 
				}else{ Digit = 3; }
			}else{ Digit = 4;	}
		}	else if (*B[8] > A){
			if (*B[7] > A){
				if (*B[6] > A){
					Digit = 5;
				}else{ Digit = 6;	}
			}else{ Digit = 7;	}
		}	else	if (*B[9] > A){
			Digit = 8;
		}else{ Digit = 9;	}

		// subtract and set result
		A -= *B[Digit];

		// TEST
		//Val.setText(A);

		memset(Result.getBuffer() + i, '0' + Digit, 1);
	}

	// set remainder
	Remainder = A;

	// purge operand B
	for (int32 i = 0; i < 10; i++){
		delete B[i];
	}

	// remove leading zero if needed
	if (Result[0] == '0'){
		Result.assignSubString(int32(Result.getLength() - 1), 0, 1);
	}

	return Result;
}
CmString& CmValueFTL::operator/= (const CmValueFTL& _B)
{
	setNumber(operator/(_B));
	return *this;
}
CmString CmValueFTL::operator % (const CmValueFTL& _B)
{
	// only number (strings) are accepted
	CmString Result("rem");
	if ((TypeX != TYPEFTL_NUMBER) || (_B.TypeX != TYPEFTL_NUMBER)) return Result;

	// get remainder after diRemote
	operator/(_B);
	Result = Remainder;

	return Result;
}
CmString& CmValueFTL::getRemainder()
{
	return Remainder;
}
// string operations
void CmValueFTL::operator+=(const CmString& _String)
{
	CmString::operator+=(_String);
}
void CmValueFTL::operator+=(const int8* _String)
{
	CmString::operator+=(_String);
}
void CmValueFTL::operator+=(int32 _Value)
{
	CmString::operator+=(_Value);
}
void CmValueFTL::operator+=(uint32 _Value)
{
	CmString::operator+=(_Value);
}
void CmValueFTL::operator+=(int64 _Value)
{
	CmString::operator+=(_Value);
}
void CmValueFTL::operator+=(uint64 _Value)
{
	CmString::operator+=(_Value);
}

bool CmValueFTL::asDataFormat(CmDataFormat _DataFormat, int32 _Precision, int32 _Digits)
{
	// reject conversion if data format is unknown
	if (DATAFORMAT_NONE == DataFormat) return false;

	// check if requested format matches current format
	if (_DataFormat == DataFormat) return true;

	// provide a string representation
	if (DATAFORMAT_STRING == _DataFormat && TYPEFTL_NUMBER == TypeX && NULL != TextX && 0 != LengthX){
		// restore string representaion of a numeric value
		setLength(LengthX);
		memcpy(getBuffer(), TextX, LengthX);
	}
	else if (DATAFORMAT_STRING == _DataFormat && DATAFORMAT_FTLight == DataFormat){
		// leave string content unchanged
	}
	// convert FTLight/numeric format
	else if (DATAFORMAT_FTLight == DataFormat && TYPEFTL_NUMBER == TypeX){
		// number string goes to TextX, binary format is written to Text
		if (false == decodeNum(_DataFormat)) return false;
	}
	else if (DATAFORMAT_FTLight != DataFormat && TYPEFTL_NUMBER == TypeX){
		// check for string representation in TextX field
		if (NULL != TextX && 0 != LengthX){
			// restore string representation
			setLength((uint32)LengthX);
			memcpy(getBuffer(), TextX, LengthX);
		}
		// binary format is written to Text, number string is preserved in TextX
		if (false == decodeNum(_DataFormat)) return false;
	}
	// convert FTLight/binary format
	else if (TYPEFTL_BINX == TypeX || (0==LengthX && NULL==TextX)){
		// check for binary data
		if (DATAFORMAT_BINARY != DataFormat){
			// conversion to 'uint64' from any other format is not supported (can not be done exactly using an intermediate 'double')
			if (DATAFORMAT_UINT64 == _DataFormat) return false;
			// put current value into a 'double' bit field and save it as BinX in TextX
			if (NULL == TextX && 0 == LengthX){
				switch (DataFormat){
				case DATAFORMAT_DOUBLE: encodeNumX(double(*this));  break;
				case DATAFORMAT_UINT64: encodeNumX((double)uint64(*this));  break;
				case DATAFORMAT_INT64: encodeNumX((double)int64(*this));  break;
				case DATAFORMAT_UINT32: encodeNumX((double)uint32(*this));  break;
				case DATAFORMAT_INT32: encodeNumX((double)int32(*this));  break;
				case DATAFORMAT_FLOAT: encodeNumX((double)float(*this));  break;
				case DATAFORMAT_UINT16: encodeNumX((double)uint16(*this));  break;
				case DATAFORMAT_INT16: encodeNumX((double)int16(*this));  break;
				case DATAFORMAT_UINT8: encodeNumX((double)uint8(*this));  break;
				case DATAFORMAT_INT8: encodeNumX((double)int8(*this));  break;
				case DATAFORMAT_BOOL: encodeNumX((double)bool(*this));  break;
				default: break;
				}
				// TypeX is now BinX
				0 != LengthX ? TypeX = TYPEFTL_BINX : 0;
			}
			// restore a 'double' bit field in Text from BinX, preserve BinX representation in TextX
			decodeBinX(TextX, LengthX);
			// special case 'uint64': convert 'double'
			if (DATAFORMAT_UINT64 == _DataFormat){
				*(uint64*)getBuffer() = (uint64)(*(double*)getBuffer());
			}
		}
		// check length
		if (getLength() < 8) return false;
		// leave data for uint64 format as it is, all other formats: 
		// assume that the binary data represents a double value
		double Value = *(double*)getBuffer();
		// convert double to requested format
		switch (_DataFormat){
		case DATAFORMAT_STRING: double2String(Value, _Precision > -1 ? _Precision : estimatePrecision(Value), _Digits); break;
		case DATAFORMAT_INT64: *(int64*)getBuffer() = (int64)Value; break;
		case DATAFORMAT_DOUBLE: *(double*)getBuffer() = (double)Value; break;
		case DATAFORMAT_UINT32: *(uint32*)getBuffer() = (uint32)Value; break;
		case DATAFORMAT_INT32: *(int32*)getBuffer() = (int32)Value; break;
		case DATAFORMAT_FLOAT: *(float*)getBuffer() = (float)Value; break;
		case DATAFORMAT_UINT16: *(uint16*)getBuffer() = (uint16)Value; break;
		case DATAFORMAT_INT16: *(int16*)getBuffer() = (int16)Value; break;
		case DATAFORMAT_UINT8: *(uint8*)getBuffer() = (uint8)Value; break;
		case DATAFORMAT_INT8: *(int8*)getBuffer() = (int8)Value; break;
		case DATAFORMAT_BOOL: *(bool*)getBuffer() = fabs(Value) > MIN_DOUBLE; break;
			break;
		default: break;
		}
	}

	// remember new data format
	DataFormat = _DataFormat;

	return true;
}
int32 CmValueFTL::estimatePrecision(double _Value, int32 _ValidDigits)
{
	int32 Exponent = _Value < 1.7e-308 ? 0 : (int32)floor(log10(_Value)) + 1;
	int32 Precision = Exponent >= _ValidDigits ? 0 : _ValidDigits - Exponent;
	Precision = _Value > 1.7e-308 ? Precision : 0;

	return Precision;
}

//--------CmMatrixFTL-integration---------------------------------------------
bool CmValueFTL::clearMatrix()
{
	return NULL == Matrix ? true : getMatrix().clearMatrix();
}

#ifdef CM_TYPE_OLD_STYLE

int32 CmValueFTL::getVectorLength(CmIndex& _Index)
{
	return NULL == Matrix ? 0 : getMatrix()[_Index].getSizeLastLevel();
}
int32 CmValueFTL::getVectorLength()
{
	CmIndex Index;
	return NULL == Matrix ? 0 : getMatrix()[Index].getSizeLastLevel();
}
double CmValueFTL::operator[](CmIndex& _Index)
{
	return NULL == Matrix ? 0 : getMatrix().operator[](_Index);
}
bool CmValueFTL::setValue(int8 _Val8, CmIndex& _Index)
{
	return getMatrix().setValue(_Val8, _Index);
}
bool CmValueFTL::setValue(uint8 _Val8, CmIndex& _Index)
{
	return getMatrix().setValue(_Val8, _Index);
}
bool CmValueFTL::setValue(bool _ValBool, CmIndex& _Index)
{
	return getMatrix().setValue(_ValBool, _Index);
}
bool CmValueFTL::setValue(int16 _Val16, CmIndex& _Index)
{
	return getMatrix().setValue(_Val16, _Index);
}
bool CmValueFTL::setValue(uint16 _Val16, CmIndex& _Index)
{
	return getMatrix().setValue(_Val16, _Index);
}
bool CmValueFTL::setValue(int32 _Val32, CmIndex& _Index)
{
	return getMatrix().setValue(_Val32, _Index);
}
bool CmValueFTL::setValue(uint32 _Val32, CmIndex& _Index)
{
	return getMatrix().setValue(_Val32, _Index);
}
bool CmValueFTL::setValue(float _ValFloat, CmIndex& _Index)
{
	return getMatrix().setValue(_ValFloat, _Index);
}
bool CmValueFTL::setValue(int64 _Val64, CmIndex& _Index)
{
	return getMatrix().setValue(_Val64, _Index);
}
bool CmValueFTL::setValue(uint64 _Val64, CmIndex& _Index)
{
	return getMatrix().setValue(_Val64, _Index);
}
bool CmValueFTL::setValue(double _ValDouble, CmIndex& _Index)
{
	return getMatrix().setValue(_ValDouble, _Index);
}
bool CmValueFTL::setString(const CmString& _String, CmIndex& _Index)
{
	return getMatrix().setString(_String, _Index);
}

CmString CmValueFTL::asString(CmIndex& _Index)
{
	return NULL == Matrix ? CmString() : CmString(getMatrix()[_Index]);
}
uint64 CmValueFTL::asUInt64(CmIndex& _Index)
{
	return NULL == Matrix ? 0 : uint64(getMatrix()[_Index]);
}
int64 CmValueFTL::asInt64(CmIndex& _Index)
{
	return NULL == Matrix ? 0 : int64(getMatrix()[_Index]);
}
double CmValueFTL::asDouble(CmIndex& _Index)
{
	return NULL == Matrix ? 0 : double(getMatrix()[_Index]);
}
uint32 CmValueFTL::asUInt32(CmIndex& _Index)
{
	return NULL == Matrix ? 0 : uint32(getMatrix()[_Index]);
}
int32 CmValueFTL::asInt32(CmIndex& _Index)
{
	return NULL == Matrix ? 0 : int32(getMatrix()[_Index]);
}
float CmValueFTL::asFloat(CmIndex& _Index)
{
	return NULL == Matrix ? 0 : float(getMatrix()[_Index]);
}
uint16 CmValueFTL::asUInt16(CmIndex& _Index)
{
	return NULL == Matrix ? 0 : uint16(getMatrix()[_Index]);
}
int16 CmValueFTL::asInt16(CmIndex& _Index)
{
	return NULL == Matrix ? 0 : int16(getMatrix()[_Index]);
}
uint8 CmValueFTL::asUInt8(CmIndex& _Index)
{
	return NULL == Matrix ? 0 : uint8(getMatrix()[_Index]);
}
int8 CmValueFTL::asInt8(CmIndex& _Index)
{
	return NULL == Matrix ? 0 : int8(getMatrix()[_Index]);
}
bool CmValueFTL::asBool(CmIndex& _Index)
{
	return NULL == Matrix ? 0 : bool(getMatrix()[_Index]);
}
#endif

CmMatrix& CmValueFTL::getMatrix()
{
	// Instantiate a CmMatrixFTL class if it does not exist yet
	if (NULL == Matrix){
		Matrix = CmString::allocateMemory<CmMatrix>(1, isCmMatrixFTL);
	}

	// Throw exception in case of memory problems
	if (NULL == Matrix)
		throw(0);

	return *Matrix;
}

//--------CmMatrixFTL-application---------------------------------------------

/** set/clearItemUpdated.
*  Item updates are indicated by a flag in the scalar position of a matrix.
*
*  [`]=item updated
*/
bool CmValueFTL::setItemUpdated()
{
	CmMatrix& M = getMatrix();
	return M() = true;
}
bool CmValueFTL::clearItemUpdated()
{
	// get ItemUpdated flag
	CmMatrix& M = getMatrix();
	bool isItemUpdated = M();

	// clear ItemUpdated flag
	M() = false;

	return isItemUpdated;
}

/** set/getCanvasDrawingEnabled/ItemEnabled.
*  Drawing on screen can be activated/deactivated by a flag in the scalar
*  position of a matrix respectively in the scalar position of an item.
*
*  [`]=drawing enabled, [0,i]=item i disabled
*/
bool CmValueFTL::setDrawingEnabled(bool _isDrawingEnabled)
{
	CmMatrix& M = getMatrix();
	return M() = _isDrawingEnabled;
}
bool CmValueFTL::getDrawingEnabled()
{
	CmMatrix& M = getMatrix();
	return bool(M());
}
bool CmValueFTL::setItemEnabled(int32 _Index, bool _isItemEnabled)
{
	CmMatrix& M = getMatrix();
	return M(0, _Index) = _isItemEnabled ? false : true; // disable item drawing
}
bool CmValueFTL::getItemEnabled(int32 _Index)
{
	CmMatrix& M = getMatrix();
	return bool(M(0, _Index)) ? false : true;
}
int32 CmValueFTL::getItemCount()
{
	CmMatrix& M = getMatrix();
	CmIndex I;
	return M(0).getSizeLastLevel();
}

/** setChartBackground/Axis.
*  The background parameters as well as the axis parameters are set/get.
*
* [0]=background, [1]=alpha, [2]=Xmin, [3]=Xmax, [4]=Ymin, [5]=Ymax
*/
bool CmValueFTL::setChartBackground(const CmString& _Color, double _Alpha)
{
	CmMatrix& M = getMatrix();
	M(0) = _Color;
	M(1) = _Alpha;
	return true;
}
bool CmValueFTL::getChartBackground(CmString& _Color, double& _Alpha)
{
	CmMatrix& M = getMatrix();
	_Color = CmString(M(0));
	_Alpha = double(M(1));
	return true;
}
bool CmValueFTL::setRangeX(double _Xmin, double _Xmax)
{
	CmMatrix& M = getMatrix();
	M(2) = _Xmin;
	M(3) = _Xmax;
	return true;
}
bool CmValueFTL::getRangeX(double& _Xmin, double& _Xmax)
{
	CmMatrix& M = getMatrix();
	_Xmin = double(M(2));
	_Xmax = double(M(3));
	return true;
}
bool CmValueFTL::setRangeY(double _Ymin, double _Ymax)
{
	CmMatrix& M = getMatrix();
	M(4) = _Ymin;
	M(5) = _Ymax;
	return true;
}
bool CmValueFTL::getRangeY(double& _Ymin, double& _Ymax)
{
	CmMatrix& M = getMatrix();
	_Ymin = double(M(4));
	_Ymax = double(M(5));
	return true;
}


/** chart text
*  A collection of chart texts will be written into the matrix by an application module  
*  and they will be read out for displaying them on screen by a visualization module.
*
* [`]=enabled, [i]=default, [0,l]=disabled, [0,l,0/1]=position x/y, [1,l]=text, [2,l]=size, [3,l]=rotation, [4,l]=color, [5,l]=font, [6,l]=alpha
*/
bool CmValueFTL::setChartText(int32 _Index, const CmPoint2D& _Position, const CmString& _Text, double _FontSize, double _Rotation, const CmString& _Color, double _Alpha, const CmString& _Font)
{
	CmMatrix& M = getMatrix();
	// provide for having default values always available 
	M().getSizeLastLevel() == 0 ? setChartTextDefaults() : 0;
	
	M(0, _Index) = false; // enable text
	M(0, _Index, 0) = _Position.x();
	M(0, _Index, 1) = _Position.y();
	M(1, _Index) = _Text;
	M(2, _Index) = _FontSize;
	M(3, _Index) = _Rotation;
	M(4, _Index) = _Color;
	M(5, _Index) = _Alpha;
	M(6, _Index) = _Font;

	return true;
}
bool CmValueFTL::setChartTextWrap(int32& _Index, const CmPoint2D& _Position, const int32 _PosWrap, const CmString& _Text, double _FontSize, double _Rotation, const CmString& _Color, double _Alpha, const CmString& _Font)
{
	CmMatrix& M = getMatrix();
	// provide for having default values always available 
	M().getSizeLastLevel() == 0 ? setChartTextDefaults() : 0;

	CmString Text = _Text;
	CmString Line;
	int32 TextLength = int32(_Text.getLength());
	double RatioFontSize2Width = 1.28;
	int32 LineLength = _FontSize > 0 ? int32((_PosWrap - _Position.x()) * RatioFontSize2Width / _FontSize) : TextLength;
	int32 PosY = int32(_Position.y());
	double LineDistance = 1.3;
	// write all lines
	for (int32 i = 0; i < TextLength; i += LineLength){
		// generate a line of text
		Line.assignSubString(LineLength, &Text);
		Text.assignSubString(int32(Text.getLength()), 0, LineLength);

		M(0, _Index) = false; // enable text
		M(0, _Index, 0) = _Position.x();
		M(0, _Index, 1) = PosY;
		M(1, _Index) = Line;
		M(2, _Index) = _FontSize;
		M(3, _Index) = _Rotation;
		M(4, _Index) = _Color;
		M(5, _Index) = _Alpha;
		M(6, _Index) = _Font;
		_Index++;
		PosY -= int32(LineDistance * _FontSize);
	}

	return true;
}
bool CmValueFTL::setChartTextDefaults(double _FontSize, double _Rotation, const CmString& _Color, double _Alpha, const CmString& _Font)
{
	CmMatrix& M = getMatrix();
	M() = true; // enable drawing
	M(0) = false; // set default disable text drawing off
	M(2) = _FontSize;
	M(3) = _Rotation;
	M(4) = _Color;
	M(5) = _Alpha;
	M(6) = _Font;

	return true;
}
bool CmValueFTL::getChartText(int32 _Index, CmPoint2D& _Position, CmString& _Text, double& _FontSize, double& _Rotation, CmString& _Color, double& _Alpha, CmString& _Font)
{
	CmMatrix& M = getMatrix();
	_Position.x() = float(M(0, _Index, 0));
	_Position.y() = float(M(0, _Index, 1));
	_Text = CmString(M(1, _Index));
	_FontSize = double(M(2, _Index));
	_Rotation = double(M(3, _Index));
	_Color = CmString(M(4, _Index));
	_Alpha = double(M(5, _Index));
	_Font = CmString(M(6, _Index));
	// check for default values
	_FontSize == -1 ? _FontSize = double(M(2)) : 0;
	_Rotation == -1 ? _Rotation = double(M(3)) : 0;
	_Color == "" ? _Color = CmString(M(4)) : 0;
	_Alpha == -1 ? _Alpha = double(M(5)) : 0;
	_Font == "" ? _Font = CmString(M(6)) : 0;

	return true;
}

/** chart lines
*  A collection of chart lines will be written into the matrix by an application module and
*  they will be read out for putting them on screen by a visualization module.
*
*	 @Position: a (-1,-1)-terminated array of points that represent a line respectively a polyline
*
*  [`]=enabled, [i]=default, [0,p]=disabled, [0,p,0,0../1,0..]=points x/y, [1,p]=width, [2,p]=line color, [3,p]=fill color, [4,p]=line alpha, [5,p]=fill alpha
*/

bool CmValueFTL::setChartLine(int32 _Index, CmVector<float>& PointX, CmVector<float>& PointY, double _LineWidth, const CmString& _LineColor, const CmString& _FillColor, double _LineAlpha, double _FillAlpha)
{
	CmMatrix& M = getMatrix();
	// provide for always having default values available 
	M().getSizeLastLevel() == 0 ? setChartLineDefaults() : 0;

	// write points to array
	int32 PointCount = PointX.getLength();
	for (int i = 0;i < PointCount; i++){
		M(0, _Index, 0, i) = PointX[i];
		M(0, _Index, 1, i) = PointY[i];
	}
	M(1, _Index) = _LineWidth;
	M(2, _Index) = _LineColor;
	M(3, _Index) = _FillColor;
	M(4, _Index) = _LineAlpha;
	M(5, _Index) = _FillAlpha;
	M(0, _Index) = false; // enable line

	return true;
}
bool CmValueFTL::setChartRectangle(int32 _Index, const CmRectangle2D& _Rectangle, double _LineWidth, const CmString& _LineColor, const CmString& _FillColor, double _LineAlpha, double _FillAlpha)
{
	// generate 4 points of a rectangle
	CmVector<float> PointX;
	CmVector<float> PointY;
	PointX[0] = _Rectangle.P1().x(); PointY[0] = _Rectangle.P1().y();
	PointX[1] = _Rectangle.P2().x(); PointY[1] = _Rectangle.P1().y();
	PointX[2] = _Rectangle.P2().x(); PointY[2] = _Rectangle.P2().y();
	PointX[3] = _Rectangle.P1().x(); PointY[3] = _Rectangle.P2().y();
	return setChartLine(_Index, PointX, PointY, _LineWidth, _LineColor, _FillColor, _LineAlpha, _FillAlpha);
}
bool CmValueFTL::setChartLineDefaults(double _LineWidth, const CmString& _LineColor, const CmString& _FillColor, double _LineAlpha, double _FillAlpha)
{
	CmMatrix& M = getMatrix();
	M() = true; // enable drawing
	M(0) = false; // set default disable line drawing: off
	M(1) = _LineWidth;
	M(2) = _LineColor;
	M(3) = _FillColor;
	M(4) = _LineAlpha;
	M(5) = _FillAlpha;

	return true;
}
bool CmValueFTL::getChartLine(int32 _Index, CmVector<float>& PointX, CmVector<float>& PointY, double& _LineWidth, CmString& _LineColor, CmString& _FillColor, double& _LineAlpha, double& _FillAlpha)
{
	CmMatrix& M = getMatrix();
	// generate point array
	int32 PointCount = M(0, _Index, 0).getSizeLastLevel();
	PointX.setLength(PointCount);
	PointY.setLength(PointCount);

	// retrieve points from matrix and forward them to caller
	for (int i = 0; i < PointCount; i++){
		PointX[i] = float(M(0, _Index, 0, i));
		PointY[i] = float(M(0, _Index, 1, i));
	}
	_LineWidth = float(M(1, _Index));
	_LineColor = CmString(M(2, _Index));
	_FillColor = CmString(M(3, _Index));
	_LineAlpha = float(M(4, _Index));
	_FillAlpha = float(M(5, _Index));
	// check for default values
	_LineWidth == -1 ? _LineWidth = float(M(1)) : 0;
	_LineColor == "" ? _LineColor = CmString(M(2)) : 0;
	_FillColor == "" ? _FillColor = CmString(M(3)) : 0;
	_LineAlpha == -1 ? _LineAlpha = float(M(4)) : 0;
	_FillAlpha == -1 ? _FillAlpha = float(M(5)) : 0;

	return true;
}

//------------------------------------------------------------------------------
// CmValueINI class
//------------------------------------------------------------------------------
//

static CmMatrix* MatrixINI_ = NULL;
static CmString* StringINI_ = NULL;

bool CmValueINI::testCmValueINI()
{
	bool Result = true;
	printf("\n");

	// test CmValueINI
	struct _ConfigSettings {
		// identification
		CmValueINI UURI;
		CmValueINI Timestamp;
		CmValueINI LogLevel;
		CmValueINI Message;
		CmValueINI Context;
		// items
		CmValueINI Item00;
		CmValueINI Item0000;
		CmValueINI Item01;
		CmValueINI Item02;
		CmValueINI Item03;
		CmValueINI Item031;
		CmValueINI Item04;
		CmValueINI Item0400;
		CmValueINI Item04001;
		CmValueINI Item05;

	};
	typedef struct _ConfigSettings ConfigSettings;

#define CfgUURI					"EKD@JN58nc_Tuerkenfeld.CmValueINI"
#define CfgTimestamp		"/timestamp"
#define CfgLogLevel			">loglevel"
#define CfgMessage			"message"
#define CfgContext			"context"
#define CfgItem00				"/Item00:Value000"
#define CfgItem0000			">>Item0000:Value00000"
#define CfgItem01				"<<Item01:Value010"
#define CfgItem02				"Item02"
#define CfgItem03				":Value030"
#define CfgItem031			">Item031"
#define CfgItem04				"<Item04:Value040"
#define CfgItem0400			">>Item0400:Value04000"
#define CfgItem04001		">Item04001"
#define CfgItem05				"/Item05:Value050"
	ConfigSettings Cfg =
	{
		// identification
		CfgUURI,
		CfgTimestamp,
		CfgLogLevel,
		CfgMessage,
		CfgContext,
		CfgItem00,
		CfgItem0000,
		CfgItem01,
		CfgItem02,
		CfgItem03,
		CfgItem031,
		CfgItem04,
		CfgItem0400,
		CfgItem04001,
		CfgItem05,

	};






	// verify values and structure
	// name (value) below root level
	if ((CmValueINI::getValue(CfgItem00) != Cfg.Item00) || (Cfg.Item00.getAddress() != "0-1-0")) Result = false;
	// name:value pair two levels lower as before
	if (CmValueINI::getValue(CfgItem0000) != (Cfg.Item0000) || (Cfg.Item0000.getAddress() != "0-1-0-0-0")) Result = false;
	// name:value pair on same level as before
	if (CmValueINI::getValue(CfgItem01) != (Cfg.Item01) || (Cfg.Item01.getAddress() != "0-2-0")) Result = false;
	// only a name on same level as before
	if (CmValueINI::getName(CfgItem02) != (Cfg.Item02) || (Cfg.Item02.getAddress() != "0-3")) Result = false;
	// only a value on same level as before
	if (CmValueINI::getValue(CfgItem03) != (Cfg.Item03) || (Cfg.Item03.getAddress() != "0-4-0")) Result = false;
	// only a name one level lower as before
	if ((CmValueINI::getName(CfgItem031) != Cfg.Item031) || (Cfg.Item031.getAddress() != "0-4-1")) Result = false;
	// name:value pair one level higher as before
	if (CmValueINI::getValue(CfgItem04) != (Cfg.Item04) || (Cfg.Item04.getAddress() != "0-5-0")) Result = false;
	// name:value pair two levels lower as before
	if ((CmValueINI::getValue(CfgItem0400) != Cfg.Item0400) || (Cfg.Item0400.getAddress() != "0-5-0-0-0")) Result = false;
	// item three levels lower as before with a path gap
	if ((CmValueINI::getName(CfgItem04001) != Cfg.Item04001) || (Cfg.Item04001.getAddress() != "0-5-0-0-1")) Result = false;
	// name (value) below root level
	if ((CmValueINI::getValue(CfgItem05) != Cfg.Item05) || (Cfg.Item05.getAddress() != "0-6-0")) Result = false;







	// synchronize and write resulting StringFTL to disk

	NULL != CmValueINI::StringINI ? CmValueINI::StringINI->writeBinary("../Data/CmValueINI_v1.0.ini") : 0;

	// generate FTLight structure
	CmStringFTL StructureFTL;
	CmString StringFTL;
	CmUURI UURI(CfgUURI);
	StructureFTL.processStringFTL(Cfg.UURI.StringINI->getText());

	StructureFTL.serializeStringFTL(StringFTL);
	StringFTL.writeBinary("../Data/CmValueINI_v1.0.ftl");


	if (false == Cfg.UURI.writeInfoFTL("../Data/", StructureFTL, Cfg.LogLevel)){
		printf("\nwriteInfoFTL failed:\n");
		printf(Cfg.Message.getText());
		printf(Cfg.Context.getText());
		printf("\n");
	}

	return Result;
}
CmValueINI::CmValueINI(const int8 *_Init)
	:CmValueFTL(CmUURI::isUURI(_Init) ? "0" : "0-0")
{
	// split into a name/value pair
	int PosName = 0;
	CmString Name = CmValueINI::getName(_Init, &PosName);
	CmString Value = CmValueINI::getValue(_Init);
	CmString Separator(",");

	// is root element?
	if ((_Init == NULL) || (strlen(_Init) == 0) || CmUURI::isUURI(_Init)){
		// start new FTLight hierarchy generation
		MatrixINI = CmString::allocateMemory<CmMatrix>(1, isCmMatrixFTL);
		MatrixINI_ = MatrixINI;
		StringINI = CmString::allocateMemory<CmString>(1, isCmString);
		StringINI_ = StringINI;
		// verify that an INI string exists
		if (NULL == StringINI_) return;
		// UURI will be first element
		Address = "0";
		CmValueFTL::operator= (Name);
		if (NULL != _Init){
			// root element (UURI)
			*StringINI_ = _Init; 
			*StringINI_ += "\n";
		}
	}
	else{
		StringINI = NULL;

		int Level = (*MatrixINI_).getLength();
		CmString AddressName;
		CmString AddressValue;

		// verify that an INI items exists
		if (NULL == MatrixINI_) return;
		if (NULL == StringINI_) return;
		if (NULL == _Init) return;

		// navigation
		int32 ExtendedLevels;
		switch (*_Init){
		case '/':
			// extend subordinate level below root
			Level > 0 ? (*MatrixINI_)(0) = int((*MatrixINI_)(0)) + 1 : 0;
			(*MatrixINI_).setLength(Level = 1);
			break;
		case '>':
			// shift level downwards
			(*MatrixINI_).setLength(Level += PosName);
			// ToDo: fix extended level for higher levels (5??)
			ExtendedLevels = (*MatrixINI_)(0, 0);
			PosName <= ExtendedLevels ? (*MatrixINI_)(Level - 1) = int((*MatrixINI_)(Level - 1)) + 1 : Separator = ":";
			break;
		case '<':
			Level > PosName ? (*MatrixINI_).setLength(Level -= PosName) : 0;
			(*MatrixINI_)(Level - 1) = int((*MatrixINI_)(Level - 1)) + 1;
			(*MatrixINI_).setLength(Level);
			break;
		default:
			// continue on current level
			Level > 0 ? (*MatrixINI_)(Level - 1) = int((*MatrixINI_)(Level - 1)) + 1 : (*MatrixINI_)(0) = 0;
			break;
		}
		// address/StringFTL generation
		AddressName = "0";
		for (int i = 0; i < Level; i++){
			AddressName += "-";
			AddressName += int((*MatrixINI_)(i));
			*StringINI_ += i<=PosName ? "," : Separator;
		}
		AddressValue = AddressName;
		AddressValue += "-0";

		// insert name and value to StringINI_
		*StringINI_ += Name.getLength() > 0 ? Name : " ";
		Value.getLength() > 0 ? *StringINI_ += ":" : 0;
		Value.getLength() > 0 ? *StringINI_ += Value : 0;
		*StringINI_ += "\n";

		// assign address of name or value to config item
		if (Value.getLength() > 0){
			Address = AddressValue;
			CmValueFTL::operator= (Value);
			(*MatrixINI_)(0, 0) = 1; // level is extended by 1 (value below name)
		}
		else{
			Address = AddressName;
			CmValueFTL::operator= (Name);
			(*MatrixINI_)(0, 0) = 0; // level is not extended (name without any item below)
		}
	}
}
CmValueINI::~CmValueINI()
{
	// cleanup resources
	StringINI != NULL ? delete StringINI : 0;
}
CmString CmValueINI::getName(CmString _NameValue, int* _PosName)
{
	// find ':' position
	int PosColon = _NameValue.findPosition(':');

	// skip control chars
	int PosName = 0;
	while ((_NameValue[PosName] == '/') || (_NameValue[PosName] == '>') || (_NameValue[PosName] == '<')){
		PosName++;
	}
	NULL != _PosName ? *_PosName = PosName : 0;

	// return name
	_NameValue.assignSubString(PosColon - 1, 0, PosName);

	return _NameValue;
}
CmString CmValueINI::getValue(CmString _NameValue)
{
	// find ':' position
	int PosColon = _NameValue.findPosition(':');

	// return value
	_NameValue.assignSubString(_NameValue.getLength()-1, 0, PosColon+1);

	return _NameValue;
}
bool CmValueINI::setDefaultInfoFTL(CmValueINI& _ValueINI, CmValueINI& _Return)
{
	// check initialization string availability
	if (NULL == _ValueINI.StringINI) return false;

	CmStringFTL InfoFTL;
	InfoFTL.processStringFTL(*_ValueINI.StringINI);
	return _ValueINI.updateInfoFTL(InfoFTL, _Return);
}

CmValueFTL& CmValueINI::getLogLevel()
{
	return *this;
}
CmValueFTL& CmValueINI::getMessage()
{
	return getNextConfigValue();
}
CmValueFTL& CmValueINI::getContext()
{
	return getNextConfigValue().getNextConfigValue();
}


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
	}	else{
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
	if(false == TryEnterCriticalSection(&CriticalSection)) return false;

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
	CmStringFTL *LogLevel =NULL;
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
	CmString Timestamp = CmDateTime::getTimeUTC(0,-1,true); // true= timestamp for filename
	Timestamp += "_";
	Timestamp += _LogFile;
	_LogFile = _FilePath;
	_LogFile[((int32)_LogFile.getLength())-1] != '\\' ? _LogFile += "\\" : 0;
	_LogFile += Timestamp;

	// write logfile to disk
	return LogFTL.writeBinary(_LogFile.getText());
}

CmValueFTL& getReturn();