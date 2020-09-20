//////////////////////////////////////////////////////////////////////////////
//
// CmServiceAccess.h: Declaration of CmServiceAccess classes
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

#ifndef CmServiceAccessH
#define CmServiceAccessH

#include "Services/PlugNode/CmServiceConnection.h"

namespace Cosmos
{
// Forward declarations
class CmServiceAccess;
class CmStringFTL;
class SERVICE_CmGateway;

/** CMPEERS
 *  This enum describes to how many peers a CmServiceAccess module is currently
 *  connected to, either locally or remotely. Usually, a service access module
 *  starts with PEERS_0 after its instantiation. Afterwards, it will accept
 *  connections until all four service access points have been connected.
 *
 *  Further attempts to establish a new connection will provide to releasing
 *  (shutting down) an existing connection after asking appropriate peer to
 *  switch as well to the new service provider that is applying for integration
 *  into the network.
 */
enum CMPEERS { PEERS_0, PEERS_1, PEERS_2, PEERS_3, PEERS_4 };

/** CMCOMMAND
*   Initialization string of a command reader for network maintenance.
*/
#define CMCOMMAND_HEADER "EKD@JN58nc_Türkenfeld.Command\r\nToken,SequenceNumber,Command,Type1,Address1,Port1,Type2,Address2,Port2,@,1\r\n"
#define CMCOMMAND_SEQNUM "0-1-0"
#define CMCOMMAND_COMMAND "0-2-0"
#define CMCOMMAND_TYPE1 "0-3-0"
#define CMCOMMAND_ADDRESS1 "0-4-0"
#define CMCOMMAND_PORT1 "0-5-0"
#define CMCOMMAND_TYPE2 "0-6-0"
#define CMCOMMAND_ADDRESS2 "0-7-0"
#define CMCOMMAND_PORT2 "0-8-0"

/** CMMESSAGE
*   Initialization string of a message reader for network communication.
*/
#define CMMESSAGE_HEADER "EKD@JN58nc_Türkenfeld.Message\r\nConnectionInfo,MessageNumber,Message,@,2\r\n"
#define CMMESSAGE_CONNINFO "0-0-0"
#define CMMESSAGE_CONNINFO_2 "0-0-1"
#define CMMESSAGE_MSGNUM "0-1-0"
#define CMMESSAGE_MSGNUM_2 "0-1-1"
#define CMMESSAGE_MESSAGE "0-2-0"




/**
 * The CmServiceAccess module connects service PROVIDERs (program modules) in a
 * potentially large logical network as soon as those SERVICEs become available
 * on a PC, in the LAN or in the Internet.
 *
 * Every CmServiceAccess node will build up four connections (if possible),
 * that provide to other service providers.
 *
 * The CmServiceAccess module allows for sending queries to other service access
 * points. Because of the four connections, every query will be propagated
 * very fast to all service access points throughout the network.
 *
 * A direct connection will be established to all computers offering requested
 * data or services. A digital signature as well as encryption may be applied
 * on the level of a service connection. 
 */
class CmServiceAccess : public CmCommunication
{
public:
	//Constructor and destructor
	CmServiceAccess();
protected:
	~CmServiceAccess();

public:
	/** getContactID.
	*  A ConctactID will be returned that was received from a contact PLugNode when
	*  connecting to the local network. The contact IDs of connected peers will also
	*  be returned if valid Locals are present as well as the total peer count.
	*/
  int64	getContactID(int32* PeerCount = NULL, int64 *ContactID1 = NULL, int64 *ContactID2 = NULL, int64 *ContactID3 = NULL, int64 *ContactID4 = NULL, bool isContactPlugNode = false);


//-----local-network-maintenance-(protected)----------------------------------

protected:
	/** set/getServiceUURI.
	*  A UURI for the SERVICE that this ServiceAccess class represents will be
	*  saved or retrieved.
	*/
	virtual void setServiceUURI(const CmUURI *ServiceUURI);
public:
	const CmUURI * getServiceUURI();

protected:
	/** set/getNetworkUURI.
	*  A UURI for the network that this ServiceAccess class is a service access
	*  for will be saved or retrieved.
	*/
	void setNetworkUURI(const CmUURI& NetworkUURI);
public:
	const CmUURI& getNetworkUURI();

protected:
	/** setContactID.
	*  A unique ContactID will be set which usually is generated by ContactPlugNode
	*  during initialization.
	*/
	void setContactID(int64 ContactID);

protected:
	/** contactServiceAccess.
	*   A contact attempt will be made to another service access.
	*/
	bool contactServiceAccess();

protected:
	/** switchServiceAccess.
	*   The peer will be informed to switch to a new service access.
	*   Subsequently, the connection will be set idle.
	*/
	bool switchServiceAccess();

protected:
	/** processMessage.
	*  A message will be processed on arrival. This funtion is called from
	*  CmCommunication class. Usually, the content will be forwarded to a
	*  'processInformation()' function respectively to 'maintainNetwork()'.
	*
	* NOTE: Message must not be 'const' in order to be processed by readLine().
	*/
	bool processMessage(CmStream& Message, CmServiceConnection *ServiceConnection);

protected:
	/** maintainNetwork.
	*  A network maintenance request will be processed on arrival. The CmServiceAccess
	*  module is supposed to respond to every request for maintaining network structure.
	*/
	bool maintainNetwork(CmString& Request, CmServiceConnection *ServiceConnection, CMCONN ConnectionID, bool& isOffline);

protected:
	/** reconnectNetwork.
	*  A PlugNode applies for re-connecting to the network after it has got disconnected
	*  in the result of maintaining the network structure.
	*/
	virtual bool reconnectNetwork();

protected:
	/** sendMessage.
   *  A message (a text line) will be transferred to the peer site of a
   *  specified service connection
	 *
	 * NOTE: Message must not be 'const' in order to be processed by readLine().
	 */
	void sendMessage(CmString& Message, CMCONN Connection);

protected:
	/** getStream.
   *  A stream with a specified (peer) service UURI will be searched for. This
   *  will be the RxData stream of a CmServiceConnection which matches specified
   *  service UURI.
   *
   *  @param ServiceUURI the identification of the peer SERVICE
   *  @return a pointer to the RxData stream of desired ServiceConnection or NULL
   */
	CmStream *getStream(const CmUURI& ServiceUURI);

protected:
	/** disconnectFromPeers.
   *  This CmServiceAccess module will be disconnected from all peers.
   */
  void disconnectFromPeers();
  void disconnect(CMCONN ConnectionID);

protected:
	/** shutdown/delete/deleteIdleConnections.
	*  All connections will be shutdown, respectively they will be scheduled for
	*  shutdown at a later time if they are not idle yet. All idle connections
	*  will be deleted. Remaining connections will be deleted when the 
	*  ContactPlugNode goes down.
	*/
	bool shutdownConnections();
	bool deleteIdleConnections();
	static bool deleteConnections();

protected:
	/** findServiceUURI.
	 *  Run a lookup for specified ServiceUURI inside program's address space and
	 *  return a service access point that is able to provide desired service.
	 */
	CmServiceAccess * findServiceUURI(const CmUURI& ServiceUURI, uint64 SearchToken = 0);

protected:
	/** joinNetwork.
	*  A connection to a service network will be established.
	*
	*  @param NetworkUURI, the UURI of desired service network
	*  @param GatewayInfo, contact information for reaching a gateway in the service network
	*  @return succes or failed
	*/
	virtual bool joinNetwork(const CmUURI& NetworkUURI, CmConnectionInfo *GatewayInfo = NULL);

protected:
	/** setConnectionInfo.
	*  The connection info will be set for an existing network gateway.
	*/
	virtual bool setConnectionInfo(CmConnectionInfo& GatewayInfo);

protected:
	/** openConnection.
   *  Another service access point asks for opening a connection to this service access 
   *  point for a specified network UURI. This service access point will try to let
   *  the new participant in to current network if their network UURIs match.
   *
   *  @param NetworkUURI the desired network's identification (UURI)
   *  @param PeerConnection a potential peer service connection that requested a login
	 *  @param PeerContactInfo the peer's contact connection information
   *  @return the assigned local service connection or NULL to let the call fail
   */
	virtual CmServiceConnection * openConnection(const CmUURI& NetworkUURI, CmServiceConnection *PeerConnection, const CmConnectionInfo& PeerContactInfo);

protected:
	/** reportState.
	*   The state off all ServiceConnections will be reported
	*/
	void reportState(CmString& State, bool isContactPlugNode =false);

protected:
	/** increment/decrement/getPeers.
	*		The number of peer connections will be inc/dec/get respectively.
	*/
	void incrementPeers();
	void decrementPeers();
	int32 getPeers();

protected:
	/** sendInfo.
	*   A connection will be looked-up respectively established to specified
	*   network/service UURI. Subsequently, an information is sent to the
	*   recipient PlugNode if a connection is available.
	*/
	virtual bool sendInfo(const CmString& Info, const CmUURI& ServiceUURI, bool isControl = false, bool isFTLight = true, CmString* RecipientUURI =NULL);
	bool sendInfo(const CmString& Info, const CmUURI& ServiceUURI, const CmUURI& NetworkUURI, bool isControl = false, bool isFTLight = true, CmString* RecipientUURI = NULL);

//-----remote-network-maintenance-(private)-----------------------------------

private:
	/** createNetworkGateway
	*  A new CmServiceAccess will be created and loged in to specified network.
	*/
	bool createNetworkGateway(const CmUURI& NetworkUURI);

private:
	/** findNetworkUURI.
	*  Run a lookup for specified NetworkUURI inside ServiceAccess module and
	*  return a service access point to that network.
	*/
	CmServiceAccess * findNetworkUURI(const CmUURI& NetworkUURI);

private:
	/** queryNetworkForServiceUURI.
	*   A network will be searched for a specified ServiceUURI. If a service access
	*   with that ServiceUURI could be found then a connection will be established to it.
	*/
	bool queryNetworkForServiceUURI(const CmUURI& ServiceUURI);

	//-----workspace-(protected)--------------------------------------------------

protected:
	// Identification ID in a local network
	// This is a sequence number received from a contact PlugNode (first PlugNode)
	int64	ContactID;

protected:
	// Service network parameters
	CmUURI NetworkUURI;
	SERVICE_CmGateway *ServiceNetworkGateway;
	CmConnectionInfo ContactInfo;

protected:
	// Service connections to other PlugNodes
	CmServiceConnection *Connection1;
	CmServiceConnection *Connection2;
	CmServiceConnection *Connection3;
	CmServiceConnection *Connection4;
	CmServiceConnection *ConnectionSwitch;

//-----workspace-(private)----------------------------------------------------

private:
	// Identification of this SERVICE
  const CmUURI *ServiceUURI;

private:
	CMPEERS Peers;

private:
	// Next list member of network gateways
	CmServiceAccess *NetworkGateways;

private:
	// command reader
	CmStringFTL *CommandX;
	CmStringFTL *CmdReader();

private:
	// message reader
	CmStringFTL *MessageX;
	CmStringFTL *MsgReader();

private:
	// network search token
	uint64 SearchToken;

private:
	// recent information 
	CMCONN	RecentInfoConnection;
	CmServiceAccess *RecentInfoServiceNetwork;

};

/**
 *  CmNetworkService.
 *  This class supports access to network services (e.g. local or from Internet)
 */
#define CM_NETWORK_UURI_LAN_TCP					"EKD@JN58nc_Türkenfeld.LAN_TCP"
#define CM_NETWORK_UURI_LAN_UDP					"EKD@JN58nc_Türkenfeld.LAN_UDP"
#define CM_NETWORK_UURI_WEGALINK				"EKD@JN58nc_Türkenfeld.WegaLink"
#define CM_NETWORK_PORT_LAN_TCP					4225
#define CM_NETWORK_PORT_LAN_UDP					4226
#define CM_NETWORK_PORT_WEGALINK				CM_NETWORK_PORT_LAN_TCP

#define CM_SOCKET_NO_CONNECTION         0
#define CM_SOCKET_BIND_ANY_PORT         0
#define CM_SOCKET_DEFAULT_TIMEOUT       200
#define CM_SOCKET_DEFAULT_PORT_TCP      4226
#define CM_SOCKET_DEFAULT_PORT_UDP      4225
#define CM_SOCKET_DEFAULT_LOCALHOST     0x7F000001
#define CM_SOCKET_BIND_ALL_INTERFACES   0

} // namespace Cosmos

#endif // !defined(CmServiceAccessH)
