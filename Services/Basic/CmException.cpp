//////////////////////////////////////////////////////////////////////////////
//
// CmException.cpp: Implementation of a CmException class
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

#include "Services/Basic/CmException.h"

//--------------------------------------------------------------------
// CmException Class
//--------------------------------------------------------------------
CmException::CmException()
{
	pData  = NULL;
	putMessage("General exception encountered");
	putContext("");
}

CmException::CmException(void* _pData)
{
	pData = _pData;
	putMessage("Exception with data encountered");
	putContext("");
}

CmException::CmException(CmString& _Message)
{
	pData = NULL;
	putMessage(_Message.getText());
	putContext("");
}

CmException::CmException(const int8* _pMessage)
{
	pData = NULL;
	putMessage(_pMessage);
	putContext("");
}

CmException::CmException(CmString& _Message,int32 _nParameter)
{
	CmString Parameter;

	pData = NULL;
	putMessage(_Message.getText());
	Parameter = _nParameter;
	putContext(Parameter.getText());
}

CmException::CmException(const int8* _pMessage,int32 _nParameter)
{
	CmString Parameter;

	pData = NULL;
	putMessage(_pMessage);
	Parameter = _nParameter;
	putContext(Parameter.getText());
}

CmException::CmException(CmString& _Message,uint32 _uParameter)
{
	CmString Parameter;

	pData = NULL;
	putMessage(_Message.getText());
	Parameter = _uParameter;
	putContext(Parameter.getText());
}

CmException::CmException(const int8* _pMessage,uint32 _uParameter)
{
	CmString Parameter;

	pData = NULL;
	putMessage(_pMessage);
	Parameter = _uParameter;
	putContext(Parameter.getText());
}

CmException::CmException(CmString& _Message,uint64 _u64Parameter)
{
	CmString Parameter;

	pData = NULL;
	putMessage(_Message.getText());
	Parameter = _u64Parameter;
	putContext(Parameter.getText());
}

CmException::CmException(const int8* _pMessage,uint64 _u64Parameter)
{
	CmString Parameter;

	pData = NULL;
	putMessage(_pMessage);
	Parameter = _u64Parameter;
	putContext(Parameter.getText());
}

CmException::CmException(CmString& _Message,CmString& _Context)
{
	pData = NULL;
	putMessage(_Message.getText());
	putContext(_Context.getText());
}

CmException::CmException(const int8* _pMessage,CmString& _Context)
{
	pData = NULL;
	putMessage(_pMessage);
	putContext(_Context.getText());
}

CmException::CmException(const int8* _pMessage,const int8* _pContext)
{
	pData = NULL;
	putMessage(_pMessage);
	putContext(_pContext);
}

CmException::CmException(void* _pData,CmString& _Message)
{
	pData = _pData;
	putMessage(_Message.getText());
	putContext("");
}
 
CmException::CmException(void* _pData,const int8* _pMessage)
{
	pData = _pData;
	putMessage(_pMessage);
	putContext("");
}
 
CmException::CmException(void* _pData,CmString& _Message,CmString& _Context)
{
	pData = _pData;
	putMessage(_Message.getText());
	putContext(_Context.getText());
}
 
CmException::CmException(void* _pData,const int8* _pMessage,CmString& _Context)
{
	pData = _pData;
	putMessage(_pMessage);
	putContext(_Context.getText());
}
 
CmException::CmException(void* _pData,const int8* _pMessage,const int8* _pContext)
{
	pData = _pData;
	putMessage(_pMessage);
	putContext(_pContext);
}
 
CmException::CmException( const CmException &_E)
{
	pData = _E.pData;
	putMessage(_E.pMessage);
	putContext(_E.pContext);
}

CmException::~CmException()
{
	if (pMessage != NULL){
		CmString::releaseMemory<int8>(pMessage, LengthMessage + 1, isInt8);
	}
	if (pContext != NULL){
		CmString::releaseMemory<int8>(pContext, LengthContext + 1, isInt8);
	}
}

void CmException::putMessage(const int8* _pMessage)
{
	_pMessage == NULL ? _pMessage = "<Message>" : 0;

	LengthMessage = int32(strlen(_pMessage));
	pMessage = CmString::allocateMemory<int8>(int32(LengthMessage + 1), isInt8);
	memset(pMessage, 0, LengthMessage + 1);
	STRCPY(pMessage, LengthMessage + 1, _pMessage);
}

void CmException::putContext(const int8* _pContext)
{
	_pContext == NULL ? _pContext = "<Context>" : 0;

	LengthContext = int32(strlen(_pContext));
	pContext = CmString::allocateMemory<int8>(int32(LengthContext + 1), isInt8);
	memset(pContext, 0, LengthContext + 1);
	STRCPY(pContext, LengthContext + 1, _pContext);
}

void* CmException::getData()
{
	return pData;
}

int8* CmException::getMessage()
{
	return pMessage;
}

int8* CmException::getContext()
{
	return pContext;
}


