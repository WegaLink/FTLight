//////////////////////////////////////////////////////////////////////////////
//
// CmCommunication.cpp: Implementation of the CmCommunication class
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

#include "Services/PlugNode/CmCommunication.h"

using namespace Cosmos;


//------------------------------------------------------------------------------
// CmCommunication class
//------------------------------------------------------------------------------

CmCommunication::CmCommunication()
{

  // Initialize Communication callback
	CommunicationReceiver = NULL;
	ServiceConnection = NULL;

}
CmCommunication::~CmCommunication()
{
	// NOTE: CommunicationReceiver and ServiceConnection have been deleted before.
}

bool CmCommunication::processInformation(CmString& _Information)
{
	// this functionality will usually be implemented by derived classes,
	// the default behavior here is to save recent information item
	LastInformation = _Information;

	LOG4("ID=?", " CmCommunication::processInformation() '", _Information, "'", Msg, CMLOG_Process)

	return true;
}

bool CmCommunication::processMessage(CmStream& /*_Message*/, CmServiceConnection * /*_ServiceConnection*/)
{
	// usually, this function will be implemented in derived classes

	return true;
}

bool CmCommunication::processMessage(CmStream& _Message)
{
	// forward information to a registered receiver
	if (NULL != CommunicationReceiver)
		CommunicationReceiver->processMessage(_Message, ServiceConnection);

	return true;
}

bool CmCommunication::sendInformation(CmString& _Information)
{
	// encapsulate information in a FTLight structure


	// send a FTLight message to peer
	writeLine(_Information);

	return true;
}

bool CmCommunication::getLastInfo(CmString& _LastInfo)
{
	// clear returned information
	_LastInfo.setLength(0);

	// check if any information is available
	if (0 == LastInformation.getLength()) return false;

	// return last received information
	_LastInfo = LastInformation;

	return true;
}

bool CmCommunication::registerCommunicationReceiver(CmCommunication* _CommunicationReceiver, CmServiceConnection *_ServiceConnection)
{
	// Check parameters and make sure that a communication receiver has not been registered before
	if (NULL == _CommunicationReceiver) return false;
	if (NULL == _ServiceConnection) return false;

	// Register this module to underlying CmStream
	if (registerStreamReceiver(this)){
		// Accept registration for CmCommunication
		CommunicationReceiver = _CommunicationReceiver;
		ServiceConnection = _ServiceConnection;
	}

  return true;
}

bool CmCommunication::unregisterCommunicationReceiver(CmCommunication* _CommunicationReceiver, CmServiceConnection *_ServiceConnection)
{
  // Check parameters and whether registered communication receiver matches argument
	if (NULL == _CommunicationReceiver) return false;
	if (NULL == _ServiceConnection) return false;
	if (_CommunicationReceiver != CommunicationReceiver) return false;
	if (_ServiceConnection != ServiceConnection) return false;

  // Clear registration
	CommunicationReceiver = NULL;
	ServiceConnection = NULL;

  return true;
}




