//////////////////////////////////////////////////////////////////////////////
//
// CmPlugNode.h: Declaration of CmPlugNode classes
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

#ifndef CmPlugNodeH
#define CmPlugNodeH

#include "Services/PlugNode/CmServiceAccess.h"

#define PLUGNODE_VERSION					"v1.0"

namespace Cosmos
{

/** CmPlugNode.
 *  The CmPlugNode class provides for obtaining a service (functionality) from
 *  other program modules as well as for exposing the current class' service
 *  (functionality) to other modules. According to the OSI model, a CmPlugNode
 *  implements a Service Access Point (SAP).
 *
 *  A class derived from CmPlugNode will on the one hand represent an interface
 *  (SERVICE interface) which may forward actual functionality (a PROVIDER's
 *  implementation of the SERVICE) from another CmPlugNode. On the other hand,
 *  the derived class may decide to instantiate an implementation of respective
 *  functionality itself (to become a PROVIDER on its own). That functionality
 *  (PROVIDER implementation) will subsequently be offered to other CmPlugNodes
 *  which as well have registered with the same service network.
 *
 *  Described dynamic linking functionality is almost transparent for a derived
 *  class. That means, the CmPlugNode base class implements most of the functions
 *  which are needed for the dynamic forwarding of a PROVIDER's SERVICE, locally
 *  as well as remotely. It just needs to know the UURI of respective SERVICE.
 *
 *  Usually, the derived class will obtain a SERVICE pointer from CmPlugNode for
 *  accessing a potentially remote PROVIDER's implementation of that SERVICE. If
 *  that SERVICE pointer is not available yet then the derived class may decide
 *  to instantiate missing PROVIDER implementation and to store a SERVICE pointer
 *  in CmPlugNode that will point to the PROVIDER. That SERVICE pointer will then
 *  also be distributed to other parties on demand.
 *
 *  A CmPlugNode will normally always have local access to the functionality
 *  (SERVICE) of other program modules that live in the same process space.
 *  Additionally, a CmPlugNode may be enabled for remote SERVICE access.
 *
 *  Appropriate UURIs have to be declared for those local and remote SERVICE
 *  access networks. The local SERVICE access for 'Cosmos' software has been
 *  defined by the following UURI:
 *                     'EKD@JN58nc_Türkenfeld.Cosmos'
 */
class CmPlugNode : public CmServiceAccess
{
#define UURI_SERVICE_CmPlugNode     "CmPlugNode/"PLUGNODE_VERSION
#define WEGALINK_URL                "wegalink.feste-ip.net:50843"

#pragma warning( disable: 4520)

public:
  /** Constructor for a CmPlugNode instance representing a UURI's functionality.
   *  If the RootUURI is missing (NULL) then 'anonymous@Earth.UURI' will be
   *  used. If the UURI string is missing as well (NULL), then CmPlugNode's UURI
   *  will be generated for the Cosmos software: 'EKD@JN58nc_Türkenfeld.Cosmos'.
   */
  CmPlugNode(CmUURI *UURI =NULL,CmUURI *RootUURI =NULL);
  CmPlugNode(const char *UURI =NULL,const char *RootUURI =NULL);
  CmPlugNode(const CmPlugNode& PlugNode);

	// Destructor
	virtual ~CmPlugNode();

	/** getUURI.
	*  The ServiceUURI will be returned which usually represents a derived
	*  class' service UURI.
	*/
	virtual CmUURI& getUURI();

public:
	/** testPlugNode.
	*   A unit test will run against the CmPlugNode class
	*/
	bool testPlugNode();

protected:
	/** initPlugNode.
   *  Initialize PlugNode and connect it to a local network
   */
  void initPlugNode(CmUURI *UURI,CmUURI *RootUURI);
  void initPlugNode(const CmPlugNode& PlugNode);

protected:
	/** checkinToLocalNetwork.
   *  This PlugNode wants to be checked in to a local network according to specified
   *  UURI of the network.
   *
   *  @param LocalNetworkUURI the intended local network's UURI
	 *	@param PlugNode a Local to the PlugNode to be connected
   *  @return whether connecting to the local network succeeded or not
   */
	bool checkinToLocalNetwork(CmUURI& LocalNetworkUURI, CmPlugNode *PlugNode);

private:
	/** reconnectNetwork.
	*  A PlugNode applies for re-connecting to the network after it has got disconnected
	*  in the result of maintaining the network structure.
	*/
	bool reconnectNetwork();

protected:
	// register/unregister a callback,
	// the functions are used by derived classes that implement callbacks
	template <class T>
	bool registerCallback(T& _Callback, T _NewCallback)
	{
		// Check if there has been a registration already before
		if (NULL != _Callback)
		{
			// Check if registered callback function matches new one
			if (_Callback == _NewCallback) return true;

			// Callback can not be registered
			return false;
		}

		// Store callback function pointer
		_Callback = _NewCallback;

		return true;
	}

	template <class T>
	bool unregisterCallback(T& _Callback, T _OldCallback)
	{
		// Check for valid registration
		if (NULL == _Callback) return true;

		// Check if currently registered callback function matches 
		if (_Callback != _OldCallback) return false;

		// Unregister callback function
		_Callback = NULL;

		return true;
	}

public:
	// FTL hierarchy
	CmStringFTL ControlFTL;

protected:
  // Workspace of a derived class for which this CmPlugNode is a container for
	CmUURI ServiceUURI;    // class identifier (identification of a SERVICE)
	void *ServiceProvider; // class implementation (pointer to a SERVICE PROVIDER)

};


/** SERVICE_CmPlugNode_Demo.
 *  The following class demonstrates how a class derived from CmPlugNode will
 *  obtain its actual PROVIDER functionality using a dynamic linking process.
 */

// forward declaration
class PROVIDER_CmPlugNode_Demo;

class SERVICE_CmPlugNode_Demo : public CmPlugNode
{
// UURI extensions for a 'CmPlugNode_Demo' SERVICE and PROVIDER
#define UURI_SERVICE_CmPlugNode_Demo      "EKD@JN58nc_Türkenfeld.CmPlugNode_Demo/"PLUGNODE_VERSION
#define UURI_PROVIDER_CmPlugNode_Demo     UURI_SERVICE_CmPlugNode_Demo"/built/2012-09-06"

public:
	SERVICE_CmPlugNode_Demo(const char *UURI = NULL, const char *RootUURI = NULL);
	~SERVICE_CmPlugNode_Demo();

public:
	// Any information from other PlugNodes will arrive in this function
	bool processInformation(CmString& Information);

public:
	/** getLastInformation.
	*   A recently received information item will be returned
	*/
	const CmString& getLastInfo();

private:
  /** Provider.
   *  A SERVICE PROVIDER will be returned which may be either a Local to a
   *  local PROVIDER implementation or to a remote PROVIDER implementation. If
   *  there has not been a PROVIDER yet, then a local PROVIDER will be
   *  instantiated. It is supposed that this will usually succeed. Otherwise an
   *  exception will be thrown since it is not possible to continue program flow
   *  when instantiation of a class fails due to e.g. insufficient resources.
   */
	PROVIDER_CmPlugNode_Demo& Provider();

  // A local instance of a PROVIDER 
	PROVIDER_CmPlugNode_Demo *LocalProvider;

public:
  /** getUURI.
   *  The UURI including version information will be retrieved from class
   *  implementation.
   */
  virtual CmUURI& getUURI();

};

/** PROVIDER_CmPlugNode_Demo.
 *  The following class implements a 'CmPlugNode_Demo' PROVIDER
 */
class PROVIDER_CmPlugNode_Demo : public CmPlugNode
{
public:
	PROVIDER_CmPlugNode_Demo(const char *UURI = NULL, const char *RootUURI = NULL);
  ~PROVIDER_CmPlugNode_Demo();

public:
  CmUURI& getUURI();

public:
	bool processInformation(CmString& Information);
	const CmString& getLastInfo();

};

} // namespace Cosmos

#endif // !defined(CmPlugNodeH)
