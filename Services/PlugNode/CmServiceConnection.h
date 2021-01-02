//////////////////////////////////////////////////////////////////////////////
//
// CmServiceConnection.h: Declaration of CmServiceConnection classes
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

#ifndef CmServiceConnectionH
#define CmServiceConnectionH

#include "Services/PlugNode/CmCommunication.h"

namespace Cosmos
{

// Forward declarations
class CmServiceAccess;
class CmNetworkConnection;
class CmLocalConnection;
class CmStringFTL;

/** CmConnectionType
 *  This enumeration describes the type of a CmGateway connection with regard to
 *  its physical transfer medium as well as to the protocol that it implements.
 */
typedef enum
{
	CMCONNECTION_TYPE_NONE =  0,  // type not specified yet
	CMCONNECTION_TYPE_ADR  =  1,  // Connection in program's address space
	CMCONNECTION_TYPE_TCP  =  2,  // Internet TCP/IP network connection
  CMCONNECTION_TYPE_UDP  =  3,  // Internet UDP/IP network connection
  CMCONNECTION_TYPE_IP4  =  4,  // Internet IPv4 network connection
  CMCONNECTION_TYPE_IP6  =  5,  // Internet IPv6 network connection
  CMCONNECTION_TYPE_1394 =  6,  // 1394<->1394 fire wire network connection
  CMCONNECTION_TYPE_ETH  =  7,  // Ethernet network connection
  CMCONNECTION_TYPE_USB  =  8,  // USB<->USB cabel connection
  CMCONNECTION_TYPE_COM  =  9,  // COM<->COM serial connection
  CMCONNECTION_TYPE_LPT  = 10,  // LPT<->LPT parallel connection
  CMCONNECTION_TYPE_X25  = 11,  // modem/sound card connection
} CmConnectionType;

/** CMSTATE
*  This enum defines the state that a service connection may possess.
*/
enum CMSTATE { STATE_IDLE, STATE_CHECKIN, STATE_CONNECT, STATE_CONFIRM, STATE_DISCONNECT };

/** CMCONN
*  This enum denotes one of multiple possible connections that a CmServiceAccess
*  module may maintain locally or/and remotely to other service providers.
*/
enum CMCONN { CONN_NONE, CONN_1, CONN_2, CONN_3, CONN_4 };

/** CmConnectionInfo
 *  Connection information is defined by following union dependent on the type
 *  of socket that is actually used when connecting to peers
 */
typedef struct
{
  CmConnectionType ConnectionType;
  union
  {
    struct{
      CmServiceAccess *ContactAddress;
			int64	ContactID;
    } ADR;

    struct {
      uint32  DstHost;
      uint16  DstPort;
    } LAN;

    struct {
      uint32  DstHost;
    } IP4;

    struct {
      uint8 DstHost[16];
    } IP6;

    struct {
      uint8 DstMAC[6];
      uint8 SrcMAC[6];
    } ETH;

    struct {
      uint64 DstEUI64;
    } FireWire;

    struct {
    } USB;

    struct {
    } COM;

    struct {
    } LPT;

    struct {
    } X25;

  };
} CmConnectionInfo;

/** The following command tokens will be used for establishing, maintaining and
 *  for shutting down connections between PlugNodes:
 */
enum CMCOMMAND {
	CMCOMMAND_START				= 0,	// a new PlugNode is asked to checkin to a network of PlugNodes
	CMCOMMAND_CHECKIN			= 1,	// a PlugNode asks another PlugNode for establishing a connection
	CMCOMMAND_CONNECT			= 2,	// a new connection has been registered and is pending for CONFIRM
	CMCOMMAND_DISCONNECT	= 3,	// a PlugNode has shut down a connection
	CMCOMMAND_CONFIRM			= 4,	// a registered connection is confirmed and thereby activated
	CMCOMMAND_CONTACT			= 5,	// a PlugNode informs a new PlugNode about its current peers
	CMCOMMAND_SWITCH      = 6 	// a PlugNode asks a peer for switching over to a new PlugNode
};

/** CmServiceConnection
 *  The class maintains a connection to a peer service connection. It sends
 *  commands to its peer and receives confirmations. Further it receives
 *  messages from its peer (OSI: indications) and it allows to send responses.
 */
class CmServiceConnection : public CmParallelFTL
{
public:
  CmServiceConnection();
  virtual ~CmServiceConnection();

public:
	/** initServiceConnection. Set UURI and connection info */
	void initServiceConnection(const CmUURI *_ServiceUURI, CmConnectionInfo *_ConnectionInfo = NULL);

public:
	/** sendCommand.
	*   A command will be transferred to peer.
	*/
	bool sendCommand(CMCOMMAND _Command);
	bool sendCommand(CMCOMMAND _Command, CmServiceConnection *_ServiceConnection1);
	bool sendCommand(CMCOMMAND _Command, CmServiceConnection *_ServiceConnection1, CmServiceConnection *_ServiceConnection2);
	bool sendCommand(CMCOMMAND _Command, CmServiceConnection *_ServiceConnection1, CmServiceConnection *_ServiceConnection2, CmServiceConnection *_ServiceConnection3);

public:
	/** sendMessage.
   *  A message (text line) will be sent to the peer of this service connection.
	 *
	 * NOTE: Message must not be 'const' in order to be processed by readLine().
	 */
	bool sendMessage(CmString& _Message);

public:
	/** maintainNetwork.
	*  A network maintenance request will be processed on arrival. This funtion call
	*  will be forwarded to a CmServiceAccess class which is supposed to maintain a network.
	*
	* NOTE: Request must not be 'const' in order to be processed by readLine().
	*/
	bool maintainNetwork(CmString& _Request);

public:
	/** switchToIdle.
	*   A connection will be switched to idle, usually after receiving a SWITCH command.
	*/
	bool switchToIdle();

public:
	/** getConnectionID.
	*	  A ConnectionID will be returned that was registered together with a CommunicationReceiver.
	*/
	CMCONN getConnectionID();

public:
	/** isPeerServiceUURI.
   *  The peer's SERVICE UURI will be checked against a specified UURI. If the
   *  UURIs match then a pointer to RxData stream will be returned, otherwise NULL.
   */
	CmStream * isPeerServiceUURI(const CmUURI& _ServiceUURI);

public:
	/** checkin.
   *  A service connection will be established between two PlugNodes
   */
	void openConnection(const CmUURI& _NetworkUURI, CmServiceConnection *_PeerConnection, const CmConnectionInfo& _PeerContactInfo);

public:
	/** set/getPeerInfo.
	 *  The peer's connection information will be stored/retrieved
	 */
	void setPeerInfo(const CmConnectionInfo& _PeerInfo);
	CmServiceAccess *getPeerContactAddress();
	int64 getPeerContactID();

public:
	/** set/getState.
	*   The state of a connection will be stored/retrieved
	*/
	void setState(CMSTATE _ConnectionState);
	CMSTATE getState();
	CmString reportState();

public:
	/** disconnect.
   *  A two-way service connection will be shutdown
   */
  void disconnect();

public:
	/**  registerCommunicationReceiver.
	*   A communication receiver will be registered against RxInfo communication.
	*/
	bool registerCommunicationReceiver(CmCommunication* _CommunicationReceiver, CMCONN _ConnectionID);

public:
	/**  unregisterCommunicationReceiver.
	*   A registered communication receiver will be unregistered. The value for
	*   CommunicationReceiver has to match the value when registering 
	*   the callback. Otherwise the function will fail.
	*/
	bool unregisterCommunicationReceiver(CmCommunication* _CommunicationReceiver, CMCONN _ConnectionID);

//--------conversion-functions------------------------------------------------

public:
	/** ip2host/host2ip.
	*  An IP address xxx.xxx.xxx.xxx will be converted to int or vice versa.
	*/
	static uint32 ip2host(CmString IP);
	static CmString host2ip(uint32 Host);

public:
	// shutdown list
	CmServiceConnection* NextServiceConnection;

protected:
  // SERVICE identification
  const CmUURI *ServiceUURI;

  // connection information
	CMCONN ConnectionID;
	CMSTATE ConnectionState;
	CmConnectionInfo ConnectionInfo;
	CmCommunication* CommunicationReceiver;

private:
  // data streams
	CmCommunication *TxInfo;
	CmCommunication *RxInfo;

private:
	// command writer
	CmStringFTL *CommandX;
	CmStringFTL *CmdWriter();

private:
  // bookkeeping
  __int64 NextReadPosition;
  __int64 NextWritePosition;

private:
  // peer
  CmServiceConnection* Peer;
	CmConnectionInfo PeerInfo;
};

/** CmNetworkConnection.
 *  Service connections will be realized in a network that potentially might be
 *  the Internet or a local network connection.
 */
class CmNetworkConnection : CmServiceConnection
{
public:
	CmNetworkConnection(const CmUURI *_ServiceUURI, CmConnectionInfo *_ConnectionInfo);
  ~CmNetworkConnection();


private:
  // Peer contact data
  uint32 PeerIP;
  uint16 PeerPort;

};

/** CmLocalConnection.
 *  Service connections will be realized as pointers to program modules on the
 *  same PC in current process' address space.
 */
class CmLocalConnection : CmServiceConnection
{
public:
	CmLocalConnection(const CmUURI *_ServiceUURI);
  ~CmLocalConnection();


};



} // namespace Cosmos

#endif // !defined(CmServiceConnectionH)
