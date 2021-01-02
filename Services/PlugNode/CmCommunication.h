//////////////////////////////////////////////////////////////////////////////
//
// CmCommunication.h: Declaration of CmCommunication class
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

#ifndef CmCommunicationH
#define CmCommunicationH

#include "Services/PlugNode/CmStream.h"

namespace Cosmos
{
// forward declaration
	class CmServiceConnection;

/** CmCommunication.
 *  A CmCommunication class allows for talking to a peer by exchanging structured
 *  data that represents hierarchical information items.
 *  
 *  NOTE: The CmCommunication class has been designed for transferring 'FTLight' coded
 *        data. The following link points to a 'FTLight' documentation:
 *          http://wegalink.eu/development/FTLight/FTLight_DE.pdf
 *
 */
class CmCommunication : public CmStream
{

public:
	CmCommunication();
	virtual ~CmCommunication();

public:
	/** processInformation.
	*  An information item will be processed on arrival. This funtion will be overridden by
	*  derived classes which are supposed to deal with received information.
	*
	* NOTE: Information must not be 'const' in order to be processed by readLine().
	*/
	virtual bool processInformation(CmString& _Information);

public:
	/** processMessage.
	*  A message will be forwarded to asuperior modul for processing. Usually, this function
	*  will be overridden by derived clases which are supposed to further process the message.
	*
	* NOTE: Message must not be 'const' in order to be processed by readLine().
	*/
	virtual bool processMessage(CmStream& _Message, CmServiceConnection *_ServiceConnection);

public:
	/** processMessage.
	*  A message will be processed on arrival. Usually, this funtion will be called by
	*  the CmStream base class.
	*
	* NOTE: Message must not be 'const' in order to be processed by readLine().
	*/
	bool processMessage(CmStream& _Message);

public:
	/** sendInformation.
	 *  An information item will be sent to peer
	 *
	 * NOTE: Information must not be 'const' in order to be processed by readLine().
	 */
	bool sendInformation(CmString& _Information);

public:
	/** getLastInformation.
	*   A recently received information item will be returned
	*/
	bool getLastInfo(CmString& _LastInfo);

public:

  /**  registerCommunicationReceiver.
   *   A communication receiver will be registered which makes a callback to 
	 *   user defined functions whenever a next complete information item or
	 *   network maintenance item has arrived from a peer CmCommunication module.
   */
	bool registerCommunicationReceiver(CmCommunication* _CommunicationReceiver, CmServiceConnection *_ServiceConnection);

  /**  unregisterCommunicationReceiver.
   *   A registered communication receiver will be unregistered. The value for
   *   CommunicationReceiver has to match the value when registering the module. 
   */
	bool unregisterCommunicationReceiver(CmCommunication* _CommunicationReceiver, CmServiceConnection *_ServiceConnection);

protected:
	CmString LastInformation;

private:
	CmCommunication *CommunicationReceiver;
	CmServiceConnection *ServiceConnection;

};


} // namespace Cosmos

#endif // !defined(CmCommunicationH)
