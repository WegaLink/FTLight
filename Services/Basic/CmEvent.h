//////////////////////////////////////////////////////////////////////////////
//
// CmEvent.h: Declaration/Implementation of a CmEvent template class
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

#ifndef CmEventH
#define CmEventH

#include "Services/PlugNode/CmStream.h"

namespace Cosmos
{

/** CmEvent.
 *  A CmEvent class allows for initiating an event processing by sending
 *  structured FTLight data to a module.
 *  
 *  NOTE: The CmEvent class has been designed for transferring 'FTLight' coded
 *        data. The following link points to a 'FTLight' documentation:
 *          http://wegalink.eu/development/FTLight/FTLight_DE.pdf
 *
 */
template <class T>
class CmEvent : public CmStream
{
public:
	/** CmEvent.
	*   The event handler will be cleared
	*/
	CmEvent()
	{
		// Initialize EventHandler callback
		Module = NULL;
		processEvent = NULL;
	}
	/** ~CmEvent.
	*/
	~CmEvent(){};

public:
	/** processMessage.
	*  A FTLight message will be processed on arrival. This funtion will be called 
	*  by the CmStream base class. 
	*  An EventFTL structure will be composed from arriving event message. If an event
	*  handler function exists than EventFTL will be forwarded for event processing.
	*/
	bool processMessage(CmStream& _Message)
	{
		// put received FTLight data into EventFTL object
		CmString LineFTL;
		_Message.readLine(LineFTL);
		EventFTL.processStringFTL(LineFTL);

		// forward event to a registered EventHandler
		if (NULL != Module && NULL != processEvent)
			Module->processEvent(EventFTL);

		return true;
	}

public:
	/** registerEventHandler.
	*   A function will be registered which is supposed to handle events
	*/
	bool registerEventHandler(T* _Module, bool(T::*_processEvent)(CmStringFTL&))
	{
		// Check parameter and make sure that an EventHandler has not been registered before
		if (NULL == _Module) return false;
		if (NULL == _processEvent) return false;
		if (NULL != Module) return false;
		if (NULL != processEvent) return false;

		// Register this module to underlying CmStream
		if (registerStreamReceiver(this)){
			// Accept registration for EventHandler
			Module = _Module;
			processEvent = _processEvent;
		}

		return true;
	}

public:
	/** unregisterEventHandler.
	*   A previously registered CmEventHandler will be unregistered. If provided 
	*   handler does not match the registered handler then nothing will be done.
	*/
	bool unregisterEventHandler(T* _Module, bool(T::*_processEvent)(CmStringFTL&))
	{
		// Check parameter and whether registered EventHandler matches argument
		if (NULL == _Module) return false;
		if (NULL == _processEvent) return false;
		if (_Module != Module) return false;
		if (_processEvent != processEvent) return false;

		// Clear registration
		Module = NULL;
		processEvent = NULL;

		return true;
	}

private:
	T* Module;
	bool(T::*processEvent)(CmStringFTL&);
	CmStringFTLroot EventFTL;
};


} // namespace Cosmos

using namespace Cosmos;

#endif // !defined(CmEventH)
