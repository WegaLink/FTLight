//////////////////////////////////////////////////////////////////////////////
//
// CmEvent.cpp: Implementation of the CmEvent class
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

#include "Services/Basic/CmEvent.h"

using namespace Cosmos;


//------------------------------------------------------------------------------
// CmEvent class
//------------------------------------------------------------------------------
template <class T>
CmEvent<T>::CmEvent()
{
  // Initialize EventHandler callback
	EventHandler = NULL;

}
template <class T>
CmEvent<T>::~CmEvent()
{
}

template <class T>
bool CmEvent<T>::triggerEvent(CmString& EventInfo)
{
	// send received event string to underlying CmStream class.
	writeLine(Event);

	return true;
}

template <class T>
bool CmEvent<T>::processMessage(CmStream& _Message)
{
	// put received FTLight data into EventX object
	CmString LineX;
	_Message.readLine(LineX);
	EventX.receiveLineX(LineX);

	// forward event to a registered EventHandler
	if (NULL != EventHandler)
		EventHandler(EventX);

	return true;
}

template <class T>
bool CmEvent<T>::registerEventHandler(T* _Object, bool(T::*_EventHandler)(CmStringX&))
{
	// Check parameter and make sure that an EventHandler has not been registered before
	if (NULL == _Object) return false;
	if (NULL == _EventHandler) return false;

	// Register this module to underlying CmStream
	if (registerStreamReceiver(this)){
		// Accept registration for EventHandler
		Object = _Object;
		EventHandler = _EventHandler;
	}

  return true;
}

template <class T>
bool CmEvent<T>::unregisterEventHandler(T* _Object, bool(T::*_EventHandler)(CmStringX&))
{
  // Check parameter and whether registered EventHandler matches argument
	if (NULL == _Object) return false;
	if (NULL == _EventHandler) return false;
	if (_Object != Object) return false;
	if (_EventHandler != EventHandler) return false;

  // Clear registration
	Object = NULL;
	EventHandler = NULL;

  return true;
}




