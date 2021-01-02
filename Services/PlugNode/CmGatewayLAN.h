//////////////////////////////////////////////////////////////////////////////
//
// CmGatewayLAN.cpp: Declaration of a CmGatewayLAN classes
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

#ifndef CmGatewayLANH
#define CmGatewayLANH

#include "Services/PlugNode/CmGateway.h"

namespace Cosmos
{
// NOTE: CmConnectionType and CmConnectionInfo have been defined in
//       CmServiceConnection.h in order to avoid compilation problems from
//       circle dependencies.

// UURI extensions for a 'CmGatewayTCP/UDP' PROVIDER
#define UURI_PROVIDER_CmGatewayTCP  "/TCP/built/2017-11-28"
#define UURI_PROVIDER_CmGatewayUDP  "/UDP/built/2017-11-28"

/** PROVIDER_CmGatewayTCP.
*  This is a provider for connecting to LAN/TCP
*/
class PROVIDER_CmGatewayTCP : public SERVICE_CmGateway
{
public:
	PROVIDER_CmGatewayTCP(CmConnectionInfo& _ConnectionInfo, const CmUURI& _NetworkUURI, const CmUURI& _ServiceUURI);
  ~PROVIDER_CmGatewayTCP();

//------Actual-service-implementation-for-a-PROVIDER_CmGatewayTCP-------------
public:
	CmUURI& getUURI();

public:
	bool joinNetwork(const CmUURI& _NetworkUURI, CmConnectionInfo *_ConnectionInfo);

public:
	bool findGateway(const CmUURI& _ServiceUURI, CmConnectionInfo& _GatewayInfo);

public:
	bool sendInfoToGateway(CmString& _Info, CmConnectionInfo& _GatewayInfo);

public:
	/** runParallel.
	*  Run a thread in background for accepting incoming connections.
	*/
	bool runParallel();

//------workspace-------------------------------------------------PROVIDER----

private:
	// network interface
	uint16 BindPort;
	uint32 Interface;

private:
	// network sockets
	SOCKET server_socket;
	SOCKET client_socket;
	SOCKADDR_IN server_addr;
	SOCKADDR_IN client_addr;

};


class CmGatewayConnectionLAN : public CmGatewayConnection
{
public:
	CmGatewayConnectionLAN(SERVICE_CmGateway* _Gateway, SOCKET _ConnectionSocket);
	~CmGatewayConnectionLAN();

public:
	/** runParallel.
	*  Run a thread in background for servicing a connection
	*/
	bool runParallel();

public:
	/** receiveData.
	*  Incoming data will be read from peer and be forwarded to the server.
	*/
	bool receiveData();

public:
	/** isMatchingGatewayConnection.
	*  The connection parameters will be checked against destination gateway.
	*/
	bool isMatchingGatewayConnection(CmConnectionInfo& _GatewayInfo);

	/** sendInfo.
	*  An information will be sent to remote gateway.
	*/
	bool sendInfoToGateway(CmString& _Info);

//------workspace-------------------------------------------------------------

private:
	// network socket
	SOCKET ConnectionSocket;
	SOCKADDR_IN RemoteAddr;

private:
	// Gateway
	SERVICE_CmGateway *Gateway;

};

} // namespace Cosmos

#endif   // #ifndef CmGatewayLANH


