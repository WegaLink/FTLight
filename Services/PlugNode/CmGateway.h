//////////////////////////////////////////////////////////////////////////////
//
// CmGateway.h: Declaration of a SERVICE_CmGateway class
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

#ifndef CmGatewayH
#define CmGatewayH

#include "Services/PlugNode/CmPlugNode.h"

namespace Cosmos
{
// NOTE: CmConnectionType and CmConnectionInfo have been defined in
//       CmServiceConnection.h in order to avoid compilation problems from
//       circle dependencies.

/** SERVICE_CmGateway.
*  This class provides for network connectivity. A connection to a server
*  can be established as well as a local server can be activated.
*
*  This class is an interface for multiple gateway providers.
*/
class SERVICE_CmGateway : public CmPlugNode
{
// UURI extensions for a 'CmGateway' SERVICE
#define UURI_SERVICE_CmGateway      "CmGateway/version/1.0"

public:
	SERVICE_CmGateway(const CmConnectionInfo& ConnectionInfo);
  virtual ~SERVICE_CmGateway();

//------Service-functions-------------------------------------------------------

public:
	/** setNetworkUURI.
	*  A network UURI will be assigned to this gateway.
	*/
	bool setNetworkUURI(const CmUURI& NetworkUURI);

public:
	/** getProviderUURI.
	*  The provider's service UURI will be returned.
	*/
  const CmUURI& getProviderUURI();

public:
	/** joinNetwork.
	*  A connection to a service network will be established.
	*
	*  @param NetworkUURI, the UURI of desired service network
	*  @param GatewayInfo, contact information for reaching a gateway in the service network
	*  @return succes or failed
	*/
	virtual bool joinNetwork(const CmUURI& NetworkUURI, CmConnectionInfo *GatewayInfo);

public:
	/** setConnectionInfo.
	*  The connection info will be set for an existing network gateway.
	*/
	bool setConnectionInfo(CmConnectionInfo& GatewayInfo);

public:
	/** sendInfo.
	*   A connection will be looked-up respectively established to a gateway of 
	*   specified service UURI. Subsequently, an information is sent to that gateway. 
	*   The information is supposed to reach the recipient PlugNode at the remote site.
	*   If the service UURI is locally available then the information will be forwarded
	*   locally.
	*/
	bool sendInfo(const CmString& Info, const CmUURI& ServiceUURI, bool isControl = false, bool isFTLight = true, CmString* RecipientUURI = NULL);

public:
	/** findGateway.
	*  A gateway for sending information to a service UURI in this network will be determined.
	*/
	virtual bool findGateway(const CmUURI& ServiceUURI, CmConnectionInfo& GatewayInfo);

public:
	/** sendInfoToGateway.
	*  An information for a service UURI will be sent to a gateway with specified 
	*  contact information.
	*/
	virtual bool sendInfoToGateway(CmString& Info, CmConnectionInfo& GatewayInfo);

public:
	/** processInformation.
	*  An information item will be analyzed and forwarded to the respective recipient.
	*/
	bool processInformation(CmString& Information);

protected:
	// Server parameters
	CmConnectionInfo ServerConnectionInfo;
	CmConnectionInfo RemoteGatewayConnectionInfo;
	int32 SetupReturn;

protected:
	// recipient UURI and FTLight flag
	CmUURI RecipientUURI;
	bool isFTLight;

public:
	// Gateway control
	bool isShutdownGateway;

//------Service-access-through-a-PROVIDER---------------------------------------

private:
	/** Provider.
	*  A SERVICE PROVIDER will be returned which may be either a referenc to a
	*  local PROVIDER implementation or to a network PROVIDER implementation. If
	*  there has not been a PROVIDER yet, then a local PROVIDER will be
	*  instantiated. It is supposed that this will usually succeed. Otherwise an
	*  exception will be thrown since it is not possible to continue program flow
	*  when instantiation of a class fails due to e.g. insufficient resources.
	*/
	SERVICE_CmGateway& Provider();

private:
	// A local instance of a PROVIDER
	SERVICE_CmGateway *LocalProvider;

};

class CmGatewayConnection : public CmParallelFTL
{
public: 
	CmGatewayConnection();
	virtual ~CmGatewayConnection();

public:
	/** addToGatewayConnectionList.
	*  A gateway service will be added to a global service list
	*/
	bool addToGatewayConnectionList();

public:
	/** cleanupGatewayConnections.
	*  Gateway connections will be removed if they have finished.
	*/
	static bool cleanupGatewayConnections();

public:
	/** isMatchingGatewayConnection.
	*  The connection parameters will be checked against destination gateway.
	*/
	virtual bool isMatchingGatewayConnection(CmConnectionInfo& GatewayInfo);

	/** sendInfo.
	*  An information will be sent to remote gateway.
	*/
	virtual bool sendInfoToGateway(CmString& Info);


public:
	CmGatewayConnection *NextGatewayConnection;

};

} // namespace Cosmos

#endif   // #ifndef CmGatewayH


