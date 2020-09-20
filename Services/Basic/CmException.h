//////////////////////////////////////////////////////////////////////////////
//
// CmException.h - Declaration of a CmException class
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

#ifndef CmExceptionH
#define CmExceptionH

#include "Services/Basic/CmTypes.h"
#include "Services/Basic/CmString.h"

namespace Cosmos
{

// forward declaration
class CmString;

// Exception class used in all modules
class CmException
{
private:
	void* pData;     // data pointer forwarding (no destruction)
	int8* pMessage;  // will be removed in destructor
	int8* pContext;  // will be removed in destructor
	int32 LengthMessage;
	int32 LengthContext;
public:
	//Constructors and destructor
	CmException();
	CmException(void* _pData);
	CmException(CmString& mMessage);
	CmException(const int8* _pMessage);
	CmException(CmString& mMessage,uint32 _uParameter);
	CmException(const int8* _pMessage,uint32 _uParameter);
	CmException(CmString& mMessage,int32 _nParameter);
	CmException(const int8* _pMessage,int32 _nParameter);
	CmException(CmString& mMessage,uint64 _u64Parameter);
	CmException(const int8* _pMessage,uint64 _u64Parameter);
	CmException(CmString& mMessage,CmString& mContext);
	CmException(const int8* _pMessage,CmString& mContext);
	CmException(const int8* _pMessage,const int8* _pContext);
	CmException(void* _pData,CmString& mMessage);
	CmException(void* _pData,const int8* _pMessage);
	CmException(void* _pData,CmString& mMessage,CmString& mContext);
	CmException(void* _pData,const int8* _pMessage,CmString& mContext);
	CmException(void* _pData,const int8* _pMessage,const int8* _pContext);
 	CmException(const CmException &E);
	~CmException();

	void putMessage(const int8* _pMessage);
	void putContext(const int8* _pContext);

	void* getData();
	int8* getMessage();
	int8* getContext();
};

}  // namespace Cosmos

using namespace Cosmos;

#endif // ifndef CmExceptionH
